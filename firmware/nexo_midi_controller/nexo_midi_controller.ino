/*
 * Nexo MIDI Controller v1.2.1
 * Trabajo final - Complementos de Analogica 2
 * Ingenieria Electronica, Universidad Nacional de San Juan
 * Autores: Marcelo Cuello y Leonardo Pezzini
 */

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <TFT_ILI9163C.h>
#include <EEPROM.h>
#include <MIDI.h>

#ifndef BLUE
#define BLUE 0x001F
#endif
#ifndef RED
#define RED 0xF800
#endif

constexpr byte TFT_DC = 10, TFT_CS = 9, TFT_RST = 12;
constexpr byte ENC_CLK = 8, ENC_DT = 7, ENC_SW = 6;
constexpr byte MUX_SELECT_PINS[4] = {5, 4, 3, 2};
constexpr byte MUX_SIGNAL = A0;
constexpr byte BATTERY_PIN = A1;

constexpr byte BUTTON_CHANNEL[12] = {0, 12, 6, 9, 8, 2, 14, 5, 4, 10, 1, 13};
constexpr byte POT_CHANNEL[4] = {3, 11, 7, 15};

constexpr byte PRESET_COUNT = 8;
constexpr uint16_t EEPROM_MAGIC = 0x4D43; // "MC"
constexpr byte EEPROM_VERSION = 2;
constexpr int BUTTON_THRESHOLD = 512;
constexpr unsigned long BUTTON_DEBOUNCE_MS = 25;
constexpr unsigned long ENCODER_DEBOUNCE_MS = 30;
constexpr byte POT_MIDI_THRESHOLD = 2;
constexpr unsigned long BATTERY_READ_MS = 1000;
constexpr uint16_t ADC_REFERENCE_MV = 5000;
constexpr uint16_t BATTERY_CALIBRATION_PERMILLE = 1000;

struct Preset {
  byte notes[12];
  byte noteChannels[12];
  byte controlChange[4];
  byte ccChannels[4];
};

struct PersistentData {
  uint16_t magic;
  byte version;
  byte activePreset;
  byte enabledPresets;
  Preset presets[PRESET_COUNT];
};

enum class ScreenMode : byte {
  PLAY,
  MAIN_MENU,
  PRESET_SELECT,
  PRESET_MANAGER,
  PRESET_CONFIRM,
  EDIT_GRID,
  EDIT_DETAIL
};

enum class PresetAction : byte {
  NONE,
  RESET,
  DELETE
};

TFT_ILI9163C tft(TFT_DC, TFT_CS, TFT_RST);
MIDI_CREATE_DEFAULT_INSTANCE();
PersistentData data;
ScreenMode mode = ScreenMode::PLAY;

bool rawButtonState[12], stableButtonState[12], displayedButtonState[12];
unsigned long lastButtonChange[12];
int filteredPotValue[4], displayedPotValue[4];
byte lastPotMidiValue[4];
uint16_t batteryMillivolts = 0;
unsigned long lastBatteryRead = 0;
byte displayedBatteryLevel = 255;
byte pendingBatteryLevel = 255;
byte pendingBatteryReadings = 0;

byte encoderState = 0;
int8_t encoderAccumulator = 0;
int8_t encoderDelta = 0;
bool lastEncoderSwitch = HIGH;
unsigned long lastSwitchChange = 0;
bool encoderPressed = false;

byte menuSelection = 0;
byte pendingPreset = 0;
byte managerSelection = 0;
PresetAction pendingAction = PresetAction::NONE;
const __FlashStringHelper* managerMessage = nullptr;
Preset editPreset;
byte editSelection = 0; // P1-P4, B1-B12, X, V
byte detailField = 0;   // mensaje, canal, volver
bool detailAdjusting = false;
bool screenDirty = true;

uint16_t readBatteryMillivolts() {
  // Descarta la primera conversion despues de leer el multiplexor por A0.
  analogRead(BATTERY_PIN);
  uint16_t sum = 0;
  for (byte sample = 0; sample < 8; ++sample) {
    sum += analogRead(BATTERY_PIN);
  }
  const uint16_t average = sum / 8;

  // Divisor 10k/10k: la tension real es el doble de la medida en A1.
  // Divide antes de aplicar la calibracion para evitar desbordar uint32_t.
  uint32_t millivolts = (uint32_t)average * ADC_REFERENCE_MV * 2UL / 1023UL;
  millivolts = millivolts * BATTERY_CALIBRATION_PERMILLE / 1000UL;
  return millivolts;
}

