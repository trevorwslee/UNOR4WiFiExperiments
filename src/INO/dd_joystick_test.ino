#include "Arduino_LED_Matrix.h"
#include "wifidumbdisplay.h"

ArduinoLEDMatrix matrix;

DumbDisplay dumbdisplay(new DDWiFiServerIO(WIFI_SSID, WIFI_PASSWORD));
JoystickDDLayer *joystickLayer;


const size_t JOYSTICK_SIZE = 240;

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

  joystickLayer = dumbdisplay.createJoystickLayer(JOYSTICK_SIZE - 1);
  joystickLayer->border(3, "darkblue", "round", 1);

  set_bit(0, true);
  matrix.loadFrame(frame);
}

int prev_bit = 0;
void loop() {
  const DDFeedback* fb = joystickLayer->getFeedback();
  if (fb != NULL) {
    size_t x = int((fb->x * 12) / (double) JOYSTICK_SIZE);
    size_t y = int((fb->y * 8) / (double) JOYSTICK_SIZE);
    size_t bit = x + y * 12;
    if (bit != prev_bit) {
      dumbdisplay.writeComment("x: " + String(x) + " y: " + String(y) + " bit: " + String(bit));
      set_bit(prev_bit, false);
      set_bit(bit, true);
      matrix.loadFrame(frame);
      prev_bit = bit;
    }
  }
}