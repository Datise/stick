#include <FastLED.h>
#include <IRremote.h>
#include "IRcodes.h"

#define FASTLED
#define RECV_PIN 11
#define NEOPIXEL_PIN 22
#define NUM_LEDS 30

IRrecv irrecv(RECV_PIN);

CRGB leds[NUM_LEDS];

void setup() {
    Serial.begin(115200);
    Serial.println("Enabling IRin");
    irrecv.enableIRIn(); // Start the receiver
    Serial.println("Enabled IRin, turn on LEDs");


    Serial.print("Using FastLED to drive these LEDs: ");
    FastLED.addLeds<NEOPIXEL,NEOPIXEL_PIN>(leds, NUM_LEDS);
    FastLED.setBrightness(15);

    Serial.println(NUM_LEDS);
    leds_show(); // Initialize all pixels to 'off'
    Serial.println("LEDs on");
    colorWipeSingle;
    
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


typedef enum {
    f_nothing = 0,
    f_colorWipe = 1,
    f_rainbow = 2,
    f_rainbowCycle = 3,
    f_theaterChase = 4,
    f_theaterChaseRainbow = 5,
    f_cylon = 6,
    f_cylonTrail = 7,
    f_doubleConverge = 8,
    f_doubleConvergeRev = 9,
    f_doubleConvergeTrail = 10,
    f_flash = 11,
    f_flash3 = 12,
    f_demo = 13, // <<<<<< f_(mode/function name) = to some number
} StripDemo;

StripDemo nextdemo = f_theaterChaseRainbow;
// Is the current demo linked to a color (false for rainbow demos)
bool colorDemo = true;
int32_t demo_color = 0x00FF00; // Green
int16_t speed = 50;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


void change_brightness(int8_t change) {
  static uint8_t brightness = 4;

    static uint32_t last_brightness_change = 0 ;
    uint8_t bright_value;

    if (millis() - last_brightness_change < 300) {
  Serial.print("Too soon... Ignoring brightness change from ");
  Serial.println(brightness);
  return;
    }
    last_brightness_change = millis();
    brightness = constrain(brightness + change, 1, 8);
    bright_value = (1 << brightness) - 1;

    FastLED.setBrightness(bright_value);

    bright_value = 255;
    if (brightness == 7) bright_value = 224;
    else if (brightness == 6) bright_value = 176;
    else if (brightness == 5) bright_value = 128;
    else if (brightness == 4) bright_value = 96;
    else if (brightness == 3) bright_value = 64;
    else if (brightness == 2) bright_value = 32;
    else if (brightness == 1) bright_value = 16;
    else if (brightness == 0) bright_value = 0;
    


    Serial.print("Changing brightness ");
    Serial.print(change);
    Serial.print(" to level ");
    Serial.print(brightness);
    Serial.print(" value ");
    Serial.println(bright_value);
    leds_show();
}

void change_speed(int8_t change) {
    static uint32_t last_speed_change = 0 ;

    if (millis() - last_speed_change < 200) {
  Serial.print("Too soon... Ignoring speed change from ");
  Serial.println(speed);
  return;
    }
    last_speed_change = millis();

    Serial.print("Changing speed ");
    Serial.print(speed);
    speed = constrain(speed + change, 1, 100);
    Serial.print(" to new speed ");
    Serial.println(speed);
}

bool handle_IR(uint32_t delay_time) {
    decode_results IR_result;
    delay(delay_time);
    if (irrecv.decode(&IR_result)) {
      irrecv.resume(); // Receive the next value
  switch (IR_result.value) {
    
  case IR_RGBZONE_POWER:
      nextdemo = f_colorWipe;
      demo_color = 0x000000;
      // Changing the speed value will
      speed = 1;
      Serial.println("Got IR: Power");
      return 1;

  case IR_RGBZONE_BRIGHT:
      change_brightness(+1);
      Serial.println("Got IR: Bright");
      return 1;

  case IR_RGBZONE_DIM:
      change_brightness(-1);
      Serial.println("Got IR: Dim");
      return 1;

  case IR_RGBZONE_QUICK:
      change_speed(-10);
      Serial.println("Got IR: Quick");
      return 1;

  case IR_RGBZONE_SLOW:
      change_speed(+10);
      Serial.println("Got IR: Slow");
      return 1;


  case IR_RGBZONE_RED:
      if (!colorDemo) nextdemo = f_colorWipe;
      demo_color = 0xFF0000;
      Serial.println("Got IR: Red");
      return 1;

  case IR_RGBZONE_GREEN:
      if (!colorDemo) nextdemo = f_colorWipe;
      demo_color = 0x00FF00;
      Serial.println("Got IR: Green");
      return 1;

  case IR_RGBZONE_BLUE:
      if (!colorDemo) nextdemo = f_colorWipe;
      demo_color = 0x0000FF;
      Serial.println("Got IR: Blue");
      return 1;

  case IR_RGBZONE_WHITE:
      if (!colorDemo) nextdemo = f_colorWipe;
      demo_color = 0xFFFFFF;
      Serial.println("Got IR: White");
      return 1;



  case IR_RGBZONE_RED2:
      if (!colorDemo) nextdemo = f_colorWipe;
      demo_color = 0xCE6800;
      Serial.println("Got IR: Red2");
      return 1;

  case IR_RGBZONE_GREEN2:
      if (!colorDemo) nextdemo = f_colorWipe;
      demo_color = 0x00CC00;
      Serial.println("Got IR: Green2");
      return 1;

  case IR_RGBZONE_BLUE2:
      if (!colorDemo) nextdemo = f_colorWipe;
      demo_color = 0x0000BB;
      Serial.println("Got IR: Blue2");
      return 1;

  case IR_RGBZONE_PINK:
      if (!colorDemo) nextdemo = f_colorWipe;
      demo_color = 0xFF50FE;
      Serial.println("Got IR: Pink");
      return 1;



  case IR_RGBZONE_ORANGE:
      if (!colorDemo) nextdemo = f_colorWipe;
      demo_color = 0xFF8100;
      Serial.println("Got IR: Orange");
      return 1;

  case IR_RGBZONE_BLUE3:
      if (!colorDemo) nextdemo = f_colorWipe;
      demo_color = 0x00BB00;
      Serial.println("Got IR: Green2");
      return 1;

  case IR_RGBZONE_PURPLED:
      if (!colorDemo) nextdemo = f_colorWipe;
      demo_color = 0x270041;
      Serial.println("Got IR: DarkPurple");
      return 1;

  case IR_RGBZONE_PINK2:
      if (!colorDemo) nextdemo = f_colorWipe;
      demo_color = 0xFFB9FF;
      Serial.println("Got IR: Pink2");
      return 1;



  case IR_RGBZONE_ORANGE2:
      if (!colorDemo) nextdemo = f_colorWipe;
      demo_color = 0xFFCA49;
      Serial.println("Got IR: Orange2");
      return 1;

  case IR_RGBZONE_GREEN3:
      if (!colorDemo) nextdemo = f_colorWipe;
      demo_color = 0x006600;
      Serial.println("Got IR: Green2");
      return 1;

  case IR_RGBZONE_PURPLE:
      if (!colorDemo) nextdemo = f_colorWipe;
      demo_color = 0x2B0064;
      Serial.println("Got IR: DarkPurple2");
      return 1;

  case IR_RGBZONE_BLUEL:
      if (!colorDemo) nextdemo = f_colorWipe;
      demo_color = 0x50A7FF;
      Serial.println("Got IR: BlueLight");
      return 1;



  case IR_RGBZONE_YELLOW:
      if (!colorDemo) nextdemo = f_colorWipe;
      demo_color = 0xF0FF00;
      Serial.println("Got IR: Yellow");
      return 1;

  case IR_RGBZONE_GREEN4:
      if (!colorDemo) nextdemo = f_colorWipe;
      demo_color = 0x003300;
      Serial.println("Got IR: Green2");
      return 1;

  case IR_RGBZONE_PURPLE2:
      if (!colorDemo) nextdemo = f_colorWipe;
      demo_color = 0x660265;
      Serial.println("Got IR: Purple2");
      return 1;

  case IR_RGBZONE_BLUEL2:
      if (!colorDemo) nextdemo = f_colorWipe;
      demo_color = 0x408BD8;
      Serial.println("Got IR: BlueLight2");
      return 1;

//Kellen's Demo Experiment<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
case IR_RGBZONE_NEXT: //IR
      // this uses the last color set
      nextdemo = f_demo;   // nextdemo = f_(mode function name)
      Serial.println("Got IR: DIY1/colorWipe");
      return 1;


  case IR_RGBZONE_DIY1:
      // this uses the last color set
      nextdemo = f_colorWipe;
      Serial.println("Got IR: DIY1/colorWipe");
      return 1;

  case IR_RGBZONE_DIY2:
      // this uses the last color set
      nextdemo = f_theaterChase;
      Serial.println("Got IR: DIY2/theaterChase");
      return 1;

  case IR_RGBZONE_DIY3:
      nextdemo = f_flash3;
      Serial.println("Got IR: DIY3/Flash3");
      return 1;

  case IR_RGBZONE_DIY4:
      nextdemo = f_rainbowCycle;
      Serial.println("Got IR: DIY4/rainbowCycle");
      return 1;

  case IR_RGBZONE_DIY5:
      nextdemo = f_theaterChaseRainbow;
      Serial.println("Got IR: DIY5/theaterChaseRainbow");
      return 1;

  case IR_RGBZONE_DIY6:
      nextdemo = f_doubleConvergeRev;
      Serial.println("Got IR: DIY6/DoubleConvergeRev");
      return 1;

  case IR_RGBZONE_JUMP3:
      nextdemo = f_cylon;
      Serial.println("Got IR: JUMP3/Cylon");
      return 1;

  case IR_RGBZONE_JUMP7:
      nextdemo = f_cylonTrail;
      Serial.println("Got IR: JUMP7/CylonWithTrail");
      return 1;

  case IR_RGBZONE_FADE3:
      nextdemo = f_doubleConverge;
      Serial.println("Got IR: FADE3/DoubleConverge");
      return 1;

  case IR_RGBZONE_FADE7:
      nextdemo = f_doubleConvergeTrail;
      Serial.println("Got IR: FADE7/DoubleConvergeTrail");
      return 1;

  case IR_RGBZONE_FLASH:
      nextdemo = f_flash;
      Serial.println("Got IR: FLASH");
      return 1;

  case IR_JUNK:
      return 0;

  default:
      Serial.print("Got unknown IR value: ");
      Serial.println(IR_result.value, HEX);
      return 0;
  }
    }
    return 0;
}

void leds_show() {
    FastLED.show();
}

void leds_setcolor(uint16_t i, uint32_t c) {
    leds[i] = c;
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
    WheelPos = 255 - WheelPos;
    if(WheelPos < 85) {
  //return leds.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  return ((((uint32_t)(255 - WheelPos * 3)) << 16) + (WheelPos * 3));
    }
    if(WheelPos < 170) {
  WheelPos -= 85;
  //return leds.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  return ((((uint32_t)(WheelPos * 3)) << 8) + (255 - WheelPos * 3));
    }
    WheelPos -= 170;
    //return leds.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
    return ((((uint32_t)(WheelPos * 3)) << 16) + (((uint32_t)(255 - WheelPos * 3)) << 8));
}

// Demos from FastLED

void fadeall() {
    for(int i = 0; i < NUM_LEDS; i++) {  leds[i].nscale8(250); }
}

void cylon(bool trail, uint8_t wait) {
    static uint8_t hue = 0;
    // First slide the led in one direction
    for(uint16_t i = 0; i < NUM_LEDS; i++) {
  // Set the i'th led to red
  leds_setcolor(i, Wheel(hue++));
  // Show the leds
  leds_show();
  // now that we've shown the leds, reset the i'th led to black
  if (!trail) leds_setcolor(i, 0);
  fadeall();
  // Wait a little bit before we loop around and do it again
  delay(wait/4);
    }

    // Now go in the other direction.
    for(uint16_t i = (NUM_LEDS)-1; i > 0; i--) {
  // Set the i'th led to red
  leds_setcolor(i, Wheel(hue++));
  // Show the leds
  leds_show();
  // now that we've shown the leds, reset the i'th led to black
  if (!trail) leds_setcolor(i, 0);
  fadeall();
  // Wait a little bit before we loop around and do it again
  delay(wait/4);
    }
}

void doubleConverge(bool trail, uint8_t wait, bool rev=false) {
    static uint8_t hue;
    for(uint16_t i = 0; i < NUM_LEDS/2 + 4; i++) {
  // fade everything out
  //leds.fadeToBlackBy(60);

  if (i < NUM_LEDS/2) {
      if (!rev) {
    leds_setcolor(i, Wheel(hue++));
    leds_setcolor(NUM_LEDS - 1 - i, Wheel(hue++));
      } else {
    leds_setcolor(NUM_LEDS/2 -1 -i, Wheel(hue++));
    leds_setcolor(NUM_LEDS/2 + i, Wheel(hue++));
      }
  }
  if (!trail && i>3) {
      if (!rev) {
    leds_setcolor(i - 4, 0);
    leds_setcolor(NUM_LEDS - 1 - i + 4, 0);
      } else {
    leds_setcolor(NUM_LEDS/2 -1 -i +4, 0);
    leds_setcolor(NUM_LEDS/2 + i -4, 0);
      }
  }

  leds_show();
  delay(wait/3);
    }
}

// The animations below are from Adafruit_NeoPixel/examples/strandtest

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
    for(uint16_t i=0; i<NUM_LEDS; i++) {
  leds_setcolor(i, c);
  leds_show();
  delay(10);
  if (handle_IR(wait/5)) return;
    }
}
 //Kellen's Experiment <<<<<<<<<<<<<<<<<<<<<<<<<<<< Mode/Function
