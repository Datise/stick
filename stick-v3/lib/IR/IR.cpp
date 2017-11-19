#include <IR.h>
#include <IRremote.h>

IRrecv irrecv(RECV_PIN);
decode_results results;

IR::IR() {};

void IR::init() {
  if (IR_DEBUG) Serial.println("Init IR");
  irrecv.enableIRIn();
  irrecv.blink13(true);

  attachInterrupt(RECV_PIN, IRinterrupt, RISING);
  if (IR_DEBUG) Serial.println("IR Ready");
}

void IR::IRinterrupt() {
  if (irrecv.decode(&results)) {
    if(IR_DEBUG) {
      Serial.print("IR: ");
      if (results.decode_type == NEC) {
        Serial.print("(NEC) ");
      } else if (results.decode_type == SONY) {
        Serial.print("(SONY) ");
      } else if (results.decode_type == RC5) {
        Serial.print("(RC5) ");
      } else if (results.decode_type == RC6) {
        Serial.print("(RC6) ");
      } else if (results.decode_type == UNKNOWN) {
        Serial.print("(UNKNOWN) ");
      }
      Serial.println(results.value, HEX);
    }
    
    irrecv.resume();
  }
}