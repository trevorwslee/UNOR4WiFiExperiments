---
title: Arduino UNO R4 WiFi Experiments
published: false
tags: 'arduino uno r4 wifi, arduino, platformio'
cover_image: ./imgs/cat.jpg
id: 1810998
---


# Arduino UNO R4 WiFi Experiments


With this work, I hope to describe my Arduino UNO R4 WiFi experiments with the microcontroller board's LED matrix, starting from simply turning on/off each one of the LEDs, to having a simple remote UI for controlling the LEDs of the matrix, using an Android app (DumeDisplay) connected using the microcontroller board's WiFi support. 

The microcontroller programs here are Arduino sketches developed using VSCode with PlatformIO, in the similar fashion as described by the post -- [A Way to Run Arduino Sketch With VSCode PlatformIO Directly](https://www.instructables.com/A-Way-to-Run-Arduino-Sketch-With-VSCode-PlatformIO/).

Nevertheless, the sketches should be buildable with Arduino IDE.


# PlatformIO Project for the Sketches

A single PlatformIO project will be used for all the sketches here.

Assuming you have created a PlatformIO project `UNOR4WiFiExperiments` for the board `Arduino UNO R4 Wifi` with the `Arduino` framework.

You should have the file `platformio.ini` similar to

```
[env:UNOR4]
platform = renesas-ra
board = uno_r4_wifi
framework = arduino
monitor_speed = 115200
lib_deps =
    https://github.com/trevorwslee/Arduino-DumbDisplay
```
Notes:
* `UNOR4` is simply the name of the "PlatformIO Project Environment"
* The section `monitor_speed` specifies the baud-rate for the PlatformIO's Serial Monitor to be `115200`
* The section `lib_deps` specifies that the project will be depending on the [DumbDisplay Arduino Library](https://github.com/trevorwslee/Arduino-DumbDisplay)

The file `src/main.cpp` should simply be the two lines like
```
#include <Arduino.h>
#include "INO/matrix_test/matrix_test.ino"
```

Apparent, the first sketch will be `src/INO/matrix_test/matrix_test.ino`
```
#include "Arduino_LED_Matrix.h"
ArduinoLEDMatrix matrix;
void setup() {
  Serial.begin(115200);
  matrix.begin();
}
const uint32_t happy[] = {
    0x19819,
    0x80000001,
    0x81f8000
};
const uint32_t heart[] = {
    0x3184a444,
    0x44042081,
    0x100a0040
};
void loop(){
  matrix.loadFrame(happy);
  delay(500);
  matrix.loadFrame(heart);
  delay(500);
}
```
which is basically directly copied from the web page -- [Using the Arduino UNO R4 WiFi LED Matrix](https://docs.arduino.cc/tutorials/uno-r4-wifi/led-matrix/)

Notes:
* The basic initialization for using the LED matrix is like
  ```
  matrix.begin();
  ```
* To manipulate the LEDs of the matrix, a "frame" of 3 `uint32_t`s is needed, as in `happy` and `heart`
* You load and display a "frame" like
  ```
  matrix.loadFrame(happy);
  ```

Since each one of the 96 bits of the 3 `u_int32_t`s corresponds to a LED of the 12x8 LED matrix, apparently, the LED on/off can be controlled by turning the proper bit on/off, with routine like the following `set_bit()` which sets the corresponding bits of `frame` on/off
```
...
uint32_t frame[] = { 0, 0, 0 };  // 3 32-bit unsigned ints can holds 96 bits
void set_bit(size_t bit, bool on) {
  int index = bit / 32;
  int offset = 31 - (bit % 32);
  if (on) {
    frame[index] |= (1 << offset);
  } else {
    frame[index] &= ~(1 << offset);
  }
}
...
```  

Accordingly, lets try to turn on/off each of the LEDs one by one with sketch `src/INO/matrix_obo_test/matrix_obo_test.ino`
```
#include "Arduino_LED_Matrix.h"
ArduinoLEDMatrix matrix;
uint32_t frame[] = { 0, 0, 0 };  // 3 32-bit unsigned ints can holds 96 bits
void set_bit(size_t bit, bool on) {
  int index = bit / 32;
  int offset = 31 - (bit % 32);
  if (on) {
    frame[index] |= (1 << offset);
  } else {
    frame[index] &= ~(1 << offset);
  }
}
void setup() {
  Serial.begin(115200);
  matrix.begin();
}
int bit = -1;  // the bit last turned on
void loop() {
  if (bit != -1) {
    // turn off last bit
    set_bit(bit, false);
  }
  // advance bit
  if (bit == -1) {
    bit = 0;
  } else {
    bit = (bit + 1) % 96;
  }
  // turn on bit
  set_bit(bit, true);
  // load the frame (bits)
  matrix.loadFrame(frame);
  delay(500);
}
```
Notice that as it is looping through, the bit (turned on previously) is first turned off
```
  if (bit != -1) {
    // turn off last bit
    set_bit(bit, false);
  }
```
then the bit is advanced to next bit and turn on
```
  // advance bit
  if (bit == -1) {
    bit = 0;
  } else {
    bit = (bit + 1) % 96;
  }
  // turn on bit
  set_bit(bit, true);
```
finally, the "frame" is loaded
```
  // load the frame (bits)
  matrix.loadFrame(frame);
```


# zzz

![alt text](imgs/image.png)


