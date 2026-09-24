#include <SPI.h>
#include <Adafruit_GFX.h>
#include <TFT_ILI9163C.h>

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

// Mapa fisico confirmado experimentalmente.
constexpr byte BUTTON_CHANNEL[12] = {0, 12, 6, 9, 8, 2, 14, 5, 4, 10, 1, 13};
constexpr byte POT_CHANNEL[4] = {3, 11, 7, 15};

constexpr int BUTTON_THRESHOLD = 512;
constexpr int POT_REDRAW_THRESHOLD = 8;
constexpr unsigned long DEBOUNCE_MS = 30;

TFT_ILI9163C tft(TFT_DC, TFT_CS, TFT_RST);
int muxValues[16];
int displayedPotValues[4] = {-100, -100, -100, -100};
bool displayedButtonStates[12];
byte selectedControl = 0;       // 0..11 botones; 12..15 potenciometros
byte previousSelectedControl = 255;
byte encoderState = 0;
int8_t encoderAccumulator = 0;
bool lastEncoderSwitch = HIGH;
unsigned long lastSwitchChange = 0;
unsigned int encoderPresses = 0;
unsigned int displayedEncoderPresses = 65535;
int displayedSelectedValue = -100;

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

void readAllControls() {
  for (byte channel = 0; channel < 16; ++channel) {
    muxValues[channel] = readMux(channel);
  }
}

byte selectedMuxChannel() {
  return selectedControl < 12
      ? BUTTON_CHANNEL[selectedControl]
      : POT_CHANNEL[selectedControl - 12];
}

void applyEncoderStep(int8_t direction) {
  previousSelectedControl = selectedControl;
  if (direction > 0) {
    selectedControl = (selectedControl + 1) % 16;
  } else {
    selectedControl = selectedControl == 0 ? 15 : selectedControl - 1;
  }
  displayedSelectedValue = -100;
}

void readEncoder() {
  // Tabla de transiciones Gray: ignora rebotes y cambios imposibles.
  static const int8_t TRANSITION[16] = {
     0, -1,  1,  0,
     1,  0,  0, -1,
    -1,  0,  0,  1,
     0,  1, -1,  0
  };

  const byte current = (digitalRead(ENC_CLK) << 1) | digitalRead(ENC_DT);
  encoderAccumulator += TRANSITION[(encoderState << 2) | current];
  encoderState = current;

  // Este encoder entrega dos transiciones validas por cada paso mecanico.
  if (encoderAccumulator >= 2) {
    encoderAccumulator = 0;
    applyEncoderStep(1);
  } else if (encoderAccumulator <= -2) {
    encoderAccumulator = 0;
    applyEncoderStep(-1);
  }

  const bool switchState = digitalRead(ENC_SW);
  if (switchState != lastEncoderSwitch &&
      millis() - lastSwitchChange >= DEBOUNCE_MS) {
    lastSwitchChange = millis();
    lastEncoderSwitch = switchState;
    if (switchState == LOW) ++encoderPresses;
  }
}

void drawHeader() {
  const int value = muxValues[selectedMuxChannel()];
  if (value == displayedSelectedValue &&
      selectedControl == previousSelectedControl) return;

  tft.fillRect(0, 5, 128, 13, BLACK);
  tft.setTextSize(1);
  tft.setTextColor(WHITE);
  tft.setCursor(3, 7); // Separado del borde superior por el offset del modulo.
  tft.print(selectedControl < 12 ? 'B' : 'P');
  tft.print((selectedControl < 12 ? selectedControl : selectedControl - 12) + 1);
  tft.print(F(" CH"));
  const byte channel = selectedMuxChannel();
  if (channel < 10) tft.print('0');
  tft.print(channel);
  tft.print(F(" ADC:"));
  tft.print(value);
  displayedSelectedValue = value;
}

void drawPot(byte pot, bool force) {
  const int value = muxValues[POT_CHANNEL[pot]];
  if (!force && abs(value - displayedPotValues[pot]) < POT_REDRAW_THRESHOLD) return;

  const int x = 4 + pot * 31, y = 25, width = 26, height = 15;
  const int barWidth = map(value, 0, 1023, 0, width - 4);
  const bool selected = selectedControl == pot + 12;
  tft.drawRect(x, y, width, height, selected ? BLUE : RED);
  tft.fillRect(x + 2, y + 2, width - 4, height - 4, BLACK);
  tft.fillRect(x + 2, y + 2, barWidth, height - 4, RED);
  displayedPotValues[pot] = value;
}

void drawButton(byte button, bool force) {
  const bool pressed = muxValues[BUTTON_CHANNEL[button]] < BUTTON_THRESHOLD;
  if (!force && pressed == displayedButtonStates[button]) return;

  const byte row = button / 4, column = button % 4;
  const int x = 7 + column * 30, y = 48 + row * 18;
  const bool selected = selectedControl == button;
  tft.fillRect(x, y, 17, 13, pressed ? BLUE : BLACK);
  tft.drawRect(x, y, 17, 13, selected ? RED : WHITE);
  tft.setTextSize(1);
  tft.setTextColor(WHITE);
  tft.setCursor(x + (button < 9 ? 6 : 3), y + 3);
  tft.print(button + 1);
  displayedButtonStates[button] = pressed;
}

void updateScreen() {
  const bool selectionChanged = selectedControl != previousSelectedControl;

  if (selectionChanged && previousSelectedControl < 16) {
    if (previousSelectedControl < 12) drawButton(previousSelectedControl, true);
    else drawPot(previousSelectedControl - 12, true);
  }

  for (byte pot = 0; pot < 4; ++pot) drawPot(pot, selectionChanged);
  for (byte button = 0; button < 12; ++button) drawButton(button, selectionChanged);
  drawHeader();

  if (encoderPresses != displayedEncoderPresses) {
    tft.fillRect(0, 108, 128, 15, BLACK);
    tft.setTextColor(WHITE);
    tft.setTextSize(1);
    tft.setCursor(3, 110);
    tft.print(F("Encoder SW: "));
    tft.print(encoderPresses);
    displayedEncoderPresses = encoderPresses;
  }

  previousSelectedControl = selectedControl;
}

void setup() {
  for (byte bit = 0; bit < 4; ++bit) pinMode(MUX_SELECT_PINS[bit], OUTPUT);
  pinMode(MUX_SIGNAL, INPUT);
  pinMode(ENC_CLK, INPUT);
  pinMode(ENC_DT, INPUT);
  pinMode(ENC_SW, INPUT_PULLUP);
  encoderState = (digitalRead(ENC_CLK) << 1) | digitalRead(ENC_DT);

  for (byte i = 0; i < 12; ++i) displayedButtonStates[i] = false;

  tft.begin();
  tft.setRotation(2);
  tft.fillScreen(BLACK);
  readAllControls();
  updateScreen();
}

void loop() {
  readEncoder();
  readAllControls();
  updateScreen();
}
