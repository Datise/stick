#ifndef LED_h
#define LED_h

#include "Arduino.h"

// 72 pixel
// #define NUM_LEDS 72
// #define LED_DATA_PIN  32
// #define LED_CLOCK_PIN 33

// 22 pixel
#define NUM_LEDS 22
#define LED_DATA_PIN  7
#define LED_CLOCK_PIN 10 

#define NUM_PATTERNS 3
#define LED_DEBUG false

class LED {
  public:
    LED();
    void init();

    // POV
    void pov();
    
    // Patterns
    void convergeIn();
    void fire(int Cooling, unsigned int Sparking, int SpeedDelay);
    void sparkle(byte red, byte green, byte blue, int SpeedDelay);

    // State
    uint8_t patternNumber;
    void nextImage();
    void prevImage();
    void nextPattern();
    void prevPattern();
    
  private:
    void imageInit();
    void setPixelHeatColor(int Pixel, byte temperature);
    void setPixel(int pixelNum, int r, int g, int b);
    void showStrip();
};

#endif