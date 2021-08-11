// Pixel Badge v2.2
// @blinkingthing

#include <Wire.h>
#include <Adafruit_NeoPixel.h>
#include "EEPROM.h"

#define INT_LED_PIN    19  // Digital IO pin connected to the NeoPixels.
#define EXT_LED_PIN    21  // Digital IO pin connected to the NeoPixels.

#define INT_PIXEL_COUNT 3  // Number of NeoPixels (internal)
#define EXT_PIXEL_COUNT 30  // Number of NeoPixels (external)

#define TIMER_MS 5000

#define SAO_EEPROM   0x50              // 7 bit I2C address for CAAT24C03 EEPROM

// the current address in the EEPROM (i.e. which byte
// we're going to write to next)
int addr = 0;
int hue1address = 10;
int hue2address = 20;
int hue3address = 30;

// Declare our NeoPixel strip object:
Adafruit_NeoPixel badgepixels(INT_PIXEL_COUNT, INT_LED_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip(EXT_PIXEL_COUNT, EXT_LED_PIN, NEO_GRB + NEO_KHZ800);
// Argument 1 = Number of pixels in NeoPixel strip
// Argument 2 = Arduino pin number (most are valid)
// Argument 3 = Pixel type flags, add together as needed:

//from pixelbadge v1
uint8_t saodata[] = {0xFF, 0xFF};
uint8_t pixeldata[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
uint8_t saomodes[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x10, 0x11, 0x12, 0x13, 0x14};
uint8_t speeds[] = {0x00, 0x01, 0xAA, 0xFF};
uint8_t red[] = {0x00, 0x01, 0xAA, 0xFF};
uint8_t green[] = {0x00, 0x01, 0xAA, 0xFF};
uint8_t blue[] = {0x00, 0x01, 0xAA, 0xFF};
uint8_t functions[] = {0x00, 0x01, 0x02, 0x03, 0x04};

//doom sao
//0x04 health 0x05 anger
uint8_t health[] = {0x00, 0x14, 0x28, 0x3C, 0x50};
uint8_t anger[] = {0x00, 0x14, 0x28, 0x3C, 0x50};
uint8_t lowkey[] = {0x00, 0x37};
uint8_t incon[] = {0x00, 0x01};


int oldFunction;
int currentFunction = 4; //for shitty pixel 0 = animations, for doomsao 4 = health
int oldValue;
int currentValue = 3;
int modeValue = 3;
int speedValue = 3;
int redValue = 3;
int greenValue = 3;
int blueValue = 3;
int healthValue = 6;
int angerValue = 0;
int lowkeyValue = 0;
int inconValue = 0;
int limit;


// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMillis = 0;        // will store last time LED was updated

// constants won't change:
const long interval = 100;           // interval at which to blink (milliseconds)

boolean oldState = HIGH;
boolean activeTouch = false;

int badgeMode = 0;    // Currently-active animation mode, 0-14
int modes = 16;
int masterWait = 25;

bool forward = true;

int threshold = 17; // minimum touch input threshole (smaller = more sensitive) 17 seems to be a good spot for the pixel badge for now
// had it at 20 for a while but then a few badges were getting errant button presses

int masterBrightness = 50;

int brightStep = 5;

int currentLed = 0;

int saoRegister = 0;
int doomGuySecret = 0;

const int defHue1 = 20000; //g
const int defHue2 = 0; // r
const int defHue3 = 40000; // b

int userHue1;
int userHue2;
int userHue3;

bool touch1detected = false;
bool touch2detected = false;
bool touch3detected = false;
bool touch4detected = false;
bool touch5detected = false;
bool touch6detected = false;
bool touch7detected = false;
bool touch8detected = false;
bool touch9detected = false;
bool touch10detected = false;

void gotTouch1() {
  touch1detected = true;
}

void gotTouch2() {
  touch2detected = true;
}

void gotTouch3() {
  touch3detected = true;
}

void gotTouch4() {
  touch4detected = true;
}

void gotTouch5() {
  touch5detected = true;
}

void gotTouch6() {
  touch6detected = true;
}

void gotTouch7() {
  // touch7detected = true;
}

void gotTouch8() {
  touch8detected = true;
}

void gotTouch9() {
  // touch9detected = true;
}

void gotTouch10() {
  //touch10detected = true;
}


//TaskHandle_t Task1;
//TaskHandle_t Task2;

void setup() {
  Serial.begin(115200);
  Wire.begin (21, 22);   // sda= GPIO_21 /scl= GPIO_22
  delay(1000); // give me time to bring up serial monitor

  //disable touch
  touchAttachInterrupt(T0, gotTouch1, threshold); //pin 4 H2
  touchAttachInterrupt(T5, gotTouch2, threshold); // 12 H7
  touchAttachInterrupt(T6, gotTouch3, threshold); // 14 H6
  touchAttachInterrupt(T7, gotTouch4, threshold); // 27 H5
  touchAttachInterrupt(T8, gotTouch5, threshold); // 33 H9
  touchAttachInterrupt(T9, gotTouch6, threshold); // 32 H10
  //touchAttachInterrupt(T1, gotTouch7, threshold); // 0 H1 BROKEN
  touchAttachInterrupt(T2, gotTouch8, threshold); // 2 H3
  //touchAttachInterrupt(T3, gotTouch9, threshold); // 15 H4 BROKEN
  //touchAttachInterrupt(T4, gotTouch10, threshold); // 13 H8 BROKEN

  //read eeprom




  if (!EEPROM.begin(64)) {
    Serial.println("Failed to initialise EEPROM");
    Serial.println("Restarting...");
    delay(1000);
    ESP.restart();
  }
  Serial.println("EEPROM Read");
  Serial.print("restored hue 1 : ");
  Serial.println(EEPROM.get(hue1address, userHue1));
  Serial.print("restored hue 2 : ");
  Serial.println(EEPROM.get(hue2address, userHue2));
  Serial.print("restored hue 3 : ");
  Serial.println(EEPROM.get(hue3address, userHue3));
  userHue1 = EEPROM.get(hue1address, userHue1);
  userHue2 = EEPROM.get(hue2address, userHue2);
  userHue3 = EEPROM.get(hue3address, userHue3);
  //Serial.println("Stored UserHue1 : " + String( userHue1));
  //Serial.println("Stored UserHue2 : " + String( userHue2));
  //Serial.println("Stored UserHue3 : " + String( userHue3));

  //xTaskCreatePinnedToCore(
  //    loop1, /* Function to implement the task */
  //    "loop1", /* Name of the task */
  //    4096,  /* Stack size in words */
  //    NULL,  /* Task input parameter */
  //    1,  /* Priority of the task */
  //    &Task1,  /* Task handle. */
  //    0); /* Core where the task should run */

  //xTaskCreatePinnedToCore(
  //    loop2, /* Function to implement the task */
  //    "loop2", /* Name of the task */
  //    4096,  /* Stack size in words */
  //    NULL,  /* Task input parameter */
  //    1,  /* Priority of the task */
  //    &Task2,  /* Task handle. */
  //    1); /* Core where the task should run */

  badgepixels.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  badgepixels.show();            // Turn OFF all pixels ASAP
  badgepixels.setBrightness(masterBrightness); // Set BRIGHTNESS to about 1/5 (max = 255)
  badgepixels.setPixelColor(0, 0, 255, 0);
  badgepixels.setPixelColor(1, 0, 255, 0);
  badgepixels.setPixelColor(2, 0, 0, 255);
  badgepixels.show();



}

void loop() {
  // check to see if it's time to blink the LED; that is, if the difference
  // between the current time and last time you blinked the LED is bigger than
  // the interval at which you want to blink the LED.
  unsigned long currentMillis = millis();

  //if (currentMillis - previousMillis >= interval) {
  // save the last time you blinked the LED
  //previousMillis = currentMillis;

  //do somethign every interval
  switch (badgeMode) {          // Start the new animation...
    case 0:
      defaultBlink();
      break;
    //user defineable mode
    case 1:
      userBlink();
      break;
    case 2:
      userChase();
      break;
    case 3:
      userBounce();
      break;
    case 4:
      userSolid();
      break;
    case 5:
      userFade();
      break;
    case 6:
      saoControl();
      break;
    case 7:
      colorWipe(badgepixels.Color(0,   255,   0), 50);    // Green
      break;
    case 8:
      colorWipe(badgepixels.Color(  0, 0,   255), 50);    // Blue
      break;
    case 9:
      colorWipe(badgepixels.Color(  75,   0, 130), 50);    // Indigo
      break;
    case 10:
      colorWipe(badgepixels.Color(  255,   0, 255), 50);    // Violet
      break;
    case 11:
      colorWipe(badgepixels.Color(  255,   255, 255), 50);    // Violet
      break;
    case 12:
      theaterChase(badgepixels.Color(127, 127, 127), 50); // White
      break;
    case 13:
      theaterChase(badgepixels.Color(127,   0,   0), 50); // Red
      break;
    case 14:
      theaterChase(badgepixels.Color(  0,   0, 127), 50); // Blue
      break;
    case 15:
      rainbow(masterWait);
      break;
    case 16:
      theaterChaseRainbow(masterWait);
      break;
    case 17:
      rainbowStretch(masterWait);
      break;
  }
  //}

  //touch checks
  if (touch1detected) {
    aniUp();

    touch1detected = false;
    Serial.print("touchRead : ");
    Serial.print(touchRead(T0));
    Serial.print(". ");
    Serial.println("Touch 1 detected");



    //i2c scanner will trigger every time you press animation up
    //temporary test
    //Scanner ();

  }
  if (touch2detected) {
    //BLUE LED (right)
    //touch2detected = false;

    singlePixelChange(2);
    if (badgeMode == 5) {
      saoRegister = 6;
    }
    touch2detected = false;
    Serial.print("touchRead : ");
    Serial.print(touchRead(T5));
    Serial.print(". ");
    Serial.println("Touch 2 detected");
  }

  if (touch3detected) {
    //RED LED (center)
    singlePixelChange(1);
    if (badgeMode == 5) {
      saoRegister = 5;
    }

    //slow down
    /*if (masterWait + 1 < 50){
      masterWait = masterWait+ 1;
      } else {
      masterWait = 0;
      }*/
    touch3detected = false;
    Serial.print("touchRead : ");
    Serial.print(touchRead(T6));
    Serial.print(". ");
    Serial.println("Touch 3 detected");
  }
  if (touch4detected) {
    //GREEN LED (left)
    singlePixelChange(0);
    if (badgeMode == 5) {
      saoRegister = 4;
    }
    touch4detected = false;
    Serial.print("touchRead : ");
    Serial.print(touchRead(T7));
    Serial.print(". ");
    Serial.println("Touch 4 detected");
  }
  if (touch5detected) {
    if (badgeMode == 0) {        //config mode 0
      saveUserHue();
    } else if (badgeMode == 5) { //sao mode +
      increaseValue();
      switch (saoRegister){
        case 4: writeToSAOEEPROM(saoRegister, health[currentValue]); break;
        case 5: writeToSAOEEPROM(saoRegister, anger[currentValue]); break;
        case 6: writeToSAOEEPROM(0x13, 0x37); break;
        }
      
    } else {
      brightnessUp();
    }
    touch5detected = false;
    Serial.print("touchRead : ");
    Serial.print(touchRead(T8));
    Serial.print(". ");
    Serial.println("Touch 5 detected");
  }
  if (touch6detected) {
    if (badgeMode == 0) {      //config mode 0
      restoreDefaultHue();
    } else if (badgeMode == 5) { //sao mode -
      decreaseValue();
      switch (saoRegister){
        case 4: writeToSAOEEPROM(saoRegister, health[currentValue]); break;
        case 5: writeToSAOEEPROM(saoRegister, anger[currentValue]); break;
        case 6: writeToSAOEEPROM(0x13, 0x00); break;
        }
    } else {
      brightnessDown();
    }
    touch6detected = false;
    Serial.print("touchRead : ");
    Serial.print(touchRead(T9));
    Serial.print(". ");
    Serial.println("Touch 6 detected");
    Serial.println("Current Mode : " + String(badgeMode));
  }
  if (touch7detected) {


    touch7detected = false;
    Serial.println("Touch 7 detected");
  }
  if (touch8detected) {
    aniDown();

    touch8detected = false;
    Serial.print("touchRead : ");
    Serial.print(touchRead(T2));
    Serial.print(". ");
    Serial.println("Touch 8 detected");
  }
  if (touch9detected) {


    touch9detected = false;
    Serial.println("Touch 9 detected");
  }
  if (touch10detected) {


    touch10detected = false;
    Serial.println("Touch 10 detected");
  }
};
/*void loop1(void * parameter){
  for(;;){
    //ws2812fx.service();



  }
  }

  void loop2(void * parameter) {
  for (;;){

  }
  }*/


void aniUp() {
  if (++badgeMode > modes) badgeMode = 0; // Advance to next mode, wrap around after #8
  colorWipe(badgepixels.Color(  255,   255, 255), 50) ;//white wipe right
  colorWipe(badgepixels.Color(  0,   0, 0), 50) ;//black wipe right
  //ws2812fx.setMode((ws2812fx.getMode() + 1) % ws2812fx.getModeCount());
}


void aniDown() {
  if (--badgeMode < 0) badgeMode = modes; // Advance to next mode, wrap around after #8
  revColorWipe(badgepixels.Color(  255,   255, 255), 50) ;//white wipe left
  revColorWipe(badgepixels.Color(  0,   0, 0), 50) ;//black wipe left
  // ws2812fx.setMode((ws2812fx.getMode() - 1) % ws2812fx.getModeCount());
}

void brightnessUp() {
  if (masterBrightness + brightStep > 255 ) {
    masterBrightness = 255;
  } else {
    masterBrightness = masterBrightness + brightStep;
  }
  badgepixels.setBrightness(masterBrightness);
  badgepixels.show();
}

void brightnessDown() {
  if (masterBrightness - brightStep < 0 ) {
    masterBrightness = 0;
  } else {
    masterBrightness = masterBrightness - brightStep;
  }
  badgepixels.setBrightness(masterBrightness);
  badgepixels.show();
}



void colorSnap(uint32_t color, int wait) {
  for (int i = 0; i < badgepixels.numPixels(); i++) { // For each pixel in strip...
    badgepixels.setPixelColor(i, color);         //  Set pixel's color (in RAM)

    delay(wait);                           //  Pause for a moment
  }
  badgepixels.show();                          //  Update strip to match
}

// Fill strip pixels one after another with a color. Strip is NOT cleared
// first; anything there will be covered pixel by pixel. Pass in color
// (as a single 'packed' 32-bit value, which you can get by calling
// strip.Color(red, green, blue) as shown in the loop() function above),
// and a delay time (in milliseconds) between pixels.
void colorWipe(uint32_t color, int wait) {
  for (int i = 0; i < badgepixels.numPixels(); i++) { // For each pixel in strip...
    badgepixels.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    badgepixels.show();                          //  Update strip to match
    delay(wait);                           //  Pause for a moment
  }
}

void revColorWipe(uint32_t color, int wait) {
  for (int i = badgepixels.numPixels(); i >= 0; i--) { // For each pixel in strip...
    badgepixels.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    badgepixels.show();                          //  Update strip to match
    delay(wait);                           //  Pause for a moment
  }
}

// Theater-marquee-style chasing lights. Pass in a color (32-bit value,
// a la strip.Color(r,g,b) as mentioned above), and a delay time (in ms)
// between frames.
void theaterChase(uint32_t color, int wait) {
  for (int a = 0; a < 10; a++) { // Repeat 10 times...
    //exit if touch detected
    if (touch1detected || touch2detected || touch3detected || touch4detected || touch8detected) {
      break;
    }
    for (int b = 0; b < 3; b++) { //  'b' counts from 0 to 2...
      badgepixels.clear();         //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in steps of 3...
      for (int c = b; c < badgepixels.numPixels(); c += 3) {
        badgepixels.setPixelColor(c, color); // Set pixel 'c' to value 'color'
      }
      badgepixels.show(); // Update strip with new contents
      delay(wait);  // Pause for a moment
    }
  }
}

// Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
void rainbow(int wait) {
  // Hue of first pixel runs 3 complete loops through the color wheel.
  // Color wheel has a range of 65536 but it's OK if we roll over, so
  // just count from 0 to 3*65536. Adding 256 to firstPixelHue each time
  // means we'll make 3*65536/256 = 768 passes through this outer loop:
  for (long firstPixelHue = 0; firstPixelHue < 3 * 65536; firstPixelHue += 256) {
    //exit if touch detected
    if (touch1detected || touch2detected || touch3detected || touch4detected || touch8detected) {
      break;
    }
    for (int i = 0; i < badgepixels.numPixels(); i++) { // For each pixel in strip...
      // Offset pixel hue by an amount to make one full revolution of the
      // color wheel (range of 65536) along the length of the strip
      // (strip.numPixels() steps):
      int pixelHue = firstPixelHue + (i * 65536L / badgepixels.numPixels());
      // strip.ColorHSV() can take 1 or 3 arguments: a hue (0 to 65535) or
      // optionally add saturation and value (brightness) (each 0 to 255).
      // Here we're using just the single-argument hue variant. The result
      // is passed through strip.gamma32() to provide 'truer' colors
      // before assigning to each pixel:
      badgepixels.setPixelColor(i, badgepixels.gamma32(badgepixels.ColorHSV(pixelHue)));
    }
    badgepixels.show(); // Update strip with new contents
    delay(wait);  // Pause for a moment
  }
}

// Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
void rainbowStretch(int wait) {
  // Hue of first pixel runs 3 complete loops through the color wheel.
  // Color wheel has a range of 65536 but it's OK if we roll over, so
  // just count from 0 to 3*65536. Adding 256 to firstPixelHue each time
  // means we'll make 3*65536/256 = 768 passes through this outer loop:
  for (long firstPixelHue = 0; firstPixelHue < 3 * 65536; firstPixelHue += 256) {
    //exit if touch detected
    if (touch1detected || touch2detected || touch3detected || touch4detected || touch8detected) {
      break;
    }
    for (int i = 0; i < 256; i++) { // For each pixel in strip...
      // Offset pixel hue by an amount to make one full revolution of the
      // color wheel (range of 65536) along the length of the strip
      // (strip.numPixels() steps):
      int pixelHue = firstPixelHue + (i * 65536L / 256);
      // strip.ColorHSV() can take 1 or 3 arguments: a hue (0 to 65535) or
      // optionally add saturation and value (brightness) (each 0 to 255).
      // Here we're using just the single-argument hue variant. The result
      // is passed through strip.gamma32() to provide 'truer' colors
      // before assigning to each pixel:
      badgepixels.setPixelColor(i, badgepixels.gamma32(badgepixels.ColorHSV(pixelHue)));
    }
    badgepixels.show(); // Update strip with new contents
    delay(wait);  // Pause for a moment
  }
}
// Rainbow-enhanced theater marquee. Pass delay time (in ms) between frames.
void theaterChaseRainbow(int wait) {
  int firstPixelHue = 0;     // First pixel starts at red (hue 0)
  for (int a = 0; a < 30; a++) { // Repeat 30 times...

    //exit if touch detected
    if (touch1detected || touch2detected || touch3detected || touch4detected || touch8detected) {
      break;
    }
    for (int b = 0; b < 3; b++) { //  'b' counts from 0 to 2...
      badgepixels.clear();         //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in increments of 3...
      for (int c = b; c < badgepixels.numPixels(); c += 3) {
        // hue of pixel 'c' is offset by an amount to make one full
        // revolution of the color wheel (range 65536) along the length
        // of the strip (strip.numPixels() steps):
        int      hue   = firstPixelHue + c * 65536L / badgepixels.numPixels();
        uint32_t color = badgepixels.gamma32(badgepixels.ColorHSV(hue)); // hue -> RGB
        badgepixels.setPixelColor(c, color); // Set pixel 'c' to value 'color'
      }
      badgepixels.show();                // Update strip with new contents
      delay(wait);                 // Pause for a moment
      firstPixelHue += 65536 / 90; // One cycle of color wheel over 90 frames
    }
  }
}

// Breathing
void breath() {
  float MaximumBrightness = 255;
  float SpeedFactor = 0.08; // I don't actually know what would look good
  float StepDelay = 5; // ms for a step delay on the lights

  // Make the lights breathe
  for (int i = 0; i < 256; i++) {
    //exit if touch detected
    if (touch1detected || touch2detected || touch3detected || touch4detected || touch8detected) {
      break;
    }
    // Intensity will go from 10 - MaximumBrightness in a "breathing" manner
    float intensity = MaximumBrightness / 2.0 * (1.0 + sin(SpeedFactor * i));
    badgepixels.setBrightness(intensity);
    // Now set every LED to that color
    for (int ledNumber = 0; ledNumber < INT_PIXEL_COUNT; ledNumber++) {
      badgepixels.setPixelColor(ledNumber, 255, 0, 0);
    }

    badgepixels.show();
    //Wait a bit before continuing to breathe
    delay(StepDelay);

  }
}
// default Blink
void defaultBlink() {
  float MaximumBrightness = 255;
  float SpeedFactor = 0.02; // I don't actually know what would look good
  float StepDelay = 5; // ms for a step delay on the lights

  // Make the lights breathe
  for (int i = 0; i < 65535; i++) {
    //exit if touch detected
    if (touch1detected || touch2detected || touch3detected || touch4detected || touch8detected || touch5detected || touch6detected) {
      break;
    }
    // Intensity will go from 10 - MaximumBrightness in a "breathing" manner
    float intensity = MaximumBrightness / 2.0 * (1.0 + sin(SpeedFactor * i));
    badgepixels.setBrightness(intensity);
    // Now set every LED to that color
    badgepixels.setPixelColor(0, badgepixels.gamma32(badgepixels.ColorHSV(defHue1)));
    badgepixels.setPixelColor(1, badgepixels.gamma32(badgepixels.ColorHSV(defHue2)));
    badgepixels.setPixelColor(2, badgepixels.gamma32(badgepixels.ColorHSV(defHue3)));

    badgepixels.show();
    //Wait a bit before continuing to blink
    delay(StepDelay);

  }
};

// user blink
void userBlink() {
  float MaximumBrightness = 255;
  float SpeedFactor = 0.02; // I don't actually know what would look good
  float StepDelay = 5; // ms for a step delay on the lights

  if (!userHue1) {
    userHue1 = defHue1;
  }
  if (!userHue2) {
    userHue2 = defHue2;
  }
  if (!userHue1) {
    userHue3 = defHue3;
  }

  // Make the lights breathe
  for (int i = 0; i < 65535; i++) {
    //exit if touch detected
    if (touch1detected || touch2detected || touch3detected || touch4detected || touch8detected || touch5detected || touch6detected) {
      break;
    }
    // Intensity will go from 10 - MaximumBrightness in a "breathing" manner
    float intensity = MaximumBrightness / 2.0 * (1.0 + sin(SpeedFactor * i));
    badgepixels.setBrightness(intensity);
    // Now set every LED to that color
    //strip.setPixelColor(0, 0, 25, 0);
    //strip.setPixelColor(1, 255, 0, 0);
    //strip.setPixelColor(2, 0, 0, 25);
    badgepixels.setPixelColor(0, badgepixels.gamma32(badgepixels.ColorHSV(userHue1)));
    badgepixels.setPixelColor(1, badgepixels.gamma32(badgepixels.ColorHSV(userHue2)));
    badgepixels.setPixelColor(2, badgepixels.gamma32(badgepixels.ColorHSV(userHue3)));

    badgepixels.show();
    //Wait a bit before continuing to breathe
    delay(StepDelay);

  }
};

void userChase() {
  // the interval at which you want to blink the LED.
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    if (++currentLed > 2) currentLed = 0;
    badgepixels.clear();
    switch (currentLed) {
      case 0:
        badgepixels.setPixelColor(currentLed, badgepixels.gamma32(badgepixels.ColorHSV(userHue1)));
        break;
      case 1:
        badgepixels.setPixelColor(currentLed, badgepixels.gamma32(badgepixels.ColorHSV(userHue2)));
        break;
      case 2:
        badgepixels.setPixelColor(currentLed, badgepixels.gamma32(badgepixels.ColorHSV(userHue3)));
        break;
    }
    badgepixels.show();
  }
}

void userBounce() {
  // the interval at which you want to blink the LED.
  unsigned long currentMillis = millis();


  if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;
    if (forward) {
      if (++currentLed > 2) forward = false;
    } else {
      if (--currentLed < 0) forward = true;
    }

    badgepixels.clear();
    switch (currentLed) {
      case 0:
        badgepixels.setPixelColor(currentLed, badgepixels.gamma32(badgepixels.ColorHSV(userHue1)));
        break;
      case 1:
        badgepixels.setPixelColor(currentLed, badgepixels.gamma32(badgepixels.ColorHSV(userHue2)));
        break;
      case 2:
        badgepixels.setPixelColor(currentLed, badgepixels.gamma32(badgepixels.ColorHSV(userHue3)));
        break;
    }
    badgepixels.show();
  }
}

