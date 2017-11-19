#ifndef LED_h
#define LED_h

#include "Arduino.h"

#define NUM_LEDS 72
#define LED_DATA_PIN  32
#define LED_CLOCK_PIN 33

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
    
  private:
    void imageInit();
    void setPixelHeatColor(int Pixel, byte temperature);
    void setPixel(int pixelNum, int r, int g, int b);
    void showStrip();
};

#endif