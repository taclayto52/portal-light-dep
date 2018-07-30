#include "Adafruit_WS2801.h"
#include "SPI.h" // Comment out this line if using Trinket or Gemma
#ifdef __AVR_ATtiny85__
 #include <avr/power.h>
#endif

#define NUM_SECTORS 8
#define NUM_PIXELS_PER_SECTOR 2

/*****************************************************************************
Example sketch for driving Adafruit WS2801 pixels!


  Designed specifically to work with the Adafruit RGB Pixels!
  12mm Bullet shape ----> https://www.adafruit.com/products/322
  12mm Flat shape   ----> https://www.adafruit.com/products/738
  36mm Square shape ----> https://www.adafruit.com/products/683

  These pixels use SPI to transmit the color data, and have built in
  high speed PWM drivers for 24 bit color per pixel
  2 pins are required to interface

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  BSD license, all text above must be included in any redistribution

*****************************************************************************/

// Choose which 2 pins you will use for output.
// Can be any valid output pins.
// The colors of the wires may be totally different so
// BE SURE TO CHECK YOUR PIXELS TO SEE WHICH WIRES TO USE!
uint8_t buttonPin = 2;   // Button for grabbing inputs
uint8_t dataPin  = 4;    // Yellow wire on Adafruit Pixels
uint8_t clockPin = 5;    // Green wire on Adafruit Pixels
uint8_t potPin = 0;      // Analog pin for using pot

bool setColor = false;
bool recieveIndex = true;
int currentColorIndex = 0;
int globalSectorIndex = 0;
int globalDelay = 50;
int colorArray[3] = {128, 128, 0};
byte orangeAttempt[3] = {255,40,0};
byte blueAttempt[3] = {0,190,255};
struct Colors {
  int red;
  int green;
  int blue;
  char colorFields[3][10] = {"red", "green", "blue"};
} colors;

int sectorIndices[8][2] = {{5,13},{6,14},{7,15}, {0,8}, {1,9}, {2,10}, {3,11}, {4,12}};
int sectorFadeValues[8] = {255,223,191,159,127,95,63,31};
int sectorFadePixelValues[16] = {255,239,223,207,191,175,159,143,127,111,95,79,63,47,31,15};


// Don't forget to connect the ground wire to Arduino ground,
// and the +5V wire to a +5V supply

// Set the first variable to the NUMBER of pixels. 25 = 25 pixels in a row
Adafruit_WS2801 strip = Adafruit_WS2801(16, dataPin, clockPin);

// Optional: leave off pin numbers to use hardware SPI
// (pinout is then specific to each board and can't be changed)
//Adafruit_WS2801 strip = Adafruit_WS2801(25);

// For 36mm LED pixels: these pixels internally represent color in a
// different format.  Either of the above constructors can accept an
// optional extra parameter: WS2801_RGB is 'conventional' RGB order
// WS2801_GRB is the GRB order required by the 36mm pixels.  Other
// than this parameter, your code does not need to do anything different;
// the library will handle the format change.  Examples:
//Adafruit_WS2801 strip = Adafruit_WS2801(25, dataPin, clockPin, WS2801_GRB);
//Adafruit_WS2801 strip = Adafruit_WS2801(25, WS2801_GRB);

void setup() {
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000L)
  clock_prescale_set(clock_div_1); // Enable 16 MHz on Trinket
#endif

  Serial.begin(9600);
  pinMode(buttonPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(buttonPin), setReadAnalogAndChangeCurrentColor, RISING);

  colors.red =0;
  colors.green=0;
  colors.blue=0;

  strip.begin();
  // Update LED contents, to start they are all 'off'
  strip.show();
}

void setReadAnalogAndChangeCurrentColor(){
  if(!setColor){
    setColor = !setColor;
  }
}

void serialEvent(){
//  if(recieveIndex){
//    currentColorIndex = Serial.parseInt();
//    Serial.print("Setting color index to...");
//    Serial.println(currentColorIndex);
//  }
//  else{
//    int inputColor = Serial.parseInt();
//    if(inputColor > 0 && inputColor <= 255){
//      colorArray[currentColorIndex] = inputColor;
//      Serial.print("Setting color value to...");
//      Serial.println(inputColor);
//    }
//  }
//
//  recieveIndex = !recieveIndex;
  int recievedData = Serial.parseInt();
  Serial.print("Setting global delay to...");
  Serial.println(recievedData);
  globalDelay = recievedData;
}

void loop() {
  // Some example procedures showing how to display to the pixels

//  colorWipe(Color(255, 0, 0), 50);
//  colorWipe(Color(0, 255, 0), 50);
//  colorWipe(Color(0, 0, 255), 50);
//  rainbow(20);
  int currentPotValue = analogRead(potPin);

//  int currentColorValue = ((float)currentPotValue/1024.0) * 256.0;
//  colorArray[currentColorIndex] = currentColorValue;
  if(setColor){
    for(int i=0; i<3; i++){
      Serial.print(colorArray[i]);
      if(i!=2){
        Serial.print(",");
      }
    }
    setColor = !setColor;
  }
//  colorWipe(Color(colorArray[0], colorArray[1], colorArray[2]), 0);
//  colorWipe(Color(orangeAttempt[0],orangeAttempt[1], orangeAttempt[2]), 0);
//  sectorFadeCycle(globalDelay, 0);
  granularSectorFadeCycle(globalDelay, 0);
//  setOneSectorAndClearOthers(globalSectorIndex, Color(orangeAttempt[0], orangeAttempt[1], orangeAttempt[2]));
//  rainbowCycle(20);
}