void printBatteryVoltage() {
  tft.print(batteryMillivolts / 1000);
  tft.print('.');
  const uint16_t decimals = (batteryMillivolts % 1000) / 10;
  if (decimals < 10) tft.print('0');
  tft.print(decimals);
  tft.print('V');
}

byte batteryLevelFromMillivolts(uint16_t millivolts) {
  if (millivolts >= 4050) return 4;
  if (millivolts >= 3850) return 3;
  if (millivolts >= 3650) return 2;
  if (millivolts >= 3450) return 1;
  return 0;
}

void drawBatteryIndicator(byte level) {
  const int x = 109, y = 6;
  tft.fillRect(x, y, 18, 10, BLACK);
  tft.drawRect(x, y, 15, 9, level == 0 ? RED : WHITE);
  tft.fillRect(x + 15, y + 3, 2, 3, level == 0 ? RED : WHITE);

  for (byte segment = 0; segment < 4; ++segment) {
    const uint16_t color = segment < level ? (level == 1 ? RED : BLUE) : BLACK;
    tft.fillRect(x + 2 + segment * 3, y + 2, 2, 5, color);
  }

  tft.fillRect(3, 108, 70, 11, BLACK);
  tft.setTextSize(1);
  if (level == 0) {
    tft.setTextColor(RED);
    tft.setCursor(3, 110);
    tft.print(F("BAT BAJA"));
  } else {
    tft.setTextColor(WHITE);
    tft.setCursor(3, 110);
    tft.print(F("SW: MENU"));
  }
  displayedBatteryLevel = level;
}

void updateBatteryIndicator() {
  if (millis() - lastBatteryRead < BATTERY_READ_MS) return;
  lastBatteryRead = millis();
  batteryMillivolts = readBatteryMillivolts();
  const byte newLevel = batteryLevelFromMillivolts(batteryMillivolts);

  // Exige tres lecturas consecutivas para evitar cambios por ruido o picos.
  if (newLevel != pendingBatteryLevel) {
    pendingBatteryLevel = newLevel;
    pendingBatteryReadings = 1;
  } else if (pendingBatteryReadings < 3) {
    ++pendingBatteryReadings;
  }

  if (pendingBatteryReadings >= 3 && newLevel != displayedBatteryLevel &&
      mode == ScreenMode::PLAY) {
    drawBatteryIndicator(newLevel);
  }
}

void setDefaultPreset(Preset& preset) {
  for (byte button = 0; button < 12; ++button) {
    preset.notes[button] = 60 + button;
    preset.noteChannels[button] = 1;
  }
  for (byte pot = 0; pot < 4; ++pot) {
    preset.controlChange[pot] = 20 + pot;
    preset.ccChannels[pot] = 1;
  }
}

bool presetExists(byte preset) {
  return bitRead(data.enabledPresets, preset);
}

byte countPresets() {
  byte count = 0;
  for (byte preset = 0; preset < PRESET_COUNT; ++preset) {
    if (presetExists(preset)) ++count;
  }
  return count;
}

int8_t findFreePreset() {
  for (byte preset = 0; preset < PRESET_COUNT; ++preset) {
    if (!presetExists(preset)) return preset;
  }
  return -1;
}

byte nextExistingPreset(byte current, int8_t direction) {
  byte candidate = current;
  do {
    candidate = (candidate + (direction > 0 ? 1 : PRESET_COUNT - 1)) % PRESET_COUNT;
  } while (!presetExists(candidate));
  return candidate;
}

void saveMetadata() {
  EEPROM.update(offsetof(PersistentData, activePreset), data.activePreset);
  EEPROM.update(offsetof(PersistentData, enabledPresets), data.enabledPresets);
}

void savePreset(byte preset) {
  const int address = offsetof(PersistentData, presets) + preset * sizeof(Preset);
  EEPROM.put(address, data.presets[preset]);
}

void selectMuxChannel(byte channel) {
  for (byte bit = 0; bit < 4; ++bit) {
    digitalWrite(MUX_SELECT_PINS[bit], bitRead(channel, bit));
  }
  delayMicroseconds(8);
}

