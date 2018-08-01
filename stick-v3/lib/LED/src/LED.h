#ifndef LED_h
#define LED_h

#include "Arduino.h"

// 72 pixel
// #define NUM_LEDS 72
// #define LED_DATA_PIN  32
// #define LED_CLOCK_PIN 33

// // 72 pixel South America
#define NUM_LEDS 72
#define LED_DATA_PIN  22
#define LED_CLOCK_PIN 20

// 22 pixel
// #define NUM_LEDS 22
// #define LED_DATA_PIN  22
// #define LED_CLOCK_PIN 20

#define LED_DEBUG false

class LED {
  public:
    LED();
    void init();

    // POV
    void pov();
    void PoiSonic(unsigned long time, const unsigned int array[], unsigned int width);
    
    // Patterns
    void convergeIn();
    void fire(int Cooling, unsigned int Sparking, int SpeedDelay);
    void sparkle(byte red, byte green, byte blue, int SpeedDelay);
    void cylon(bool trail, uint8_t wait, bool (*IR_Interrupt)(void));
    void doubleCoverge(bool trail, uint8_t wait, bool rev, bool (*IR_Interrupt)(void));
    void theaterChase(byte red, byte green, byte blue, uint8_t wait, bool (*IR_Interrupt)(void));
    void theaterChaseRainbow(uint8_t wait, bool (*IR_Interrupt)(void));
    void flash2(uint8_t wait, bool (*IR_Interrupt)(void));
    void flash3(uint8_t wait, bool (*IR_Interrupt)(void));
    void quickFlash(int r, int g, int b);

    // State
    uint8_t patternNumber;
    uint8_t speed;

    // State change
    void nextImage(unsigned int* image_array, int w);
    void prevImage(unsigned int* image_array, int w);
    void nextPattern();
    void prevPattern();
    void increaseBrightness();
    void decreaseBrightness();
    void faster();
    void slower();
    void fadeall();
    void autoCycle(long interval, bool povMode);
    
    // POV
    void imageInit(unsigned int* image_array, int w);
  private:
    // Util
    void setPixel(int pixelNum, uint32_t c);
    void setPixel(int pixelNum, int r, int g, int b);
    void showStrip();
    uint32_t Wheel(byte WheelPos);
    void timeLoop(long int startMillis, long int interval);

    

    // Fire
    void setPixelHeatColor(int Pixel, byte temperature);
};

#endif