void sectorFadeCycle(int wait, uint32_t pixelColor){
  for(int i=0; i<NUM_SECTORS;i++){
    setSectorFade(i, 0);
    delay(wait);
  }
}

void granularSectorFadeCycle(int wait, uint32_t pixelColor){
  for(int i=0; i<NUM_SECTORS;i++){
    for(int j=0; j<NUM_PIXELS_PER_SECTOR; j++){
      setSectorFadeByPixel(i, j, 0);
      delay(wait);
    }
  }
}

void singleSectorCycle(uint8_t wait, uint32_t pixelColor){
  for(int i=0; i<NUM_SECTORS;i++){
    setOneSectorAndClearOthers(i, pixelColor);
    delay(wait);
  }
}

void setOneSectorAndClearOthers(int sectorIndex, uint32_t pixelColor){
  if(sectorIndex < NUM_SECTORS){
    for(int i=0; i<NUM_SECTORS; i++){
      if(i == sectorIndex){
        setSectorColorDoNotUpdateDisplay(i, pixelColor);
      }
      else{
        setSectorColorDoNotUpdateDisplay(i, 0);
      }
    }
    strip.show();
  }
}

void setSectorFade(int sectorIndex, uint32_t pixelColor){
  if(sectorIndex < NUM_SECTORS){
    for(int i=0; i<NUM_SECTORS; i++){
      int diffFromTargetSector = i - sectorIndex;

      if(diffFromTargetSector<0){
        setSectorColorDoNotUpdateDisplay(i, Color(0, 0, sectorFadeValues[diffFromTargetSector + NUM_SECTORS]));
      } else {
        setSectorColorDoNotUpdateDisplay(i, Color(0, 0, sectorFadeValues[diffFromTargetSector]));
      }
    }
    strip.show();
  }
}

void setSectorFadeByPixel(int sectorIndex, int sectorPixel, uint32_t pixelColor){
  if(sectorIndex < NUM_SECTORS && sectorPixel < NUM_PIXELS_PER_SECTOR){
    for(int i=0; i<NUM_SECTORS; i++){
      int diffFromTargetSector = i - sectorIndex;

      int adjustedOffset = 0;
      if(diffFromTargetSector<0){
        adjustedOffset = ((diffFromTargetSector + NUM_SECTORS) * 2) + sectorPixel;
        setPixelInSectorColorDoNotUpdateDisplay(i, sectorPixel, Color(0, 0, sectorFadePixelValues[adjustedOffset]));
      } else {
        adjustedOffset = ((diffFromTargetSector) * 2) + sectorPixel;
        setPixelInSectorColorDoNotUpdateDisplay(i, sectorPixel, Color(0, 0, sectorFadePixelValues[adjustedOffset]));
      }
    }
    strip.show();
  }
}

void setPixelInSectorColorDoNotUpdateDisplay(int sectorIndex, int pixelIndex, uint32_t pixelColor){
  if(sectorIndex < NUM_SECTORS && pixelIndex < NUM_PIXELS_PER_SECTOR){
    strip.setPixelColor(sectorIndices[sectorIndex][pixelIndex], pixelColor);
  }
}

void setSectorColorDoNotUpdateDisplay(int sectorIndex, uint32_t pixelColor){
  if(sectorIndex < NUM_SECTORS){
    for(int i=0; i<NUM_PIXELS_PER_SECTOR; i++){
      strip.setPixelColor(sectorIndices[sectorIndex][i], pixelColor);
    }
  }
}

void setOnePixelAndClearOthers(int pixelIndex, uint32_t pixelColor){
  if(pixelIndex < strip.numPixels()){
    for(int i=0; i<strip.numPixels(); i++){
      if(i == pixelIndex){
        strip.setPixelColor(i, pixelColor);
      }
      else{
        strip.setPixelColor(i, 0);
      }
    }
    strip.show();
  }
}

void rainbow(uint8_t wait) {
  int i, j;

  for (j=0; j < 256; j++) {     // 3 cycles of all 256 colors in the wheel
    for (i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel( (i + j) % 255));
    }
    strip.show();   // write all the pixels out
    delay(wait);
  }
}

// Slightly different, this one makes the rainbow wheel equally distributed
// along the chain
void rainbowCycle(uint8_t wait) {
  int i, j;

  for (j=0; j < 256 * 5; j++) {     // 5 cycles of all 25 colors in the wheel
    for (i=0; i < strip.numPixels(); i++) {
      // tricky math! we use each pixel as a fraction of the full 96-color wheel
      // (thats the i / strip.numPixels() part)
      // Then add in j which makes the colors go around per pixel
      // the % 96 is to make the wheel cycle around
      strip.setPixelColor(i, Wheel( ((i * 256 / strip.numPixels()) + j) % 256) );
    }
    strip.show();   // write all the pixels out
    delay(wait);
  }
}

// fill the dots one after the other with said color
// good for testing purposes
void colorWipe(uint32_t c, uint8_t wait) {
  int i;

  for (i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
}

/* Helper functions */

// Create a 24 bit color value from R,G,B
uint32_t Color(byte r, byte g, byte b)
{
  uint32_t c;
  c = r;
  c <<= 8;
  c |= g;
  c <<= 8;
  c |= b;
  return c;
}

//Input a value 0 to 255 to get a color value.
//The colours are a transition r - g -b - back to r
uint32_t Wheel(byte WheelPos)
{
  if (WheelPos < 85) {
   return Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if (WheelPos < 170) {
   WheelPos -= 85;
   return Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

