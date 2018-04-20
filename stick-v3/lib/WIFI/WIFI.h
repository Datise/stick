#ifndef WIFI_h
#define WIFI_h

#include "Arduino.h"

#define WIFI_DEBUG true

class WIFI {
  public:
    WIFI();
    void init();
    void listenWifi();
};

#endif