#include <WIFI.h>

#define BUFFER_SIZE 1024

#define SSID  "YOUR-SSID"      // change this to match your WiFi SSID
#define PASS  "your password"  // change this to match your WiFi password
#define PORT  "15164"           // using port 8080 by default

char buffer[BUFFER_SIZE];

WIFI::WIFI() {};

void WIFI::init() {
  // if (WIFI_DEBUG) Serial.println("Init WIFI");

  // assume esp8266 operates at 115200 baud rate
  // change if necessary to match your modules' baud rate
  Serial1.begin(115200);  // Teensy Hardware Serial port 1   (pins 0 and 1)
  Serial.begin(115200);   // Teensy USB Serial Port
  
  delay(5000);
  Serial.println("begin.");
}

void WIFI::listenWifi() {
  /* send everything received from the hardware uart to usb serial & vv */
  if (Serial.available() > 0) {
    char ch = Serial.read();
    Serial1.print(ch);
  }
  if (Serial1.available() > 0) {
    char ch = Serial1.read();
    Serial.print(ch);
  }
}