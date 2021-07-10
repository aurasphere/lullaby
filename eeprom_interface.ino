// EEPROM structure of a melody:
// MelodyInfo (4 bytes)
// Notes (2 bytes per note)

// An Arduino Nano has 1024 bytes of EEPROM memory.
// This means that the maximum length of a melody is (1024-4) / 2 = 510 notes.

void saveDataToEeprom(Note* melody, MelodyInfo melodyInfo) {
  eeprom_write_block(&melodyInfo, 0, sizeof(melodyInfo));
  eeprom_write_block(melody, sizeof(melodyInfo), sizeof(Note) * melodyInfo.length);
}

MelodyInfo loadMelodyInfoFromEeprom() {
  MelodyInfo melodyInfo;
  eeprom_read_block(&melodyInfo, 0, sizeof(melodyInfo));
  return melodyInfo;
}

void loadMelodyFromEeprom(MelodyInfo melodyInfo, Note* melody) {
  eeprom_read_block(melody, sizeof(melodyInfo), sizeof(Note) * melodyInfo.length);
}

void printEepromContent() {
  MelodyInfo melodyInfo = loadMelodyInfoFromEeprom();
  Note melody[melodyInfo.length];
  loadMelodyFromEeprom(melodyInfo, melody);
  printMelodyInfo(melodyInfo);
  Serial.println(F("Melody: "));
  for (uint8_t i = 0; i < melodyInfo.length; i++) {
    printNote(melody[i]);
  }
}

void printNote(Note note) {
  Serial.print(F("pitch: "));
  Serial.print(note.pitch);
  Serial.print(F(" duration: "));
  Serial.print(note.duration);
  Serial.print(F(" octave: "));
  Serial.print(note.octave);
  Serial.print(F(" extended: "));
  Serial.print(note.extended);
  Serial.print(F(" altered: "));
  Serial.print(note.altered);
  Serial.println();
}

void printMelodyInfo(MelodyInfo melodyInfo) {
  Serial.print(F("Melody length: "));
  Serial.println(melodyInfo.length);
  Serial.print(F("BPM: "));
  Serial.println(melodyInfo.bpm);
  Serial.print(F("Tempo: ?/"));
  Serial.println(pow(2, melodyInfo.beatUnit));
}