void colorWipeOff (uint32_t c, uint8_t wait) {
    for(uint16_t i=0; i<NUM_LEDS; i++) {
  leds_setcolor(i, CRGB::Blue);
  leds_show();
  delay(10);
  if (handle_IR(wait/5)) return;
    }
}

void rainbow(uint8_t wait) {
    uint16_t i, j;

    for(j=0; j<256; j++) {
  for(i=0; i<NUM_LEDS; i++) {
      leds_setcolor(i, Wheel((i+j) & 255));
  }
  leds_show();
  if (handle_IR(wait)) return;
    }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
    uint16_t i, j;

    for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
  for(i=0; i< NUM_LEDS; i++) {
      leds_setcolor(i, Wheel(((i * 256 / NUM_LEDS) + j) & 255));
  }
  leds_show();
  if (handle_IR(wait/20)) return;
    }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
    for (int j=0; j<10; j++) {  //do 10 cycles of chasing
  for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < NUM_LEDS; i=i+3) {
    leds_setcolor(i+q, c);    //turn every third pixel on
      }
      leds_show();

      if (handle_IR(wait)) return;

      for (uint16_t i=0; i < NUM_LEDS; i=i+3) {
    leds_setcolor(i+q, 0);        //turn every third pixel off
      }
  }
    }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
    for (int j=0; j < 256; j+=7) {     // cycle all 256 colors in the wheel
  for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < NUM_LEDS; i=i+3) {
    leds_setcolor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      leds_show();

      if (handle_IR(wait)) return;

      for (uint16_t i=0; i < NUM_LEDS; i=i+3) {
    leds_setcolor(i+q, 0);        //turn every third pixel off
      }
  }
    }
}