void userSolid() {


  badgepixels.setPixelColor(0, badgepixels.gamma32(badgepixels.ColorHSV(userHue1)));
  badgepixels.setPixelColor(1, badgepixels.gamma32(badgepixels.ColorHSV(userHue2)));
  badgepixels.setPixelColor(2, badgepixels.gamma32(badgepixels.ColorHSV(userHue3)));
  badgepixels.show();

}

const uint32_t Blue = Adafruit_NeoPixel::Color(10, 10, 255);
const uint32_t Pink = Adafruit_NeoPixel::Color(255, 0, 100);


void userFade() {
  crossFade(badgepixels.gamma32(badgepixels.ColorHSV(userHue1)), badgepixels.gamma32(badgepixels.ColorHSV(userHue2)), 10UL);
  crossFade(badgepixels.gamma32(badgepixels.ColorHSV(userHue2)), badgepixels.gamma32(badgepixels.ColorHSV(userHue3)), 10UL);
  crossFade(badgepixels.gamma32(badgepixels.ColorHSV(userHue3)), badgepixels.gamma32(badgepixels.ColorHSV(userHue1)), 10UL);
}

void setAllStrips(const uint32_t color) {
  byte red = (color >> 16) & 0xff;
  byte green = (color >> 8) & 0xff;
  byte blue = color & 0xff;
  setAllStrips(red, green, blue);
}

