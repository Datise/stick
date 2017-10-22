/*------------------------------------------------------------------------
  POV IR Supernova Poi sketch.  Uses the following Adafruit parts
  (X2 for two poi):

  - Teensy 3.2 (required - NOT compatible w/AVR-based boards)
  - 2200 mAh Lithium Ion Battery https://www.adafruit.com/product/1781
  - LiPoly Backpack https://www.adafruit.com/product/2124
  - 144 LED/m DotStar strip (#2328 or #2329)
    (ONE METER is enough for TWO poi)
  - Infrared Sensor: https://www.adafruit.com/product/157
  - Mini Remote Control: https://www.adafruit.com/product/389
    (only one remote is required for multiple poi)

  Needs Adafruit_DotStar library: github.com/adafruit/Adafruit_DotStar
  Also, uses version of IRremote library from the Teensyduino installer,
  the stock IRremote lib will NOT work here!

  This is based on the LED poi code (also included in the repository),
  but AVR-specific code has been stripped out for brevity, since these
  mega-poi pretty much require a Teensy 3.X.

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Phil Burgess / Paint Your Dragon for Adafruit Industries.
  MIT license, all text above must be included in any redistribution.
  See 'COPYING' file for additional notes.
  ------------------------------------------------------------------------*/

#include <Arduino.h>
#include <Adafruit_DotStar.h>
//#include <avr/power.h>
//#include <avr/sleep.h>
#include <IRremote.h>
#include <SPI.h>

typedef uint16_t line_t;

// CONFIGURABLE STUFF ------------------------------------------------------

#include "graphics.h" // Graphics data is contained in this header file.
// It's generated using the 'convert.py' Python script.  Various image
// formats are supported, trading off color fidelity for PROGMEM space.
// Handles 1-, 4- and 8-bit-per-pixel palette-based images, plus 24-bit
// truecolor.  1- and 4-bit palettes can be altered in RAM while running
// to provide additional colors, but be mindful of peak & average current
// draw if you do that!  Power limiting is normally done in convert.py
// (keeps this code relatively small & fast).

// Ideally you use hardware SPI as it's much faster, though limited to
// specific pins.  If you really need to bitbang DotStar data & clock on
// different pins, optionally define those here:
#define LED_DATA_PIN  32
#define LED_CLOCK_PIN 33

// Empty and full thresholds (millivolts) used for battery level display:
#define BATT_MIN_MV 3350 // Some headroom over battery cutoff near 2.9V
#define BATT_MAX_MV 4000 // And little below fresh-charged battery near 4.1V

boolean autoCycle = true; // Set to true to cycle images by default
uint32_t CYCLE_TIME = 12; // Time, in seconds, between auto-cycle images

int RECV_PIN = 30;
IRrecv irrecv(RECV_PIN);
decode_results results;

// Adafruit IR Remote Codes:
//   Button       Code         Button  Code
//   -----------  ------       ------  -----
//   VOL-:        0xFD00FF     0/10+:  0xFD30CF
//   Play/Pause:  0xFD807F     1:      0xFD08F7
//   VOL+:        0xFD40BF     2:      0xFD8877
//   SETUP:       0xFD20DF     3:      0xFD48B7
//   STOP/MODE:   0xFD609F     4:      0xFD28D7
//   UP:          0xFDA05F     5:      0xFDA857
//   DOWN:        0xFDB04F     6:      0xFD6897
//   LEFT:        0xFD10EF     7:      0xFD18E7
//   RIGHT:       0xFD50AF     8:      0xFD9867
//   ENTER/SAVE:  0xFD906F     9:      0xFD58A7
//   Back:        0xFD708F

// 7 Key
//#define BTN_BRIGHT_UP    0x820
//#define BTN_BRIGHT_DOWN  0x821
//#define BTN_RESTART      0xFD807F
//#define BTN_BATTERY      0xFD20DF
//#define BTN_FASTER       0xFFE817
//#define BTN_SLOWER       0xFFC837
//#define BTN_OFF          0x80C
//#define BTN_PATTERN_PREV 0x811
//#define BTN_PATTERN_NEXT 0x810
//#define BTN_AUTOPLAY     0X80D
//#define BTN_SWITCH_MODE  0X80B
//#define BTN_NONE         -1

// 44 Key
#define BTN_BRIGHT_UP    0xFF3AC5
#define BTN_BRIGHT_DOWN  0xFFBA45
#define BTN_RESTART      0xFD807F
#define BTN_BATTERY      0xFD20DF
#define BTN_FASTER       0xFFE817
#define BTN_SLOWER       0xFFC837
#define BTN_OFF          0x80C
#define BTN_PATTERN_PREV 0xFF08F7
#define BTN_PATTERN_NEXT 0xFF28D7
#define BTN_AUTOPLAY     0XFFF00F
#define BTN_SWITCH_MODE  0XFF827D
#define BTN_NONE         -1

