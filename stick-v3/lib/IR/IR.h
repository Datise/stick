#ifndef IR_h
#define IR_h

#include "Arduino.h"

#define RECV_PIN 30
#define IR_DEBUG true

class IR {
  static void IRinterrupt ();

  public:
    IR();
    void init();
};

#endif