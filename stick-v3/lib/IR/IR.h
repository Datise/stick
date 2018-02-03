#ifndef IR_h
#define IR_h

#include "Arduino.h"
#include <IRremote.h>

#define RECV_PIN 30
#define IR_DEBUG false

class IR {
  

  public:
    IR();
    void init();
    void IRinterrupt();

    decode_results results;
};

#endif