// -------------------------------------------------------------------------

#if defined(LED_DATA_PIN) && defined(LED_CLOCK_PIN)
// Older DotStar LEDs use GBR order.  If colors are wrong, edit here.
Adafruit_DotStar strip = Adafruit_DotStar(NUM_LEDS,
  LED_DATA_PIN, LED_CLOCK_PIN, DOTSTAR_BGR);
#else
Adafruit_DotStar strip = Adafruit_DotStar(NUM_LEDS, DOTSTAR_BGR); 
#endif

void     imageInit(void),
         IRinterrupt(void);
uint16_t readVoltage(void);

boolean povMode = false;

void setup() {
  strip.begin(); // Allocate DotStar buffer, init SPI
  strip.clear(); // Make sure strip is clear
  strip.show();  // before measuring battery
  
  imageInit();   // Initialize pointers for default image

  irrecv.enableIRIn(); // Start the receiver
}

void setPixel(int Pixel, byte red, byte green, byte blue) {
  strip.setPixelColor(Pixel, strip.Color(red, green, blue));
}

void showStrip() {
  strip.show();
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
    WheelPos = 255 - WheelPos;
    if(WheelPos < 85) {
  //return leds.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  return ((((uint32_t)(255 - WheelPos * 3)) << 16) + (WheelPos * 3));
    }
    if(WheelPos < 170) {
  WheelPos -= 85;
  //return leds.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  return ((((uint32_t)(WheelPos * 3)) << 8) + (255 - WheelPos * 3));
    }
    WheelPos -= 170;
    //return leds.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
    return ((((uint32_t)(WheelPos * 3)) << 16) + (((uint32_t)(255 - WheelPos * 3)) << 8));
}


void doubleConverge(bool trail, uint8_t wait, bool rev=false) {
    static uint8_t hue;
    int r = random(255);
    int g = random(255);
    int b = random(255);
    
    for(uint16_t i = 0; i < NUM_LEDS/2 + 4; i++) {
  // fade everything out
  //leds.fadeToBlackBy(60);

      
    
      if (i < NUM_LEDS/2) {
        if (!rev) {
          setPixel(i, r, g, b);
          setPixel(NUM_LEDS - 1 - i, r, g, b);
        } else {
          setPixel(NUM_LEDS/2 -1 -i, r, g, b);
          setPixel(NUM_LEDS/2 + i, r, g, b);
        }
      }
      if (!trail && i>3) {
       if (!rev) {
          setPixel(i - 4, 0, 0, 0);
          setPixel(NUM_LEDS - 1 - i + 4, 0, 0, 0);
        } else {
          setPixel(NUM_LEDS/2 -1 -i +4, 0, 0, 0);
          setPixel(NUM_LEDS/2 + i -4, 0, 0, 0);
        }
      }

      showStrip();
      delay(wait/3);
    }
}

// GLOBAL STATE STUFF ------------------------------------------------------

int NUM_PATTERNS = 3;

uint32_t lastImageTime = 0L, // Time of last image change
         lastLineTime  = 0L;
uint8_t  imageNumber   = 0,  // Current image being displayed
         imageType,          // Image type: PALETTE[1,4,8] or TRUECOLOR
         patternNumber = 1,  // Current pattern being displayed
        *imagePalette,       // -> palette data in PROGMEM
        *imagePixels,        // -> pixel data in PROGMEM
         palette[16][3];     // RAM-based color table for 1- or 4-bit images
line_t   imageLines,         // Number of lines in active image
         imageLine;          // Current line number in image
volatile uint16_t irCode = BTN_NONE; // Last valid IR code received

const uint8_t PROGMEM brightness[] = { 15, 31, 63, 127, 255 };
uint8_t bLevel = sizeof(brightness) - 1;

// Microseconds per line for various speed settings
const uint16_t PROGMEM lineTable[] = { // 375 * 2^(n/3)
  1000000L /  375, // 375 lines/sec = slowest
  1000000L /  472,
  1000000L /  595,
  1000000L /  750, // 750 lines/sec = mid
  1000000L /  945,
  1000000L / 1191,
  1000000L / 1500  // 1500 lines/sec = fastest
};
uint8_t  lineIntervalIndex = 3;
uint32_t lineInterval      = 1000000L / 750;

