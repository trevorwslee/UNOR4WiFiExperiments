#include "Arduino_LED_Matrix.h"

ArduinoLEDMatrix matrix;



unsigned long frame[] = { 0, 0, 0 };  // 3 32-bit unsigned longs can holds 96 bits

void set_bit(size_t bit, bool on) {
  int index = bit / 32;
  int offset = 31 - (bit % 32);
  if (on) {
    frame[index] |= (1 << offset);
  } else {
    frame[index] &= ~(1 << offset);
  }
}

void invert_bits() {
  for (int i = 0; i < 3; i++) {
    frame[i] = ~frame[i];
  }
}

void setup() {
  Serial.begin(115200);
  matrix.begin();
}

int bit = -1;
void loop() {
  if (bit != -1) {
    set_bit(bit, false);
  }
  if (bit == -1) {
    bit = 0;
  } else {
    bit = (bit + 1) % 96;
  }
  set_bit(bit, true);
  matrix.loadFrame(frame);
  delay(500);
}