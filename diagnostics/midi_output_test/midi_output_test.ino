#include <SPI.h>
#include <Adafruit_GFX.h>
#include <TFT_ILI9163C.h>
#include <MIDI.h>

#ifndef BLUE
#define BLUE 0x001F
#endif
#ifndef RED
#define RED 0xF800
#endif

constexpr byte TFT_DC = 10, TFT_CS = 9, TFT_RST = 12;
constexpr byte MUX_SELECT_PINS[4] = {5, 4, 3, 2};
constexpr byte MUX_SIGNAL = A0;

constexpr byte BUTTON_CHANNEL[12] = {0, 12, 6, 9, 8, 2, 14, 5, 4, 10, 1, 13};
constexpr byte POT_CHANNEL[4] = {3, 11, 7, 15};
constexpr byte NOTES[12] = {60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71};
constexpr byte POT_CC[4] = {20, 21, 22, 23};

constexpr byte MIDI_CHANNEL = 1;
constexpr byte NOTE_VELOCITY = 127;
constexpr int BUTTON_THRESHOLD = 512;
constexpr byte POT_MIDI_THRESHOLD = 2;
constexpr unsigned long DEBOUNCE_MS = 25;

TFT_ILI9163C tft(TFT_DC, TFT_CS, TFT_RST);
MIDI_CREATE_DEFAULT_INSTANCE();

bool rawButtonState[12];
bool stableButtonState[12];
unsigned long lastButtonChange[12];
int filteredPotValue[4];
byte lastPotMidiValue[4];
unsigned long eventCounter = 0;

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

void showEvent(const __FlashStringHelper* type, byte control, byte data1, byte data2) {
  ++eventCounter;
  tft.fillRect(0, 42, 128, 78, BLACK);
  tft.setTextColor(WHITE);
  tft.setTextSize(1);
  tft.setCursor(4, 45);
  tft.print(type);
  tft.print(control);
  tft.setCursor(4, 62);
  tft.print(F("Dato 1: "));
  tft.print(data1);
  tft.setCursor(4, 76);
  tft.print(F("Dato 2: "));
  tft.print(data2);
  tft.setCursor(4, 96);
  tft.print(F("Evento: "));
  tft.print(eventCounter);
}

void sendButtonEvent(byte button, bool pressed) {
  if (pressed) {
    MIDI.sendNoteOn(NOTES[button], NOTE_VELOCITY, MIDI_CHANNEL);
    showEvent(F("B ON  "), button + 1, NOTES[button], NOTE_VELOCITY);
  } else {
    MIDI.sendNoteOff(NOTES[button], 0, MIDI_CHANNEL);
    showEvent(F("B OFF "), button + 1, NOTES[button], 0);
  }
}

void updateButtons() {
  const unsigned long now = millis();
  for (byte button = 0; button < 12; ++button) {
    const bool pressed = readMux(BUTTON_CHANNEL[button]) < BUTTON_THRESHOLD;

    if (pressed != rawButtonState[button]) {
      rawButtonState[button] = pressed;
      lastButtonChange[button] = now;
    }

    if (pressed != stableButtonState[button] &&
        now - lastButtonChange[button] >= DEBOUNCE_MS) {
      stableButtonState[button] = pressed;
      sendButtonEvent(button, pressed);
    }
  }
}

void updatePots() {
  for (byte pot = 0; pot < 4; ++pot) {
    const int raw = readMux(POT_CHANNEL[pot]);
    filteredPotValue[pot] = (filteredPotValue[pot] * 3L + raw) / 4L;
    const byte midiValue = map(filteredPotValue[pot], 0, 1023, 0, 127);

    if (abs((int)midiValue - (int)lastPotMidiValue[pot]) >= POT_MIDI_THRESHOLD) {
      lastPotMidiValue[pot] = midiValue;
      MIDI.sendControlChange(POT_CC[pot], midiValue, MIDI_CHANNEL);
      showEvent(F("P CC  "), pot + 1, POT_CC[pot], midiValue);
    }
  }
}

void setup() {
  for (byte bit = 0; bit < 4; ++bit) pinMode(MUX_SELECT_PINS[bit], OUTPUT);
  pinMode(MUX_SIGNAL, INPUT);

  tft.begin();
  tft.setRotation(2);
  tft.fillScreen(BLACK);
  tft.setTextColor(WHITE);
  tft.setTextSize(1);
  tft.setCursor(4, 7);
  tft.println(F("PRUEBA MIDI OUT"));
  tft.setCursor(4, 22);
  tft.println(F("Canal MIDI: 1"));

  for (byte button = 0; button < 12; ++button) {
    const bool pressed = readMux(BUTTON_CHANNEL[button]) < BUTTON_THRESHOLD;
    rawButtonState[button] = pressed;
    stableButtonState[button] = pressed;
    lastButtonChange[button] = 0;
  }

  for (byte pot = 0; pot < 4; ++pot) {
    filteredPotValue[pot] = readMux(POT_CHANNEL[pot]);
    lastPotMidiValue[pot] = map(filteredPotValue[pot], 0, 1023, 0, 127);
  }

  // Configura TX a la velocidad MIDI estandar de 31250 bit/s.
  MIDI.begin(MIDI_CHANNEL_OFF);
}

void loop() {
  updateButtons();
  updatePots();
}