void setAllStrips(byte red, byte green, byte blue) {
  // for each strip
  for (int strip = 0; strip < INT_PIXEL_COUNT; strip++) {
    // for each pixel
    for (int pixel = 0; pixel < INT_PIXEL_COUNT; pixel++) {
      badgepixels.setPixelColor(pixel, red, green, blue);
    }
    badgepixels.show(); // Output to strip
  }
}

void crossFade(const uint32_t startColor, const uint32_t endColor, unsigned long speed) {

  byte startRed = (startColor >> 16) & 0xff;
  byte startGreen = (startColor >> 8) & 0xff;
  byte startBlue = startColor & 0xff;

  byte endRed = (endColor >> 16) & 0xff;
  byte endGreen = (endColor >> 8) & 0xff;
  byte endBlue = endColor & 0xff;

  // for each step in the cross-fade
  for (int step = 0; step < 256; step++) {
    if (touch1detected || touch2detected || touch3detected || touch4detected || touch8detected || touch5detected || touch6detected) {
      break;
    }
    byte red = map(step, 0, 255, startRed, endRed);
    byte green = map(step, 0, 255, startGreen, endGreen);
    byte blue = map(step, 0, 255, startBlue, endBlue);
    setAllStrips(red, green, blue);
    delay(speed);
  }
}

