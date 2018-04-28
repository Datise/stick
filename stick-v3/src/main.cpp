#include <Arduino.h>
#include <LED.h>
#include <IR.h>
#include <DotSD.h>
#include <WIFI.h>

LED led;
IR ir;
DotSD dotSD;
// WIFI wifi;

// 44 Key
#define BTN_BRIGHT_UP     0xFF3AC5
#define BTN_BRIGHT_UP_2   0x820
#define BTN_BRIGHT_UP_3   0x20
#define BTN_BRIGHT_UP_4   0xB3D4B87F
#define BTN_BRIGHT_DOWN   0xFFBA45
#define BTN_BRIGHT_DOWN_2 0x821
#define BTN_BRIGHT_DOWN_3 0x21
#define BTN_BRIGHT_DOWN_4 0x44490A7B
#define BTN_RESTART       0xFD807F
#define BTN_BATTERY       0xFD20DF
#define BTN_FASTER        0xA3C8EDDB
#define BTN_SLOWER        0x9EF4941F
#define BTN_OFF           0x80C
#define BTN_PREV          0xFF08F7
#define BTN_PREV_2        0x811
#define BTN_PREV_3        0x11
#define BTN_PREV_4        0x45473C1B
#define BTN_NEXT          0xFF28D7
#define BTN_NEXT_2        0x810
#define BTN_NEXT_3        0x10
#define BTN_NEXT_4        0x13549BDF

#define BTN_AUTOPLAY      0XFFF00F
#define BTN_SWITCH_MODE   0XFF827D
#define BTN_SWITCH_MODE_2 0X80B
#define BTN_SWITCH_MODE_3 0XB
#define BTN_SWITCH_MODE_4 0X3195A31F

#define BTN_DIR1          0XFF30CF
#define BTN_DIR1_2        0x9716BE3F

#define BTN_NONE         -1

boolean povMode = false;
boolean isOpen = false;

int patternNum = 0;

DotSD::BmpImage bmpImage;

bool changeMode();

void setup() {
  Serial.println("Welcome to dotdotflow");
  ir.init();
  ir.setup(digitalPinToInterrupt(5), []{changeMode();}, FALLING);
  led.init();
  dotSD.init();
}

void nextPressed() {
  (povMode) ? led.nextImage() : led.nextPattern();
}

void prevPressed() {
  (povMode) ? led.prevImage() : led.prevPattern();
}

void showMode() {
  if (povMode) {
    // led.pov();
    led.PoiSonic(2000, bmpImage.image_array, bmpImage.w);
  } else {
    switch(led.patternNumber) {
      case 0:
        led.flash3(led.speed, changeMode);
        break;
      case 1:
        led.doubleCoverge(false, led.speed, true, changeMode);
        break;
      case 2:
        led.theaterChaseRainbow(led.speed, changeMode);
        break;
      case 3:
        led.sparkle(random(255), random(255), random(255), led.speed);
        break;
      case 4:
        led.cylon(true, led.speed, changeMode);
        break;
      case 5:
        led.theaterChase(random(255), random(255), random(255), led.speed, changeMode);
        break;
      case 6:
        led.doubleCoverge(false, led.speed, false, changeMode);
        break;
      case 7:
        led.flash2(led.speed, changeMode);
        break;
      case 8:
        led.fire(50, 80, led.speed);
        break;
    }
  }
}

bool changeMode() {
  bool modeChanged = false;
  ir.IRinterrupt();
  if(ir.results.value != BTN_NONE) {
    modeChanged = true;
    switch(ir.results.value) {
      case BTN_NEXT:
      case BTN_NEXT_4:
        nextPressed();
        break;
      case BTN_PREV:
      case BTN_PREV_4:
        prevPressed();
        break;
      case BTN_SWITCH_MODE:
      case BTN_SWITCH_MODE_4:
        povMode = !povMode;
        break;
      case BTN_BRIGHT_UP:
      case BTN_BRIGHT_UP_4:
        led.increaseBrightness();
        break;
      case BTN_BRIGHT_DOWN:
      case BTN_BRIGHT_DOWN_4:
        led.decreaseBrightness();
        break;
      case BTN_FASTER:
        led.faster();
        break;
      case BTN_SLOWER:
        led.slower();
        break;
      // case BTN_DIR1:
      case BTN_DIR1_2:
        // dotSD.printDirectory(dotSD.root, 0);
        // dotSD.root.openNextFile()

        // Serial.println("Test file:");
        // dotSD.printFile("porn_letters.bmp");
        if (!isOpen) {
          Serial.println("POV1");
          bmpImage = dotSD.readBmp("/needle.bmp");
          Serial.println("POV2");
          povMode = true;
          isOpen = true;
        }
        // led.PoiSonic(200, povbuffer);
        // bmpFile.close();
        break;
    }
    // Reset IR value
    ir.results.value = BTN_NONE;
  }
  return modeChanged;
}

void loop() {
  // wifi.listenWifi();
  showMode();
}

