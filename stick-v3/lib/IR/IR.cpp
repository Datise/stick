#include <IR.h>

IRrecv irrecv(RECV_PIN);

IR::IR() {};

void IR::init() {
  if (IR_DEBUG) Serial.println("Init IR");
  irrecv.enableIRIn();
  irrecv.blink13(true);
  if (IR_DEBUG) Serial.println("IR Ready");
}

void IR::setup(uint8_t irq_pin, void (*ISR_callback)(void), int value) {
  attachInterrupt(digitalPinToInterrupt(irq_pin), ISR_callback, value);
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