void singlePixelChange(int pixel) {
  switch (pixel) {
    case 0:
      if (userHue1 + 1000 > 65535) {
        userHue1 = 0;
      } else {
        userHue1 = userHue1 + 100;
      }
      // strip.ColorHSV() can take 1 or 3 arguments: a hue (0 to 65535) or
      // optionally add saturation and value (brightness) (each 0 to 255).
      // Here we're using just the single-argument hue variant. The result
      // is passed through strip.gamma32() to provide 'truer' colors
      // before assigning to each pixel:
      //strip.setPixelColor(pixel, strip.gamma32(strip.ColorHSV(userHue1)));
      //strip.show();
      break;

    case 1:
      if (userHue2 + 1000 > 65535) {
        userHue2 = 0;
      } else {
        userHue2 = userHue2 + 100;
      }
      // strip.ColorHSV() can take 1 or 3 arguments: a hue (0 to 65535) or
      // optionally add saturation and value (brightness) (each 0 to 255).
      // Here we're using just the single-argument hue variant. The result
      // is passed through strip.gamma32() to provide 'truer' colors
      // before assigning to each pixel:
      //strip.setPixelColor(pixel, strip.gamma32(strip.ColorHSV(userHue2)));
      //strip.show();
      break;

    case 2:
      if (userHue3 + 1000 > 65535) {
        userHue3 = 0;
      } else {
        userHue3 = userHue3 + 100;
      }
      // strip.ColorHSV() can take 1 or 3 arguments: a hue (0 to 65535) or
      // optionally add saturation and value (brightness) (each 0 to 255).
      // Here we're using just the single-argument hue variant. The result
      // is passed through strip.gamma32() to provide 'truer' colors
      // before assigning to each pixel:
      //strip.setPixelColor(pixel, strip.gamma32(strip.ColorHSV(userHue3)));
      //strip.show();
      break;
  };
}

