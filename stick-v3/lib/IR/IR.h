#ifndef IR_h
#define IR_h

#include "Arduino.h"
#include <IRremote.h>

#define RECV_PIN 5
#define IR_DEBUG true

class IR {
  

  public:
    IR();
    void init();
    void IRinterrupt();

    decode_results results;
};

#endif