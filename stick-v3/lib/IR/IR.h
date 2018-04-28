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
    void setup(uint8_t irq_pin, void (*ISR_callback)(void), int value);

    decode_results results;
};

#endif