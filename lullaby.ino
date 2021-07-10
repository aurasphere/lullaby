#include "pitches.h"
#include <avr/eeprom.h>

#define SERIALIZED_EOS_CHAR '\n'
#define SERIALIZED_NOTE_LENGTH 3
#define SERIALIZED_HEADER_LENGTH 3
#define MAX_MELODY_LENGTH 64
// See protocol_handler for info on how this is calculated
#define MESSAGE_BUFFER_SIZE 1 + SERIALIZED_HEADER_LENGTH + (MAX_MELODY_LENGTH * SERIALIZED_NOTE_LENGTH)

#define PRINT_MENU false
#define BAUD_RATE 9600

// Button
#define BUTTON 6
#define BUZZER 7

// Leds
#define WHITE 8
#define BLUE 9
#define GREEN 10
#define YELLOW 11
#define RED 12

// Stored on flash memory to save RAM.
const uint16_t NOTES[] PROGMEM = {NOTE_C1, NOTE_D1, NOTE_E1, NOTE_F1, NOTE_G1, NOTE_A1, NOTE_B1, NOTE_C2, NOTE_D2, NOTE_E2, NOTE_F2, NOTE_G2, NOTE_A2, NOTE_B2, NOTE_C3, NOTE_D3, NOTE_E3, NOTE_F3, NOTE_G3, NOTE_A3, NOTE_B3, NOTE_C4, NOTE_D4, NOTE_E4, NOTE_F4, NOTE_G4, NOTE_A4, NOTE_B4, NOTE_C5, NOTE_D5, NOTE_E5, NOTE_F5, NOTE_G5, NOTE_A5, NOTE_B5, NOTE_C6, NOTE_D6, NOTE_E6, NOTE_F6, NOTE_G6, NOTE_A6, NOTE_B6, NOTE_C7, NOTE_D7, NOTE_E7, NOTE_F7, NOTE_G7, NOTE_A7, NOTE_B7};
const uint16_t NOTES_ALTERED[] PROGMEM = {NOTE_CS1, NOTE_DS1, NOTE_E1, NOTE_FS1, NOTE_GS1, NOTE_AS1, NOTE_B1, NOTE_CS2, NOTE_DS2, NOTE_E2, NOTE_FS2, NOTE_GS2, NOTE_AS2, NOTE_B2, NOTE_CS3, NOTE_DS3, NOTE_E3, NOTE_FS3, NOTE_GS3, NOTE_AS3, NOTE_B3, NOTE_CS4, NOTE_DS4, NOTE_E4, NOTE_FS4, NOTE_GS4, NOTE_AS4, NOTE_B4, NOTE_CS5, NOTE_DS5, NOTE_E5, NOTE_FS5, NOTE_GS5, NOTE_AS5, NOTE_B5, NOTE_CS6, NOTE_DS6, NOTE_E6, NOTE_FS6, NOTE_GS6, NOTE_AS6, NOTE_B6, NOTE_CS7, NOTE_DS7, NOTE_E7, NOTE_FS7, NOTE_GS7, NOTE_AS7, NOTE_B7};

struct Note {
  uint8_t pitch : 3;
  uint8_t duration : 2;
  uint8_t octave : 3;
  boolean extended : 1;
  boolean altered : 1;
};

struct MelodyInfo {
  uint8_t bpm;
  uint8_t beatUnit;
  uint16_t length;
};

void setup() {
  // Open serial for communication.
  Serial.begin(BAUD_RATE);

  if (PRINT_MENU) {
    printMenu();
  }

  // Inizializes LEDS (if available)
  for (uint8_t i = WHITE; i <= RED; i++) {
    pinMode(i, OUTPUT);
  }

  // Inizializes the button
  pinMode(BUTTON, INPUT_PULLUP);
}

void playMelody(Note* melody, MelodyInfo melodyInfo) {
  uint16_t beatMillis = 60000 / melodyInfo.bpm;
  for (uint8_t i = 0; i < melodyInfo.length; i++) {
    Note note = melody[i];
    uint16_t duration = beatMillis * pow(2, note.duration - melodyInfo.beatUnit) * (note.extended ? 1.5 : 1);

    // Pitch of 0 is a rest.
    if (note.pitch == 0) {
      delay(duration);
      clearLeds();
      continue;
    }

    uint16_t pitch = pgm_read_word((note.altered ? NOTES_ALTERED : NOTES) + ((note.pitch - 1)  + (7 * (note.octave - 1))));

    // Plays a note.
    tone(BUZZER, pitch, duration);

    randomizeLeds();

    // Plays the note for the specified duration.
    delay(duration);
  }
  delete[] melody;
  clearLeds();
}


void playMelody() {
  MelodyInfo melodyInfo = loadMelodyInfoFromEeprom();
  Note melody[melodyInfo.length];
  loadMelodyFromEeprom(melodyInfo, melody);
  playMelody(melody, melodyInfo);
}

void loop() {

  // Handles a menu selection.
  if (Serial.available()) {
    handleMenuInput();
  }

  if (digitalRead(BUTTON) == LOW) {
    playMelody();
  }
}
