#include <LED.h>
#include <pov.h>
#include <FastLED.h>

CRGBArray<NUM_LEDS> leds;

// GLOBAL STATE STUFF ------------------------------------------------------

uint32_t lastImageTime = 0L, // Time of last image change
         lastLineTime  = 0L;
uint8_t  imageNumber   = 0,  // Current image being displayed
         imageType,          // Image type: PALETTE[1,4,8] or TRUECOLOR
        *imagePalette,       // -> palette data in PROGMEM
        *imagePixels,        // -> pixel data in PROGMEM
         palette[16][3];     // RAM-based color table for 1- or 4-bit images
line_t   imageLines,         // Number of lines in active image
         imageLine;          // Current line number in image
  
const uint8_t PROGMEM brightness[] = { 4, 24, 48, 72,  98, 128, 192, 224, 255 };
uint8_t bLevel = 2; // Default brightness level

const int DEFAULT_SPEED = 16;
const int MAX_SPEED = 64;

const int DEFAULT_PATTERN_NUM = 0;
const String patterns[] = {"flash3", "doubleCoverge", "theaterChaseRainbow", "sparkle", "cylon", "theaterChase", "doubleConvergeNoTrail", "flash2", "fire"};
const int numPatterns = sizeof(patterns) / sizeof(String);

unsigned long time_now = 0;

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
uint32_t lineInterval      = 1000000L /  595;

LED::LED() {};

void LED::init() {
  if (LED_DEBUG) Serial.println("Init FastLED");
  FastLED.addLeds<APA102, LED_DATA_PIN, LED_CLOCK_PIN, BGR>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  patternNumber = DEFAULT_PATTERN_NUM;
  speed = DEFAULT_SPEED;

  FastLED.setBrightness(brightness[bLevel]);

  if (LED_DEBUG) Serial.println("FastLED Ready");

  imageInit(); // Initialize pointers for default image
}

// Utility
void LED::setPixel(int pixelNum, uint32_t c) {
   leds[pixelNum] = c;
}

void LED::setPixel(int pixelNum, int r, int g, int b) {
  leds[pixelNum].r = r;
  leds[pixelNum].g = g;
  leds[pixelNum].b = b;
}

 void LED::showStrip() {
  FastLED.show();
}

uint32_t LED::Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return ((((uint32_t)(255 - WheelPos * 3)) << 16) + (WheelPos * 3));
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return ((((uint32_t)(WheelPos * 3)) << 8) + (255 - WheelPos * 3));
  }
  WheelPos -= 170;
  return ((((uint32_t)(WheelPos * 3)) << 16) + (((uint32_t)(255 - WheelPos * 3)) << 8));
}

//--- Patterns ---
void LED::convergeIn() {
  static uint8_t hue;
  for(int i = 0; i < NUM_LEDS/2; i++) {
    // fade everything out
    leds.fadeToBlackBy(40);

    // let's set an led value
    leds[i] = CHSV(hue++,255,255);

    // now, let's first 20 leds to the top 20 leds, 
    leds(NUM_LEDS/2,NUM_LEDS-1) = leds(NUM_LEDS/2 - 1 ,0);
    FastLED.delay(35);
  }
}

/* Fire */
void LED::setPixelHeatColor(int Pixel, byte temperature) {
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

void LED::fire(int Cooling, unsigned int Sparking, int SpeedDelay) {
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

void LED::sparkle(byte red, byte green, byte blue, int SpeedDelay) {
  time_now = millis();
  int pixel = random(NUM_LEDS);
  setPixel(pixel, red, green, blue);
  showStrip();
  while(millis() < time_now + SpeedDelay);
  setPixel(pixel, 0, 0, 0);
}

void LED::cylon(bool trail, uint8_t wait, bool (*IR_Interrupt)(void)) {
  static uint8_t hue = 0;
  // First slide the led in one direction
  for(uint16_t i = 0; i < NUM_LEDS; i++) {
    // Set the i'th led to red
    setPixel(i, Wheel(hue++));
    if (IR_Interrupt()) return;
    showStrip();
    // now that we've shown the leds, reset the i'th led to black
    if (!trail) setPixel(i, 0);
    // Fade all
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i].nscale8(250);
    }
    // Wait a little bit before we loop around and do it again
    delay(wait/4);
  }

  for(uint16_t i = (NUM_LEDS)-1; i > 0; i--) {
    setPixel(i, Wheel(hue++));
    if (IR_Interrupt()) return;
    showStrip();
    if (!trail) setPixel(i, 0);
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i].nscale8(250);
    }
    delay(wait/4);
  }
}

void LED::doubleCoverge(bool trail, uint8_t wait, bool rev, bool (*IR_Interrupt)(void)) {
  static uint8_t hue;
  for (uint16_t i = 0; i < NUM_LEDS / 2 + 4; i++)
  {
    if (i < NUM_LEDS / 2) {
      if (!rev) {
        setPixel(i, Wheel(hue++));
        setPixel(NUM_LEDS - 1 - i, Wheel(hue++));
      }
      else {
        setPixel(NUM_LEDS / 2 - 1 - i, Wheel(hue++));
        setPixel(NUM_LEDS / 2 + i, Wheel(hue++));
      }
    }
    if (!trail && i > 3) {
      if (!rev) {
        setPixel(i - 4, 0);
        setPixel(NUM_LEDS - 1 - i + 4, 0);
      }
      else {
        setPixel(NUM_LEDS / 2 - 1 - i + 4, 0);
        setPixel(NUM_LEDS / 2 + i - 4, 0);
      }
    }
    if (IR_Interrupt()) return;
    showStrip();
    delay(wait / 3);
  }
}

