void randomizeLeds() {
  for (uint8_t i = WHITE; i <= RED; i++) {
    digitalWrite(i, random(2));
  }
}

void clearLeds() {
  for (uint8_t i = WHITE; i <= RED; i++) {
    digitalWrite(i, LOW);
  }
}
