#include "Arduino_LED_Matrix.h"
#include "wifidumbdisplay.h"
#include "secret.h"

// DumbDisplay using WIFI
// . assume the macros WIFI_SSID and WIFI_PASSWORD for WIFI SSID and WIFI password respectively 
DumbDisplay dumbdisplay(new DDWiFiServerIO(WIFI_SSID, WIFI_PASSWORD));

// the following layers will be created in setup()
LedGridDDLayer *ddMatrix;
LcdDDLayer *clearBtn;
LcdDDLayer *logBtn;


// UNO R4 matrix object
ArduinoLEDMatrix matrix;

// UNO R4 matrix frame data
uint32_t frame[] = {
  0x3184a444,
  0x42081100,
  0xa0040000
};

// toggle the frame bit @ position x, y
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
  // init for using UNO R4 matrix
  matrix.begin();
  // load whatever in frame data
  matrix.loadFrame(frame);

  // create the correspond DD "canvas" for the UNO R4 matrix
  ddMatrix = dumbdisplay.createLedGridLayer(12, 8);
  ddMatrix->border(0.3, DD_COLOR_darkblue);
  ddMatrix->padding(0.2);
  ddMatrix->onColor(DD_COLOR_red);  
  ddMatrix->offColor(DD_COLOR_lightgray);
  ddMatrix->backgroundColor(DD_COLOR_black);
  ddMatrix->enableFeedback("fa");

  // create the CLEAR "button"
  clearBtn = dumbdisplay.createLcdLayer(7, 1);
  clearBtn->border(1, DD_COLOR_gray, "raised");
  clearBtn->backgroundColor(DD_COLOR_lightgray);
  clearBtn->enableFeedback("fl");
  clearBtn->writeCenteredLine("CLEAR");

  // create the LOG "button"
  logBtn = dumbdisplay.createLcdLayer(5, 1);
  logBtn->border(1, DD_COLOR_gray, "raised");
  logBtn->backgroundColor(DD_COLOR_lightgray);
  logBtn->enableFeedback("fl");
  logBtn->writeCenteredLine("LOG");
 
  // auto pin the DD layers in the desired way
  dumbdisplay.configAutoPin(DD_AP_VERT_2(
    ddMatrix->getLayerId(),
    DD_AP_HORI_2(clearBtn->getLayerId(),logBtn->getLayerId())
  ));

  // copy the frame data to the DD "canvas"
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
}

void loop() {
  if (clearBtn->getFeedback()) {
    // CLEAR "button" clicked ==> clear the DD matrix as well as the UNO R4 matrix
    ddMatrix->clear();
    for (int i = 0; i < 3; i++) {
      frame[i] = 0;
    }
    matrix.loadFrame(frame);
  }
  if (logBtn->getFeedback()) {
    // LOG "button" clicked ==> log the frame data to DumbDisplay app as comment
    String frameCode = "{0x" + String(frame[0], HEX) + ",0x" + String(frame[1], HEX) + ",0x" + String(frame[2], HEX) + "}";
    dumbdisplay.writeComment(frameCode);
  }
  const DDFeedback* fb = ddMatrix->getFeedback();
  if (fb != NULL) {
    // DD "canvas" clicked, toggle the dot on the DD "canvas", as well as the UNO R4 matrix at the clicked position
    int x = fb->x;
    int y = fb->y;
    ddMatrix->toggle(x, y);
    toggle_bit(x, y);
    matrix.loadFrame(frame);
  }
}


