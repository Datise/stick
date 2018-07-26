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
#define BTN_FASTER        0xFFE817
#define BTN_FASTER_2      0x5BE75E7F
#define BTN_SLOWER        0xFFC837
#define BTN_SLOWER_2      0x8E5D3EBB
#define BTN_OFF           0x80C
#define BTN_PREV          0xFF08F7
#define BTN_PREV_2        0x811
#define BTN_PREV_3        0x11
#define BTN_PREV_4        0x45473C1B
#define BTN_NEXT          0xFF28D7
#define BTN_NEXT_2        0x810
#define BTN_NEXT_3        0x10
#define BTN_NEXT_4        0x13549BDF
#define BTN_AUTO          0x35A9425F
#define BTN_AUTO_2        0xFFF00F
#define BTN_BLUE_UP       0xFF6897
#define BTN_BLUE_UP_2     0xA3C8EDDB
#define BTN_BLUE_UP_3     0xC101E57B
#define BTN_BLUE_DOWN     0xFF48B7
#define BTN_BLUE_DOWN_2   0x9EF4941F
#define BTN_BLUE_DOWN_3   0xF377C5B7

#define BTN_AUTOPLAY      0XFFF00F
#define BTN_SWITCH_MODE   0XFF827D
#define BTN_SWITCH_MODE_2 0X80B
#define BTN_SWITCH_MODE_3 0XB
#define BTN_SWITCH_MODE_4 0X3195A31F

#define BTN_DIR1 0XFF30CF

#define BTN_NONE         -1

int AUTO_CYCLE_TIME = 7000;

boolean povMode = true;
int patternNum = 0;
bool autoCycleOn = false;


bool changeMode();

void setup() {
  Serial.println("Welcome to dotdotflow");
  ir.init();
  ir.setup(digitalPinToInterrupt(5), []{changeMode();}, FALLING);
  led.init();
}

void nextPressed() {
  (povMode) ? led.nextImage() : led.nextPattern();
}

void prevPressed() {
  (povMode) ? led.prevImage() : led.prevPattern();
}

void showMode() {
   if (autoCycleOn) {
    led.autoCycle(AUTO_CYCLE_TIME, povMode);
  }

  if (povMode) {
    led.pov();
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
      case 9: //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Kellen test
        led.blueGreenPurp(led.speed);
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
      case BTN_FASTER_2:
        led.faster();
        break;
      case BTN_SLOWER:
      case BTN_SLOWER_2:
        led.slower();
        break;
      case BTN_DIR1:
        // dotSD.printDirectory(dotSD.root, 0);
        // dotSD.root.openNextFile()

        // Serial.println("Test file:");
        // dotSD.printFile("porn_letters.bmp");
        // dotSD.bmpDrawScale("porn_letters.bmp");

        // bmpFile.close();
        break;
      case BTN_AUTO:
      case BTN_AUTO_2:
        
        autoCycleOn = !autoCycleOn;

        if (autoCycleOn) {
          Serial.println("Auto Cycle On");
          led.quickFlash(0, 70, 0); // flash green
        } else {
          Serial.println("Auto Cycle Off");
          led.quickFlash(70, 0, 0); // flash red
        }
        // Serial.println("Toggle auto cycle");
        break;
      case BTN_BLUE_UP:
      case BTN_BLUE_UP_2:
      case BTN_BLUE_UP_3:
        if (AUTO_CYCLE_TIME >= 30000) {
          AUTO_CYCLE_TIME = 30000;
          Serial.print("Auto Cycle time: ");
          Serial.println(AUTO_CYCLE_TIME);
          break;
        }
        AUTO_CYCLE_TIME += 1000;
        Serial.print("Auto Cycle time: ");
        Serial.println(AUTO_CYCLE_TIME);
        break;
      case BTN_BLUE_DOWN:
      case BTN_BLUE_DOWN_2:
      case BTN_BLUE_DOWN_3:
        if (AUTO_CYCLE_TIME <= 0) {
          AUTO_CYCLE_TIME = 0;
          Serial.print("Auto Cycle time: ");
          Serial.println(AUTO_CYCLE_TIME);
          break;
        }
        AUTO_CYCLE_TIME -= 1000;
        Serial.print("Auto Cycle time: ");
        Serial.println(AUTO_CYCLE_TIME);
        break;
    }
    // Reset IR value
    ir.results.value = BTN_NONE;
  }
  return modeChanged;
}

void loop() {
  // wifi.listenWifi();
  
  
  // ir.IRinterrupt();
  showMode();
  


}