int readMux(byte channel) {
  selectMuxChannel(channel);
  analogRead(MUX_SIGNAL);
  return analogRead(MUX_SIGNAL);
}

void createDefaultData() {
  data.magic = EEPROM_MAGIC;
  data.version = EEPROM_VERSION;
  data.activePreset = 0;
  data.enabledPresets = bit(0);

  for (byte preset = 0; preset < PRESET_COUNT; ++preset) {
    setDefaultPreset(data.presets[preset]);
  }
  EEPROM.put(0, data);
}

void loadData() {
  EEPROM.get(0, data);
  if (data.magic != EEPROM_MAGIC || data.version != EEPROM_VERSION ||
      data.activePreset >= PRESET_COUNT || data.enabledPresets == 0 ||
      !presetExists(data.activePreset)) {
    createDefaultData();
  }
}

void saveActivePresetNumber() {
  EEPROM.update(offsetof(PersistentData, activePreset), data.activePreset);
}

void saveEditedPreset() {
  data.presets[data.activePreset] = editPreset;
  savePreset(data.activePreset);
}

void readEncoder() {
  static const int8_t TRANSITION[16] = {
     0, -1,  1,  0,
     1,  0,  0, -1,
    -1,  0,  0,  1,
     0,  1, -1,  0
  };

  const byte current = (digitalRead(ENC_CLK) << 1) | digitalRead(ENC_DT);
  encoderAccumulator += TRANSITION[(encoderState << 2) | current];
  encoderState = current;

  if (encoderAccumulator >= 2) {
    encoderAccumulator = 0;
    if (encoderDelta < 10) ++encoderDelta;
  } else if (encoderAccumulator <= -2) {
    encoderAccumulator = 0;
    if (encoderDelta > -10) --encoderDelta;
  }

  const bool switchState = digitalRead(ENC_SW);
  if (switchState != lastEncoderSwitch &&
      millis() - lastSwitchChange >= ENCODER_DEBOUNCE_MS) {
    lastSwitchChange = millis();
    lastEncoderSwitch = switchState;
    if (switchState == LOW) encoderPressed = true;
  }
}

int8_t takeEncoderDelta() {
  const int8_t result = encoderDelta;
  encoderDelta = 0;
  return result;
}

bool takeEncoderPress() {
  const bool result = encoderPressed;
  encoderPressed = false;
  return result;
}

void resyncControls() {
  for (byte button = 0; button < 12; ++button) {
    const bool pressed = readMux(BUTTON_CHANNEL[button]) < BUTTON_THRESHOLD;
    rawButtonState[button] = pressed;
    stableButtonState[button] = pressed;
    displayedButtonState[button] = !pressed;
    lastButtonChange[button] = millis();
  }
  for (byte pot = 0; pot < 4; ++pot) {
    filteredPotValue[pot] = readMux(POT_CHANNEL[pot]);
    lastPotMidiValue[pot] = map(filteredPotValue[pot], 0, 1023, 0, 127);
    displayedPotValue[pot] = -100;
  }
}

void stopAllNotes() {
  Preset& preset = data.presets[data.activePreset];
  for (byte button = 0; button < 12; ++button) {
    if (stableButtonState[button]) {
      MIDI.sendNoteOff(preset.notes[button], 0, preset.noteChannels[button]);
    }
  }
}

void enterMode(ScreenMode nextMode) {
  if (mode == ScreenMode::PLAY && nextMode != ScreenMode::PLAY) stopAllNotes();
  mode = nextMode;
  screenDirty = true;
  if (mode == ScreenMode::PLAY) resyncControls();
}

void drawPlayBase() {
  tft.fillScreen(BLACK);
  tft.setTextSize(1);
  tft.setTextColor(WHITE);
  tft.setCursor(4, 7);
  tft.print(F("PRESET "));
  tft.print(data.activePreset + 1);
  tft.setCursor(76, 7);
  tft.print(F("PLAY"));
  tft.setCursor(3, 110);
  tft.print(F("SW: MENU"));
  for (byte i = 0; i < 12; ++i) displayedButtonState[i] = !stableButtonState[i];
  for (byte i = 0; i < 4; ++i) displayedPotValue[i] = -100;
  displayedBatteryLevel = 255;
  pendingBatteryLevel = batteryLevelFromMillivolts(batteryMillivolts);
  pendingBatteryReadings = 3;
  drawBatteryIndicator(pendingBatteryLevel);
}