void Scanner ()
{
  Serial.println ();
  Serial.println ("I2C scanner. Scanning ...");
  byte count = 0;

  Wire.begin();
  for (byte i = 8; i < 120; i++)
  {
    Wire.beginTransmission (i);          // Begin I2C transmission Address (i)
    if (Wire.endTransmission () == 0)  // Receive 0 = success (ACK response)
    {
      Serial.print ("Found address: ");
      Serial.print (i, DEC);
      Serial.print (" (0x");
      Serial.print (i, HEX);     // PCF8574 7 bit address
      Serial.println (")");
      count++;
    }
  }
  Serial.print ("Found ");
  Serial.print (count, DEC);        // numbers of devices
  Serial.println (" device(s).");
}


void writeToSAOEEPROM(byte reg, byte SAOdata) {
  Wire.beginTransmission(SAO_EEPROM); //first byte is SAO_EEPROM address
  Wire.write(reg);
  Wire.write(SAOdata);

  Wire.endTransmission();
  delay(200);

  Serial.println("SAO EEPROM I2C address : " + String(SAO_EEPROM));
  Serial.println("Current Badge Function : " + String(currentFunction));

  Serial.println("Current SAO Option Iterator  : " + String(currentValue));
  
  Serial.println("Current saoRegister : " + String(saoRegister));

  Serial.println("Current Mode : " + String(modeValue));
  Serial.println("EEPROM Address : " + String(currentFunction));
  Serial.println("Data to write  : " + String(SAOdata));

  //Serial.println("Current Speed : " + String(speedValue));
  //Serial.println("Current Red : " + String(redValue));
  //Serial.println("Current Green : " + String(greenValue));
  //Serial.println("Current Blue : " + String(blueValue));
}

