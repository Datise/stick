#include <Arduino.h>
#include <LED.h>
#include <IR.h>

LED led;
IR ir;

void setup() {
  Serial.println("Welcome to dotdotflow");
  ir.init();
  led.init();
}

void loop() {
  // led.fire(50, 80, 15);
  // led.pov();
  led.sparkle(random(255), random(255), random(255), 0);
}