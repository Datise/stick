#ifndef LED_h
#define LED_h

#include "Arduino.h"

// 72 pixel
// #define NUM_LEDS 72
// #define LED_DATA_PIN  32
// #define LED_CLOCK_PIN 33

// 72 pixel South America
#define NUM_LEDS 72
#define LED_DATA_PIN  22
#define LED_CLOCK_PIN 20

// 22 pixel
// #define NUM_LEDS 22
// #define LED_DATA_PIN  7
// #define LED_CLOCK_PIN 10

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
    void cylon(bool trail, uint8_t wait);

    // State
    uint8_t patternNumber;
    uint8_t speed;

    // State change
    void nextImage();
    void prevImage();
    void nextPattern();
    void prevPattern();
    void faster();
    void slower();
    // void getCurrPattern();
    
  private:
    // Util
    void setPixel(int pixelNum, uint32_t c);
    void setPixel(int pixelNum, int r, int g, int b);
    void showStrip();
    uint32_t Wheel(byte WheelPos);

    // POV
    void imageInit();

    // Fire
    void setPixelHeatColor(int Pixel, byte temperature);
};

#endif