void LED::theaterChase(byte red, byte green, byte blue, uint8_t wait, bool (*IR_Interrupt)(void)) {
  for (int j = 0; j < 10; j++) { //do 10 cycles of chasing
    for (int q = 0; q < 3; q++) {
      for (uint16_t i = 0; i < NUM_LEDS; i = i + 3) {
        setPixel(i + q, red, green, blue); //turn every third pixel on
      }
      if (IR_Interrupt()) return;
      showStrip();
      delay(wait);
      for (uint16_t i = 0; i < NUM_LEDS; i = i + 3) {
        setPixel(i + q, 0, 0, 0); //turn every third pixel off
      }
    }
  }
}

void LED::theaterChaseRainbow(uint8_t wait, bool (*IR_Interrupt)(void)) {
  for (int j=0; j < 256; j+=7) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < NUM_LEDS; i=i+3) {
        setPixel(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      if (IR_Interrupt()) return;
      showStrip();
      delay(wait);
      for (uint16_t i=0; i < NUM_LEDS; i=i+3) {
        setPixel(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

void LED::flash2(uint8_t wait, bool (*IR_Interrupt)(void)) {
  uint16_t i, j;

  for(j=0; j<52; j++) {
    for(i=0; i< NUM_LEDS-1; i+=2) {
      setPixel(i, Wheel(j * 5));
      setPixel(i+1, 0);
    }
    if (IR_Interrupt()) return;
    showStrip();
    delay(wait*2);
    
    for(i=1; i< NUM_LEDS-1; i+=2) {
      setPixel(i, Wheel(j * 5 + 48));
      setPixel(i+1, 0);
    }
    if (IR_Interrupt()) return;
    showStrip();
    delay(wait*2);
  }
}

void LED::flash3(uint8_t wait, bool (*IR_Interrupt)(void)) {
  uint16_t i, j;
  for(j=0; j<52; j++) {
    for(i=0; i< NUM_LEDS; i+=3) {
        setPixel(i, Wheel(j * 5));
        setPixel(i+1, Wheel(j * 5+128));
        setPixel(i+2, 0);
    }
    if (IR_Interrupt()) return;
    showStrip();
    // if (handle_IR(wait)) return;
    delay(wait);
    

    for(i=1; i< NUM_LEDS-2; i+=3) {
        setPixel(i, Wheel(j * 5 + 48));
        setPixel(i+1, Wheel(j * 5+128));
        setPixel(i+2, 0);
    }
    if (IR_Interrupt()) return;
    
    showStrip();
    delay(wait);
    

    for(i=2; i< NUM_LEDS-2; i+=3) {
        setPixel(i, Wheel(j * 5 + 96));
        setPixel(i+1, Wheel(j * 5+128));
        setPixel(i+2, 0);
    }
    if (IR_Interrupt()) return;
    showStrip();
    delay(wait);
  }
}

//--- POV ---
void LED::imageInit() { // Initialize global image state for current imageNumber
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

void LED::pov() {
  uint32_t t = millis();

  switch(imageType) {
    case PALETTE1: { // 1-bit (2 color) palette-based image
      uint8_t  pixelNum = 0, byteNum, bitNum, pixels, idx,
              *ptr = (uint8_t *)&imagePixels[imageLine * NUM_LEDS / 8];
      for(byteNum = NUM_LEDS/8; byteNum--; ) { // Always padded to next byte
        pixels = *ptr++;                       // 8 pixels of data (pixel 0 = LSB)
        for(bitNum = 8; bitNum--; pixels >>= 1) {
          idx = pixels & 1; // Color table index for pixel (0 or 1)
          setPixel(pixelNum++,
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
        setPixel(pixelNum++,
          palette[p1][0], palette[p1][1], palette[p1][2]);
        setPixel(pixelNum++,
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
        setPixel(pixelNum,
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
        setPixel(pixelNum, r, g, b);
      }
      break;
    }
  }

  if(++imageLine >= imageLines) imageLine = 0; // Next scanline, wrap around

  while(((t = micros()) - lastLineTime) < lineInterval) {}

  FastLED.show();
  lastLineTime = t + 100;
}

void LED::nextImage(void) {
  if(++imageNumber >= NUM_IMAGES) imageNumber = 0;
  imageInit();
}

void LED::prevImage(void) {
  imageNumber = imageNumber ? imageNumber - 1 : NUM_IMAGES - 1;
  imageInit();
}

void LED::nextPattern() {
  if(++patternNumber >= numPatterns) patternNumber = 0;
  Serial.print("pattern:");
  Serial.println(patterns[patternNumber]);
}

void LED::prevPattern() {
  patternNumber = patternNumber ? patternNumber - 1 : numPatterns - 1;
  Serial.print("pattern:");
 
  Serial.println(patterns[patternNumber]);
}

void LED::increaseBrightness() {
  if(bLevel < (sizeof(brightness) - 1)) FastLED.setBrightness(brightness[++bLevel]);
}

void LED::decreaseBrightness() {
  if(bLevel) FastLED.setBrightness(brightness[--bLevel]);
}

void LED::faster() {
  speed = (speed - 1 < 0) ? 0 : speed - 1;
  Serial.print("speed:");
  Serial.println(speed);
}

void LED::slower() {
  if (++speed > MAX_SPEED) speed = MAX_SPEED;
  Serial.print("speed:");
  Serial.println(speed);
}

unsigned long previousMillis = 0;
void LED::autoCycle(long interval, bool povMode) {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    povMode ? nextImage() : nextPattern();
  }
}

void LED::quickFlash(int r, int g, int b) {
  for(uint16_t i = 0; i < NUM_LEDS; i++) {
    setPixel(i, r, g, b);
  }
  showStrip();
}