void imageInit() { // Initialize global image state for current imageNumber
  imageType    = images[imageNumber].type;
  imageLines   = images[imageNumber].lines;
  imageLine    = 0;
  imagePalette = (uint8_t *)images[imageNumber].palette;
  imagePixels  = (uint8_t *)images[imageNumber].pixels;
  // 1- and 4-bit images have their color palette loaded into RAM both for
  // faster access and to allow dynamic color changing.  Not done w/8-bit
  // because that would require inordinate RAM (328P could handle it, but
  // I'd rather keep the RAM free for other features in the future).
  if(imageType == PALETTE1)      memcpy_P(palette, imagePalette,  2 * 3);
  else if(imageType == PALETTE4) memcpy_P(palette, imagePalette, 16 * 3);
  lastImageTime = millis(); // Save time of image init for next auto-cycle
}

void nextImage(void) {
  if(++imageNumber >= NUM_IMAGES) imageNumber = 0;
  imageInit();
}

void prevImage(void) {
  imageNumber = imageNumber ? imageNumber - 1 : NUM_IMAGES - 1;
  imageInit();
}

void nextPattern() {
  if(++patternNumber >= NUM_PATTERNS) patternNumber = 0;
  Serial.println(patternNumber);
}

void prevPattern() {
  patternNumber = patternNumber ? patternNumber - 1 : NUM_PATTERNS - 1;
  Serial.println(patternNumber);
}

// MAIN LOOP ---------------------------------------------------------------

void loop() {
  uint32_t t = millis(); // Current time, milliseconds
//  Serial.println(imageType);
  
//  strip.setPixelColor(3, 255, 0, 0);

//  if(autoCycle) {
//    if((t - lastImageTime) >= (CYCLE_TIME * 1000L)) nextImage();
//    // CPU clocks vary slightly; multiple poi won't stay in perfect sync.
//    // Keep this in mind when using auto-cycle mode, you may want to cull
//    // the image selection to avoid unintentional regrettable combinations.
//  }
//
//  // Transfer one scanline from pixel data to LED strip:
  if (povMode) {
    switch(imageType) {
      case PALETTE1: { // 1-bit (2 color) palette-based image
        uint8_t  pixelNum = 0, byteNum, bitNum, pixels, idx,
                *ptr = (uint8_t *)&imagePixels[imageLine * NUM_LEDS / 8];
        for(byteNum = NUM_LEDS/8; byteNum--; ) { // Always padded to next byte
          pixels = *ptr++;                       // 8 pixels of data (pixel 0 = LSB)
          for(bitNum = 8; bitNum--; pixels >>= 1) {
            idx = pixels & 1; // Color table index for pixel (0 or 1)
            strip.setPixelColor(pixelNum++,
              palette[idx][0], palette[idx][1], palette[idx][2]);
          }
        }
        break;
      }
  
      case PALETTE4: { // 4-bit (16 color) palette-based image
        uint8_t  pixelNum, p1, p2,
                *ptr = (uint8_t *)&imagePixels[imageLine * NUM_LEDS / 2];
        for(pixelNum = 0; pixelNum < NUM_LEDS; ) {
          p2  = *ptr++;  // Data for two pixels...
          p1  = p2 >> 4; // Shift down 4 bits for first pixel
          p2 &= 0x0F;    // Mask out low 4 bits for second pixel
          strip.setPixelColor(pixelNum++,
            palette[p1][0], palette[p1][1], palette[p1][2]);
          strip.setPixelColor(pixelNum++,
            palette[p2][0], palette[p2][1], palette[p2][2]);
        }
        break;
      }
  
      case PALETTE8: { // 8-bit (256 color) PROGMEM-palette-based image
        uint16_t  o;
        uint8_t   pixelNum,
                 *ptr = (uint8_t *)&imagePixels[imageLine * NUM_LEDS];
        for(pixelNum = 0; pixelNum < NUM_LEDS; pixelNum++) {
          o = *ptr++ * 3; // Offset into imagePalette
          strip.setPixelColor(pixelNum,
            imagePalette[o],
            imagePalette[o + 1],
            imagePalette[o + 2]);
        }
        break;
      }
  
      case TRUECOLOR: { // 24-bit ('truecolor') image (no palette)
        uint8_t  pixelNum, r, g, b,
                *ptr = (uint8_t *)&imagePixels[imageLine * NUM_LEDS * 3];
        for(pixelNum = 0; pixelNum < NUM_LEDS; pixelNum++) {
          r = *ptr++;
          g = *ptr++;
          b = *ptr++;
          strip.setPixelColor(pixelNum, r, g, b);
        }
        break;
      }
    }
  
    if(++imageLine >= imageLines) imageLine = 0; // Next scanline, wrap around
  } else {
    switch(patternNumber) {
      case 0:
        Sparkle(random(255), random(255), random(255), 0);
        break;
      case 1:
        Fire(50, 80, 15);
        break;
      case 2:
        doubleConverge(false, 0);
        break;
    }
  }
//    Serial.println("test");
  IRinterrupt();
//  boolean test;
//  if (povMode) {
//    test = ((t = micros()) - lastLineTime) < lineInterval;
//  } else {
//    test = true;
//  }

  if (povMode) {
//    Serial.println("pov mode on");
    lineInterval = 1000000L / 750;
  } else {
//    Serial.println("pattern mode on");
    lineInterval = 18000;
    if (patternNumber == 2) {
      lineInterval = 110000;
    } else if (patternNumber == 0) {
      lineInterval = 1000000L / 350;
    }
  }

// Debug timing
//  Serial.println(t = micros() - lastLineTime);
//  Serial.println(lineInterval);
//  Serial.println();
  
  while(((t = micros()) - lastLineTime) < lineInterval) {
    if(results.value != BTN_NONE) {
      if(!strip.getBrightness()) { // If strip is off...
        // Set brightness to last level
        strip.setBrightness(brightness[bLevel]);
        // and ignore button press (don't fall through)
        // effectively, first press is 'wake'
      } else {
        switch(results.value) {
         case BTN_BRIGHT_UP:
          if(bLevel < (sizeof(brightness) - 1))
            strip.setBrightness(brightness[++bLevel]);
          break;
         case BTN_BRIGHT_DOWN:
          if(bLevel)
            strip.setBrightness(brightness[--bLevel]);
          break;
         case BTN_RESTART:
          imageNumber = 0;
          imageInit();
          break;
         case BTN_OFF:
          strip.setBrightness(0);
          break;
         case BTN_PATTERN_PREV:
          if (povMode) {
            prevImage();
          } else {
            prevPattern();
          }
          break;
         case BTN_PATTERN_NEXT:
          if (povMode) {
            nextImage();
          } else {
            nextPattern();
          }
          
          break;
         case BTN_SWITCH_MODE:
          if (povMode) {
            Serial.println("POV mode");
          } else {
            Serial.println("Pattern mode");
          }
          povMode = !povMode;
          break;
         case BTN_AUTOPLAY:
          autoCycle = !autoCycle;
          break;
        }
      }
      results.value = BTN_NONE;
    }
  }

  strip.show(); // Refresh LEDs
  lastLineTime = t + 100;
//  Serial.println("Meow");
}