void drawPlayButton(byte button) {
  const byte row = button / 4, column = button % 4;
  const int x = 7 + column * 30, y = 48 + row * 18;
  const bool pressed = stableButtonState[button];
  tft.fillRect(x, y, 17, 13, pressed ? BLUE : BLACK);
  tft.drawRect(x, y, 17, 13, WHITE);
  tft.setTextSize(1);
  tft.setTextColor(WHITE);
  tft.setCursor(x + (button < 9 ? 6 : 3), y + 3);
  tft.print(button + 1);
  displayedButtonState[button] = pressed;
}

void drawPlayPot(byte pot) {
  const int value = filteredPotValue[pot];
  const int x = 4 + pot * 31, y = 25, width = 26, height = 15;
  const int barWidth = map(value, 0, 1023, 0, width - 4);
  tft.drawRect(x, y, width, height, RED);
  tft.fillRect(x + 2, y + 2, width - 4, height - 4, BLACK);
  tft.fillRect(x + 2, y + 2, barWidth, height - 4, RED);
  displayedPotValue[pot] = value;
}

void updatePlayControls() {
  const unsigned long now = millis();
  Preset& preset = data.presets[data.activePreset];

  for (byte button = 0; button < 12; ++button) {
    const bool pressed = readMux(BUTTON_CHANNEL[button]) < BUTTON_THRESHOLD;
    if (pressed != rawButtonState[button]) {
      rawButtonState[button] = pressed;
      lastButtonChange[button] = now;
    }
    if (pressed != stableButtonState[button] &&
        now - lastButtonChange[button] >= BUTTON_DEBOUNCE_MS) {
      stableButtonState[button] = pressed;
      if (pressed) MIDI.sendNoteOn(preset.notes[button], 127, preset.noteChannels[button]);
      else MIDI.sendNoteOff(preset.notes[button], 0, preset.noteChannels[button]);
    }
    if (stableButtonState[button] != displayedButtonState[button]) drawPlayButton(button);
  }

  for (byte pot = 0; pot < 4; ++pot) {
    const int raw = readMux(POT_CHANNEL[pot]);
    filteredPotValue[pot] = (filteredPotValue[pot] * 3L + raw) / 4L;
    const byte midiValue = map(filteredPotValue[pot], 0, 1023, 0, 127);
    if (abs((int)midiValue - (int)lastPotMidiValue[pot]) >= POT_MIDI_THRESHOLD) {
      lastPotMidiValue[pot] = midiValue;
      MIDI.sendControlChange(preset.controlChange[pot], midiValue, preset.ccChannels[pot]);
    }
    if (abs(filteredPotValue[pot] - displayedPotValue[pot]) >= 8) drawPlayPot(pot);
  }
}

void drawMainMenu() {
  const __FlashStringHelper* items[4] = {
    F("VOLVER A PLAY"), F("ELEGIR PRESET"), F("EDITAR PRESET"), F("GESTIONAR PRESETS")
  };
  tft.fillScreen(BLACK);
  tft.setTextSize(1);
  tft.setTextColor(WHITE);
  tft.setCursor(4, 7);
  tft.println(F("MENU PRINCIPAL"));
  for (byte i = 0; i < 4; ++i) {
    tft.setTextColor(i == menuSelection ? BLUE : WHITE);
    tft.setCursor(8, 29 + i * 20);
    tft.print(i == menuSelection ? F("> ") : F("  "));
    tft.print(items[i]);
  }
  tft.setTextColor(WHITE);
  tft.setCursor(8, 111);
  tft.print(F("BAT: "));
  printBatteryVoltage();
}

void updateMainMenu() {
  const int8_t delta = takeEncoderDelta();
  if (delta != 0) {
    int next = menuSelection + delta;
    while (next < 0) next += 4;
    menuSelection = next % 4;
    drawMainMenu();
  }
  if (takeEncoderPress()) {
    if (menuSelection == 0) enterMode(ScreenMode::PLAY);
    else if (menuSelection == 1) {
      pendingPreset = data.activePreset;
      enterMode(ScreenMode::PRESET_SELECT);
    } else if (menuSelection == 2) {
      editPreset = data.presets[data.activePreset];
      editSelection = 0;
      enterMode(ScreenMode::EDIT_GRID);
    } else {
      managerSelection = 0;
      managerMessage = nullptr;
      enterMode(ScreenMode::PRESET_MANAGER);
    }
  }
}