// Local stuff 

// Single Color wipe ~K
void colorWipeSingle(uint32_t c, uint8_t wait) {
    for(uint16_t i=0; i<NUM_LEDS; i++) {
  leds_setcolor(i,c);
  leds_show();
  delay(30);
  leds_setcolor(i, CRGB::Black);
  leds_show();
  delay(30);
  if (handle_IR(wait/5)) return;
    }
}

// Flash 25 colors in the color wheel
void flash(uint8_t wait) {
    uint16_t i, j;

    for(j=0; j<26; j++) {
  for(i=0; i< NUM_LEDS; i++) {
      leds_setcolor(i, Wheel(j * 10));
  }
  leds_show();
  if (handle_IR(wait*3)) return;
  for(i=0; i< NUM_LEDS; i++) {
      leds_setcolor(i, 0);
  }
  leds_show();
  if (handle_IR(wait*3)) return;
    }
}

// Flash different color on every other led
void flash2(uint8_t wait) {
    uint16_t i, j;

    for(j=0; j<52; j++) {
  for(i=0; i< NUM_LEDS-1; i+=2) {
      leds_setcolor(i, Wheel(j * 5));
      leds_setcolor(i+1, 0);
  }
  leds_show();
  if (handle_IR(wait*2)) return;

  for(i=1; i< NUM_LEDS-1; i+=2) {
      leds_setcolor(i, Wheel(j * 5 + 48));
      leds_setcolor(i+1, 0);
  }
  leds_show();
  if (handle_IR(wait*2)) return;
    }
}