void IRinterrupt() {
  if (irrecv.decode(&results)) {
    Serial.println(results.value, HEX);
    irrecv.resume(); // Receive the next value
  }
}

// Patterns

void Sparkle(byte red, byte green, byte blue, int SpeedDelay) {
//  int pixel = random(NUM_LEDS);
//  int jump = random(NUM_LEDS);
//  setPixel(pixel, red, green, blue);
//  setPixel(pixel+jump, random(255), random(255), random(255));
//  setPixel(pixel-jump, random(255), random(255), random(255));
//  showStrip();
//  delay(SpeedDelay);
//  setPixel(pixel, 0, 0, 0);
//  setPixel(pixel+jump, 0, 0, 0);
//  setPixel(pixel-jump, 0, 0, 0);

  int pixel = random(NUM_LEDS);
  setPixel(pixel, red, green, blue);
  showStrip();
  delay(SpeedDelay);
  setPixel(pixel, 0, 0, 0);
}

void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

/* Fire */
void setPixelHeatColor (int Pixel, byte temperature) {
  // Scale 'heat' down from 0-255 to 0-191
  byte t192 = round((temperature / 255.0) * 191);

  // calculate ramp up from
  byte heatramp = t192 & 0x3F; // 0..63
  heatramp <<= 2; // scale up to 0..252

  // figure out which third of the spectrum we're in:
  if ( t192 > 0x80) {                    // hottest
    setPixel(Pixel, 255, 255, heatramp);
  } else if ( t192 > 0x40 ) {            // middle
    setPixel(Pixel, 255, heatramp, 0);
  } else {                               // coolest
    setPixel(Pixel, 0, 0, heatramp);
  }
}

void Fire(int Cooling, unsigned int Sparking, int SpeedDelay) {
  static byte heat[NUM_LEDS];
  int cooldown;

  // Step 1.  Cool down ever1000000L / 750;y cell a little
  for ( int i = 0; i < NUM_LEDS; i++) {
    cooldown = random(0, ((Cooling * 10) / NUM_LEDS) + 2);

    if (cooldown > heat[i]) {
      heat[i] = 0;
    } else {
      heat[i] = heat[i] - cooldown;
    }
  }

  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for ( int k = NUM_LEDS - 1; k >= 2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
  }

  // Step 3.  Randomly ignite new 'sparks' near the bottom
  if ( random(255) < Sparking ) {
    int y = random(7);
    heat[y] = heat[y] + random(160, 255);
    //heat[y] = random(160,255);
  }

  // Step 4.  Convert heat to LED colors
  for ( int j = 0; j < NUM_LEDS; j++) {
    setPixelHeatColor(j, heat[j] );
  }

  showStrip();
  delay(SpeedDelay);
}

