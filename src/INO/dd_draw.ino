#include "Arduino_LED_Matrix.h"
#include "wifidumbdisplay.h"

ArduinoLEDMatrix matrix;

DumbDisplay dumbdisplay(new DDWiFiServerIO(WIFI_SSID, WIFI_PASSWORD));




LedGridDDLayer *ddMatrix;




unsigned long frame[] = { 0, 0, 0 };  // 3 32-bit unsigned longs can holds 96 bits


void set_bit(size_t x, size_t y, bool on) {
  size_t bit = x + y * 12;
  int index = bit / 32;
  int offset = 31 - (bit % 32);
  if (on) {
    frame[index] |= (1 << offset);
  } else {
    frame[index] &= ~(1 << offset);
  }
}

int ddData[8] = { 0b100001011111, 
                  0b100001000100, 
                  0b100001000100, 
                  0b111111000100, 
                  0b100001000100, 
                  0b100001000100, 
                  0b100001000100, 
                  0b100001011111 };



void setup() {
  Serial.begin(115200);
  matrix.begin();

  ddMatrix = dumbdisplay.createLedGridLayer(12, 8);
  ddMatrix->border(0.3, DD_COLOR_azure);
  ddMatrix->padding(0.2);
  ddMatrix->onColor(DD_COLOR_red);  
  ddMatrix->offColor(DD_COLOR_lightgray);
  ddMatrix->backgroundColor(DD_COLOR_black);
  //ddMatrix->setFeedbackHandler(FeedbackHandler, "fa");
  for (int i = 0; i < 8; i++) {
    int bits = ddData[i];
    ddMatrix->bitwise(bits, i);
    for (int j = 0; j < 12; j++) {
      set_bit(j, i, (bits & (1 << (11 - j))) != 0);
    }
  }
  matrix.loadFrame(frame);
}

void loop() {

}