// Flash different colors on every other 2 out of 3 leds
void flash3(uint8_t wait) {
    uint16_t i, j;

    for(j=0; j<52; j++) {
  for(i=0; i< NUM_LEDS; i+=3) {
      leds_setcolor(i, Wheel(j * 5));
      leds_setcolor(i+1, Wheel(j * 5+128));
      leds_setcolor(i+2, 0);
  }
  leds_show();
  if (handle_IR(wait)) return;

  for(i=1; i< NUM_LEDS-2; i+=3) {
      leds_setcolor(i, Wheel(j * 5 + 48));
      leds_setcolor(i+1, Wheel(j * 5+128));
      leds_setcolor(i+2, 0);
  }
  leds_show();
  if (handle_IR(wait)) return;

  for(i=2; i< NUM_LEDS-2; i+=3) {
      leds_setcolor(i, Wheel(j * 5 + 96));
      leds_setcolor(i+1, Wheel(j * 5+128));
      leds_setcolor(i+2, 0);
  }
  leds_show();
  if (handle_IR(wait)) return;
    }
}

void demo(){
 f_theaterChaseRainbow;
 FastLED.delay(20);
 f_cylon;

 
}


void loop() {
    if ((uint8_t) nextdemo > 0) {
  Serial.print("Running demo: ");
  Serial.println((uint8_t) nextdemo);
    }
    switch (nextdemo) {

    // Colors on DIY1-3~~~~~~~~~~~~~~~~~~~~~~~~~~
    
    case f_colorWipe:
  colorDemo = true;
  colorWipe(demo_color, speed);
  break;

  //Kellens experiment<<<<<<<<<<<<<<<< 
   case f_demo: //If it's the case that (f_demo) is called do this
  colorDemo = false; //True checks for color demo, False for non-specific colors
//  demo();
   //Calls the function/mode
  for(uint8_t i=0; i<4; ++i){ //for loop used to repeat mode since it only goes through one time
   cylon(false, speed);
  if (handle_IR(speed)) return;
 }
   flash3(speed);
  
   flash(speed);
 
   theaterChaseRainbow(speed);
   
  
  rainbowCycle(speed);
  break;
  
    case f_theaterChase:
  colorDemo = true;
  theaterChase(demo_color, speed);
  break;

    // Rainbow anims on DIY4-6~~~~~~~~~~~~~~~~~~~

    case f_rainbowCycle:
  colorDemo = false;
  rainbowCycle(speed);
  break;
  
    case f_theaterChaseRainbow:
  colorDemo = false;
  theaterChaseRainbow(speed);
  break;

    case f_doubleConvergeRev:
  colorDemo = false;
  doubleConverge(false, speed, true);
  break;

    // Jump3 to Jump7~~~~~~~~~~~~~~~~~~~~~~~~~~~
    
    case f_cylon:
  colorDemo = false;
  cylon(false, speed);
  break;
  
    case f_cylonTrail:
  colorDemo = false;
  cylon(true, speed);
  break;
  
    case f_doubleConverge:
  colorDemo = false;
  doubleConverge(false, speed);
  break;
  
    case f_doubleConvergeTrail:
  colorDemo = false;
  doubleConverge(false, speed);
  doubleConverge(true, speed);
  break;

    // Flash color wheel~~~~~~~~~~~~~~~~~~~~~~~~~
    
    case f_flash:
  colorDemo = false;
  flash(speed);
  break;

    case f_flash3:
  colorDemo = false;
  flash3(speed);
  break;

    default:
  break;
    }

    // In case a specific demo was run with an overridden speed
    // reset the speed for the next one to the value stored in our
    // speed changing function.
    //change_speed(0);

    Serial.println("Loop done, listening for IR and restarting demo");
    // delay 80ms may work rarely
    // delay 200ms works 60-90% of the time
    // delay 500ms works no more reliably.
    if (handle_IR(1)) return;
    Serial.println("No IR");
}
