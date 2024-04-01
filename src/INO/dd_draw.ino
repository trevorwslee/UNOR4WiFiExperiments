#include "Arduino_LED_Matrix.h"
#include "wifidumbdisplay.h"

// DumbDisplay using WIFI
DumbDisplay dumbdisplay(new DDWiFiServerIO(WIFI_SSID, WIFI_PASSWORD));

// will be created in setup()
LedGridDDLayer *ddMatrix;


ArduinoLEDMatrix matrix;

unsigned long frame[] = {
  0x3184a444,
  0x42081100,
  0xa0040000
};

void toggle_bit(size_t x, size_t y) {
  size_t bit = x + y * 12;
  int index = bit / 32;
  int mask = 1 << (31 - (bit % 32));
  bool bit_is_on = frame[index] & mask;
  if (!bit_is_on) {
    frame[index] |= mask;
  } else {
    frame[index] &= ~mask;
  }
}


void setup() {
  Serial.begin(115200);
  matrix.begin();
  matrix.loadFrame(frame);

  ddMatrix = dumbdisplay.createLedGridLayer(12, 8);
  ddMatrix->border(0.3, DD_COLOR_azure);
  ddMatrix->padding(0.2);
  ddMatrix->onColor(DD_COLOR_red);  
  ddMatrix->offColor(DD_COLOR_lightgray);
  ddMatrix->backgroundColor(DD_COLOR_black);
  ddMatrix->enableFeedback("fa");

  size_t bit = 0;
  for (int y = 0; y < 8; y++) {
    int index = bit / 32;
    int offset = bit % 32;
    int offset2;
    if (offset + 12 < 32) {
      offset2 = -1;
    } else {
      offset2 = (bit + 12) % 32;
    }
    unsigned int bits = frame[index] << offset;
    if (offset2 != -1) {
      bits |= frame[index + 1] >> (32 - offset);
    }
    ddMatrix->bitwise(bits, y, true);
    bit += 12;
  }

  dumbdisplay.writeComment("***");
  dumbdisplay.writeComment("* long-press to clear");
  dumbdisplay.writeComment("***");
}

void loop() {
  const DDFeedback* fb = ddMatrix->getFeedback();
  if (fb != NULL) {
    if (fb->type == LONGPRESS) {
      ddMatrix->clear();
      for (int i = 0; i < 3; i++) {
        frame[i] = 0;
      }
      matrix.loadFrame(frame);
    } else {
      int x = fb->x;
      int y = fb->y;
      ddMatrix->toggle(x, y);
      toggle_bit(x, y);
      matrix.loadFrame(frame);
    }
    if (true) {
      String frameCode = "unsigned long frame[] = {0x" + String(frame[0], HEX) + ",0x" + String(frame[1], HEX) + ",0x" + String(frame[2], HEX) + "};";
      dumbdisplay.writeComment(frameCode);
    }
  }
}


