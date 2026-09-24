#include <SPI.h>
#include <Adafruit_GFX.h>
#include <TFT_ILI9163C.h>
#include <MIDI.h>

constexpr byte TFT_DC = 10, TFT_CS = 9, TFT_RST = 12;
constexpr byte MIDI_CHANNEL = 1;
constexpr byte NOTE_C4 = 60;

TFT_ILI9163C tft(TFT_DC, TFT_CS, TFT_RST);
MIDI_CREATE_DEFAULT_INSTANCE();
unsigned long cycleCounter = 0;

void showState(const __FlashStringHelper* state) {
  tft.fillRect(0, 45, 128, 55, BLACK);
  tft.setTextColor(WHITE);
  tft.setTextSize(1);
  tft.setCursor(4, 48);
  tft.print(state);
  tft.setCursor(4, 66);
  tft.print(F("Ciclo: "));
  tft.print(cycleCounter);
}

void setup() {
  tft.begin();
  tft.setRotation(2);
  tft.fillScreen(BLACK);
  tft.setTextColor(WHITE);
  tft.setTextSize(1);
  tft.setCursor(4, 7);
  tft.println(F("MIDI LINK TEST"));
  tft.setCursor(4, 23);
  tft.println(F("Nota fija: 60"));

  // Inicializa la UART como MIDI DIN a 31250 bit/s.
  MIDI.begin(MIDI_CHANNEL_OFF);
}

void loop() {
  ++cycleCounter;
  MIDI.sendNoteOn(NOTE_C4, 127, MIDI_CHANNEL);
  showState(F("NOTE ON  90 3C 7F"));
  delay(250);

  MIDI.sendNoteOff(NOTE_C4, 0, MIDI_CHANNEL);
  showState(F("NOTE OFF 80 3C 00"));
  delay(750);
}