void increaseValue() {
  switch (saoRegister){
    case 4: //health
      if (currentValue + 1 > 5){ // if hit limit
        currentValue = 0; //loop back to 0
      } else {
        currentValue++;
        };
        break;
    case 5: //anger 
      if (currentValue + 1 > 5){ // if hit limit
        currentValue = 0; //loop back to 0
      } else {
        currentValue++;
        };
        break;
      
    }
}

void decreaseValue() {
  switch (saoRegister){
    case 4: //health
      if (currentValue - 1 < 0){ // if hit limit
        currentValue = 4; //loop back to 0
      } else {
        currentValue--;
        };
        break;
    case 5: //anger 
      if (currentValue - 1 < 0){ // if hit limit
        currentValue = 4; //loop back to 0
      } else {
        currentValue--;
        };
        break;
      
    }
}
  


void saveUserHue() {
  Serial.println("Saving User Hue");

  EEPROM.put(hue1address, userHue1);
  delay(100);
  EEPROM.put(hue2address, userHue2);
  delay(100);
  EEPROM.put(hue3address, userHue3);
  delay(100);
  EEPROM.commit();

  //three green flashes
  for (int i = 0; i < 4; i++) {
    badgepixels.setPixelColor(0, 0, 255, 0); // Set pixel 'c' to value 'color'
    badgepixels.setPixelColor(1, 0, 255, 0); // Set pixel 'c' to value 'color'
    badgepixels.setPixelColor(2, 0, 255, 0); // Set pixel 'c' to value 'color'
    badgepixels.show();
    delay(100);
    badgepixels.setPixelColor(0, 0, 0, 0); // Set pixel 'c' to value 'color'
    badgepixels.setPixelColor(1, 0, 0, 0); // Set pixel 'c' to value 'color'
    badgepixels.setPixelColor(2, 0, 0, 0); // Set pixel 'c' to value 'color'
    badgepixels.show();
    delay(100);
  }

}