void drawPresetSelect() {
  tft.fillScreen(BLACK);
  tft.setTextSize(1);
  tft.setTextColor(WHITE);
  tft.setCursor(4, 7);
  tft.println(F("ELEGIR PRESET"));
  tft.setTextSize(3);
  tft.setTextColor(BLUE);
  tft.setCursor(53, 48);
  tft.print(pendingPreset + 1);
  tft.setTextSize(1);
  tft.setTextColor(WHITE);
  tft.setCursor(13, 100);
  tft.print(F("PULSAR PARA USAR"));
}

void updatePresetSelect() {
  const int8_t delta = takeEncoderDelta();
  if (delta != 0) {
    const int8_t direction = delta > 0 ? 1 : -1;
    for (byte step = 0; step < abs(delta); ++step) {
      pendingPreset = nextExistingPreset(pendingPreset, direction);
    }
    drawPresetSelect();
  }
  if (takeEncoderPress()) {
    data.activePreset = pendingPreset;
    saveActivePresetNumber();
    enterMode(ScreenMode::PLAY);
  }
}

void drawPresetManager() {
  const __FlashStringHelper* items[5] = {
    F("CREAR NUEVO"), F("DUPLICAR ACTUAL"), F("RESTABLECER"), F("BORRAR ACTUAL"), F("VOLVER")
  };
  tft.fillScreen(BLACK);
  tft.setTextSize(1);
  tft.setTextColor(WHITE);
  tft.setCursor(4, 5);
  tft.print(F("PRESETS "));
  tft.print(countPresets());
  tft.print('/');
  tft.print(PRESET_COUNT);
  for (byte i = 0; i < 5; ++i) {
    tft.setTextColor(i == managerSelection ? BLUE : WHITE);
    tft.setCursor(5, 23 + i * 17);
    tft.print(i == managerSelection ? F("> ") : F("  "));
    tft.print(items[i]);
  }
  if (managerMessage != nullptr) {
    tft.setTextColor(RED);
    tft.setCursor(5, 112);
    tft.print(managerMessage);
  }
}

void activateNewPreset(byte preset) {
  bitSet(data.enabledPresets, preset);
  data.activePreset = preset;
  savePreset(preset);
  saveMetadata();
  managerMessage = F("PRESET CREADO");
}

void updatePresetManager() {
  const int8_t delta = takeEncoderDelta();
  if (delta != 0) {
    int next = managerSelection + delta;
    while (next < 0) next += 5;
    managerSelection = next % 5;
    managerMessage = nullptr;
    drawPresetManager();
  }
  if (!takeEncoderPress()) return;

  if (managerSelection == 4) {
    enterMode(ScreenMode::MAIN_MENU);
    return;
  }
  if (managerSelection == 0 || managerSelection == 1) {
    const int8_t freePreset = findFreePreset();
    if (freePreset < 0) {
      managerMessage = F("SIN ESPACIO");
    } else {
      if (managerSelection == 0) setDefaultPreset(data.presets[freePreset]);
      else data.presets[freePreset] = data.presets[data.activePreset];
      activateNewPreset(freePreset);
      managerMessage = managerSelection == 0 ? F("PRESET CREADO") : F("PRESET DUPLICADO");
    }
    drawPresetManager();
    return;
  }

  pendingAction = managerSelection == 2 ? PresetAction::RESET : PresetAction::DELETE;
  if (pendingAction == PresetAction::DELETE && countPresets() == 1) {
    managerMessage = F("NO ES POSIBLE");
    drawPresetManager();
  } else {
    enterMode(ScreenMode::PRESET_CONFIRM);
  }
}

void drawPresetConfirm() {
  tft.fillScreen(BLACK);
  tft.setTextSize(1);
  tft.setTextColor(WHITE);
  tft.setCursor(5, 16);
  tft.print(pendingAction == PresetAction::RESET ? F("RESTABLECER PRESET ") : F("BORRAR PRESET "));
  tft.print(data.activePreset + 1);
  tft.setCursor(5, 43);
  tft.print(F("SE PERDERAN DATOS"));
  tft.setTextColor(BLUE);
  tft.setCursor(5, 75);
  tft.print(F("GIRAR: CANCELAR"));
  tft.setTextColor(RED);
  tft.setCursor(5, 96);
  tft.print(F("PULSAR: CONFIRMAR"));
}

