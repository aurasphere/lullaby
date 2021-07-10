// Each melody is serialized in the following format:
// 3 chars (2 hex + 1|2|3) = MelodyInfo (or header), containing bpm and beatUnit
// 3 chars per note (hex)
// An header is represented by 3 characters:
// 2 HEX <- bpm
// 1|2|3 <- the beat unit
//        1 = */2 (half note)
//        2 = */4 (quarter note)
//        3 = */8 (eight note)
// Each note is represented by 3 HEX char with the following content:
// 3 bit <- note (0 to 7, 0 is for a rest)
// 2 bit <- duration (0 to 3, see below)
// 3 bit <- octave (1 to 7)
// 1 bit <- extended (true or false)
// 1 bit <- altered (true or false)
// 2 bit <- filler
// Supported durations:
// 0 -> whole note
// 1 -> half note
// 2 -> quarter note
// 3 -> eight note

// Deserialization functions.
MelodyInfo deserializeMelody(char* msgBuffer, Note* melodyBuffer) {
  uint16_t melodyLength = 0;

  // Parses the header first
  char header[4] = {msgBuffer[1], msgBuffer[2], msgBuffer[3]};
  MelodyInfo melodyInfo = deserializeHeader(header);

  // Parses the single notes.
  char nextChar = msgBuffer[4];
  while (nextChar != NULL) {
    uint8_t notePointer = (melodyLength * 3) + 4;
    char noteChar[5] = {msgBuffer[notePointer], msgBuffer[notePointer + 1] , msgBuffer[notePointer + 2]};
    melodyBuffer[melodyLength] = deserializeNote(noteChar);
    melodyLength++;
    nextChar = msgBuffer[notePointer + 3];
  }

  melodyInfo.length = melodyLength;
  return melodyInfo;
}

Note deserializeNote(char* note) {
  char firstByteArray[3] = {note[0], note[1]};
  char secondByteArray[2] = {note[2]};
  uint8_t firstByte = (uint8_t) strtol(firstByteArray, NULL, 16);
  uint8_t secondByte = (uint8_t) strtol(secondByteArray, NULL, 16);
  return  (Note) {
    (firstByte & (0b111 << 5)) >> 5,
    (firstByte & (0b11 << 3)) >> 3,
    firstByte & 0b111,
    (secondByte & (1 << 3)) != 0,
    (secondByte & (1 << 2)) != 0
  };
}

MelodyInfo deserializeHeader(char* header) {
  char bpmHex[3] = {header[0], header[1]};
  uint8_t bpm = (uint8_t) strtol(bpmHex, NULL, 16);
  uint8_t beatNote = (uint8_t) strtol(&header[2], NULL, 10);
  return (MelodyInfo) {
    bpm, beatNote, 0
  };
}

// Serialization functions.
void serializeMelody(MelodyInfo melodyInfo, Note* melodyBuffer, char* serializedMelody) {
  char buffer[4];
  serializeHeader(melodyInfo, buffer);
  memcpy(serializedMelody, buffer, sizeof(char) * 3);

  // Serializes the notes.
  for (uint16_t i = 0; i < melodyInfo.length; i++) {
    serializeNote(melodyBuffer[i], buffer);
    memcpy(serializedMelody + 3 * (i + 1), buffer, sizeof(char) * 3);
  }
  // Adds a null terminator to end the string
  serializedMelody[3 * melodyInfo.length] = NULL;
}

void serializeNote(Note note, char* serializedNote) {
  uint8_t firstByteValue = (note.pitch << 5) + (note.duration << 3) + note.octave;
  uint8_t secondByteValue = (note.extended ? 1 << 3 : 0) + (note.altered ? 1 << 2 : 0);
  sprintf(serializedNote, "%x%x", firstByteValue, secondByteValue);
  serializedNote[3] = NULL;
}

void serializeHeader(MelodyInfo melodyInfo, char* serializedHeader) {
  sprintf(serializedHeader, "%x%d", melodyInfo.bpm, melodyInfo.beatUnit);
}

// Util functions (deprecated)
uint8_t applyMask(uint8_t byteVal, uint8_t index, uint8_t bitsToMask) {
  uint16_t mask = 1;
  for (uint8_t i = 0; i < bitsToMask - 1; i++, mask = mask * 10 + 1);
  return (byteVal & (mask << index)) >> index;
}

boolean isBitSet(uint8_t byteVal, uint8_t index) {
  return (byteVal & (1 << index)) != 0;
}