void restoreDefaultHue() {

  Serial.println("restoring default hues");

  badgepixels.setPixelColor(0, badgepixels.gamma32(badgepixels.ColorHSV(defHue1)));
  badgepixels.setPixelColor(1, badgepixels.gamma32(badgepixels.ColorHSV(defHue2)));
  badgepixels.setPixelColor(2, badgepixels.gamma32(badgepixels.ColorHSV(defHue3)));
  badgepixels.show();

  userHue1 = defHue1;
  userHue2 = defHue2;
  userHue3 = defHue3;

  Serial.print("Userhue1 : ");
  Serial.println(userHue1);
  Serial.print("Userhue2 : ");
  Serial.println(userHue2);
  Serial.print("Userhue3 : ");
  Serial.println(userHue3);

  EEPROM.put(hue1address, userHue1);
  delay(100);
  EEPROM.put(hue2address, userHue2);
  delay(100);
  EEPROM.put(hue3address, userHue3);
  delay(100);

  EEPROM.commit();

  Serial.print("EEPROM Userhue1 : ");
  Serial.println(EEPROM.get(hue1address, userHue1));
  Serial.print("EEPROM Userhue2 : ");
  Serial.println(EEPROM.get(hue2address, userHue2));
  Serial.print("EEPROM Userhue3 : ");
  Serial.println(EEPROM.get(hue3address, userHue3));

  //two blue flashes
  for (int i = 0; i < 4; i++) {
    badgepixels.setPixelColor(0, 0, 0, 255); // Set pixel 'c' to value 'color'
    badgepixels.setPixelColor(1, 0, 0, 255); // Set pixel 'c' to value 'color'
    badgepixels.setPixelColor(2, 0, 0, 255); // Set pixel 'c' to value 'color'
    badgepixels.show();
    delay(100);
    badgepixels.setPixelColor(0, 0, 0, 0); // Set pixel 'c' to value 'color'
    badgepixels.setPixelColor(1, 0, 0, 0); // Set pixel 'c' to value 'color'
    badgepixels.setPixelColor(2, 0, 0, 0); // Set pixel 'c' to value 'color'
    badgepixels.show();
    delay(100);
  }


}


void saoControl() {
  switch (saoRegister) {
    case 4 :
      //three green flashes
      badgepixels.setPixelColor(0, 0, 255, 0); // Set pixel 'c' to value 'color'
      badgepixels.show();
      delay(100);
      badgepixels.setPixelColor(0, 0, 0, 0); // Set pixel 'c' to value 'color'
      badgepixels.show();
      delay(100);
      break;
    case 5 :
      //three red flashes
      badgepixels.setPixelColor(1, 255, 0, 0); // Set pixel 'c' to value 'color'
      badgepixels.show();
      delay(100);
      badgepixels.setPixelColor(1, 0, 0, 0); // Set pixel 'c' to value 'color'
      badgepixels.show();
      delay(100);
      break;
    case 6 :
      //three blue flashes
      badgepixels.setPixelColor(2, 0, 0, 255); // Set pixel 'c' to value 'color'
      badgepixels.show();
      delay(100);
      badgepixels.setPixelColor(2, 0, 0, 0); // Set pixel 'c' to value 'color'
      badgepixels.show();
      delay(100);
      break;
  }

  


};