void updatePresetConfirm() {
  if (takeEncoderDelta() != 0) {
    pendingAction = PresetAction::NONE;
    enterMode(ScreenMode::PRESET_MANAGER);
    return;
  }
  if (!takeEncoderPress()) return;

  if (pendingAction == PresetAction::RESET) {
    setDefaultPreset(data.presets[data.activePreset]);
    savePreset(data.activePreset);
    managerMessage = F("RESTABLECIDO");
  } else if (pendingAction == PresetAction::DELETE) {
    bitClear(data.enabledPresets, data.activePreset);
    data.activePreset = nextExistingPreset(data.activePreset, 1);
    saveMetadata();
    managerMessage = F("PRESET BORRADO");
  }
  pendingAction = PresetAction::NONE;
  enterMode(ScreenMode::PRESET_MANAGER);
}

void drawEditGrid() {
  tft.fillScreen(BLACK);
  tft.setTextSize(1);
  tft.setTextColor(WHITE);
  tft.setCursor(4, 7);
  tft.print(F("EDITAR PRESET "));
  tft.print(data.activePreset + 1);

  for (byte pot = 0; pot < 4; ++pot) {
    const int x = 7 + pot * 30, y = 25;
    tft.fillRect(x, y, 17, 13, RED);
    tft.drawRect(x, y, 17, 13, editSelection == pot ? BLUE : RED);
    tft.setTextColor(WHITE);
    tft.setCursor(x + 6, y + 3);
    tft.print(pot + 1);
  }

  for (byte button = 0; button < 12; ++button) {
    const byte row = button / 4, column = button % 4;
    const int x = 7 + column * 30, y = 47 + row * 18;
    tft.fillRect(x, y, 17, 13, BLACK);
    tft.drawRect(x, y, 17, 13, editSelection == button + 4 ? BLUE : WHITE);
    tft.setTextColor(WHITE);
    tft.setCursor(x + (button < 9 ? 6 : 3), y + 3);
    tft.print(button + 1);
  }

  tft.setTextColor(editSelection == 16 ? BLUE : WHITE);
  tft.setCursor(28, 108);
  tft.print(F("X"));
  tft.setTextColor(editSelection == 17 ? BLUE : WHITE);
  tft.setCursor(92, 108);
  tft.print(F("V"));
}

void updateEditGrid() {
  const int8_t delta = takeEncoderDelta();
  if (delta != 0) {
    int next = editSelection + delta;
    while (next < 0) next += 18;
    editSelection = next % 18;
    drawEditGrid();
  }

  if (!takeEncoderPress()) return;
  if (editSelection < 16) {
    detailField = 0;
    detailAdjusting = false;
    enterMode(ScreenMode::EDIT_DETAIL);
  } else if (editSelection == 16) {
    enterMode(ScreenMode::MAIN_MENU); // Descarta la copia de edicion.
  } else {
    saveEditedPreset();
    enterMode(ScreenMode::MAIN_MENU);
  }
}

void printNoteName(byte note) {
  switch (note % 12) {
    case 0: tft.print(F("C")); break;
    case 1: tft.print(F("C#")); break;
    case 2: tft.print(F("D")); break;
    case 3: tft.print(F("D#")); break;
    case 4: tft.print(F("E")); break;
    case 5: tft.print(F("F")); break;
    case 6: tft.print(F("F#")); break;
    case 7: tft.print(F("G")); break;
    case 8: tft.print(F("G#")); break;
    case 9: tft.print(F("A")); break;
    case 10: tft.print(F("A#")); break;
    default: tft.print(F("B")); break;
  }
  tft.print((int)(note / 12) - 1);
}

