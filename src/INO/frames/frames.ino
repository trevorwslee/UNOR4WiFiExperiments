#include "Arduino_LED_Matrix.h"

ArduinoLEDMatrix matrix;


void setup() {
  Serial.begin(115200);
  matrix.begin();
}


const uint32_t frames[][3] = {
  {0x20020020,0x2002002,0x200200},
  {0x20020020,0x3e02002,0x200200},
  {0x22022022,0x3e02202,0x20220220},
  {0x22422422,0x3e02202,0x20220220},
  {0x22422422,0x3e42242,0x24224224},
};
const size_t num_frames = sizeof(frames) / sizeof(frames[0]);
  
int frame_index = 0;  
void loop(){
  matrix.loadFrame(frames[frame_index]);
  delay(500);
  frame_index = (frame_index + 1) % num_frames;
}