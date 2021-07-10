void printMenu() {
  Serial.println(); 
  Serial.println(F("Smart carillon"));
  Serial.println(F("For info about the protocol, read here: https://github.com/aurasphere/lullaby"));
  Serial.println(F("Available commands: "));
  Serial.println(F("n<melody>: play the given notes"));
  Serial.println(F("s<melody>: save the given melody into EEPROM"));
  Serial.println(F("p: play the melody stored in the EEPROM"));
  Serial.println(F("e: export melody"));
  Serial.println(F("d: print the EEPROM content"));
  Serial.println();
}

void handleMenuInput() {
  char inputBuffer[MESSAGE_BUFFER_SIZE] = {};
  Serial.readBytesUntil(SERIALIZED_EOS_CHAR, inputBuffer, MESSAGE_BUFFER_SIZE);
  switch (inputBuffer[0]) {
    case 'n':
      playNotes(inputBuffer);
      break;
    case 's':
      saveMelodyFromSerial(inputBuffer);
      break;
    case 'p':
      playMelody();
      break;
    case 'e':
      exportMelody();
      break;
    case 'd':
      printEepromContent();
      break;
  }
  printMenu();
}

void exportMelody() {
  MelodyInfo melodyInfo = loadMelodyInfoFromEeprom();
  Note melody[melodyInfo.length];
  loadMelodyFromEeprom(melodyInfo, melody);
  char serializedMelody[melodyInfo.length * SERIALIZED_NOTE_LENGTH + SERIALIZED_HEADER_LENGTH + 1];
  serializeMelody(melodyInfo, melody, serializedMelody);
  Serial.println(serializedMelody);
}

void saveMelodyFromSerial(char* msgBuffer) {
  Note melodyBuffer[MAX_MELODY_LENGTH] = {};
  MelodyInfo melodyInfo = deserializeMelody(msgBuffer, melodyBuffer);
  saveDataToEeprom(melodyBuffer, melodyInfo);
}

void playNotes(char* msgBuffer) {
  Note melodyBuffer[MAX_MELODY_LENGTH] = {};
  MelodyInfo melodyInfo = deserializeMelody(msgBuffer, melodyBuffer);
  playMelody(melodyBuffer, melodyInfo);
}