void drawEditDetail() {
  const bool isPot = editSelection < 4;
  const byte index = isPot ? editSelection : editSelection - 4;
  const byte message = isPot ? editPreset.controlChange[index] : editPreset.notes[index];
  const byte channel = isPot ? editPreset.ccChannels[index] : editPreset.noteChannels[index];

  tft.fillScreen(BLACK);
  tft.setTextSize(1);
  tft.setTextColor(WHITE);
  tft.setCursor(4, 7);
  tft.print(isPot ? F("POTENCIOMETRO ") : F("BOTON "));
  tft.print(index + 1);

  tft.setTextColor(detailField == 0 ? BLUE : WHITE);
  tft.setCursor(8, 36);
  tft.print(isPot ? F("CC: ") : F("NOTA: "));
  if (isPot) tft.print(message);
  else {
    printNoteName(message);
    tft.print(F("  ("));
    tft.print(message);
    tft.print(')');
  }

  tft.setTextColor(detailField == 1 ? BLUE : WHITE);
  tft.setCursor(8, 59);
  tft.print(F("CANAL: "));
  tft.print(channel);

  tft.setTextColor(detailField == 2 ? BLUE : WHITE);
  tft.setCursor(8, 82);
  tft.print(F("VOLVER"));

  tft.setTextColor(detailAdjusting ? RED : WHITE);
  tft.setCursor(8, 108);
  tft.print(detailAdjusting ? F("GIRAR: CAMBIAR") : F("PULSAR: ELEGIR"));
}

void changeDetailValue(int8_t delta) {
  const bool isPot = editSelection < 4;
  const byte index = isPot ? editSelection : editSelection - 4;

  if (detailField == 0) {
    byte& value = isPot ? editPreset.controlChange[index] : editPreset.notes[index];
    int next = value + delta;
    while (next < 0) next += 128;
    value = next % 128;
  } else if (detailField == 1) {
    byte& channel = isPot ? editPreset.ccChannels[index] : editPreset.noteChannels[index];
    int next = (channel - 1) + delta;
    while (next < 0) next += 16;
    channel = (next % 16) + 1;
  }
}

void updateEditDetail() {
  const int8_t delta = takeEncoderDelta();
  if (delta != 0) {
    if (detailAdjusting) changeDetailValue(delta);
    else {
      int next = detailField + delta;
      while (next < 0) next += 3;
      detailField = next % 3;
    }
    drawEditDetail();
  }

  if (!takeEncoderPress()) return;
  if (detailField == 2 && !detailAdjusting) {
    enterMode(ScreenMode::EDIT_GRID);
  } else {
    detailAdjusting = !detailAdjusting;
    drawEditDetail();
  }
}

void setup() {
  for (byte bit = 0; bit < 4; ++bit) pinMode(MUX_SELECT_PINS[bit], OUTPUT);
  pinMode(MUX_SIGNAL, INPUT);
  pinMode(BATTERY_PIN, INPUT);
  pinMode(ENC_CLK, INPUT);
  pinMode(ENC_DT, INPUT);
  pinMode(ENC_SW, INPUT_PULLUP);
  encoderState = (digitalRead(ENC_CLK) << 1) | digitalRead(ENC_DT);

  tft.begin();
  tft.setRotation(2);
  loadData();
  resyncControls();
  batteryMillivolts = readBatteryMillivolts();
  lastBatteryRead = millis();
  MIDI.begin(MIDI_CHANNEL_OFF);
}

void loop() {
  readEncoder();

  if (screenDirty) {
    screenDirty = false;
    if (mode == ScreenMode::PLAY) drawPlayBase();
    else if (mode == ScreenMode::MAIN_MENU) drawMainMenu();
    else if (mode == ScreenMode::PRESET_SELECT) drawPresetSelect();
    else if (mode == ScreenMode::PRESET_MANAGER) drawPresetManager();
    else if (mode == ScreenMode::PRESET_CONFIRM) drawPresetConfirm();
    else if (mode == ScreenMode::EDIT_GRID) drawEditGrid();
    else drawEditDetail();
  }

  if (mode == ScreenMode::PLAY) {
    takeEncoderDelta();
    updatePlayControls();
    updateBatteryIndicator();
    if (takeEncoderPress()) {
      menuSelection = 0;
      enterMode(ScreenMode::MAIN_MENU);
    }
  } else if (mode == ScreenMode::MAIN_MENU) {
    updateMainMenu();
  } else if (mode == ScreenMode::PRESET_SELECT) {
    updatePresetSelect();
  } else if (mode == ScreenMode::PRESET_MANAGER) {
    updatePresetManager();
  } else if (mode == ScreenMode::PRESET_CONFIRM) {
    updatePresetConfirm();
  } else if (mode == ScreenMode::EDIT_GRID) updateEditGrid();
  else updateEditDetail();
}
