#include "LPD8806.h"
#include "SPI.h"

const int POT_INPUT = A0;
int red = 0;
int blue = 0;
int value = 0;
int nLEDs = 18;
LPD8806 strip = LPD8806(nLEDs, 2, 3);

void setup()
{
  // Serial.begin(9600); // Sets up serial communication and sets the rate
   /* Map an analog value to 8 bits (0 to 255) */
  // Start up the LED strip
  strip.begin();

  // Update the strip, to start they are all 'off'
  strip.show();
}

void loop()
{
  value = analogRead(POT_INPUT);
//   Serial.println(value);
   
  if (value < 512)
  {
    red = map(value, 0, 512, 0, 127);
    blue = 127;
  }
  
  else
  {
    int val2 = 1023 - value;
    blue = map(val2, 0, 511, 0, 127);
    red = 127;
  }
  
  for (int i = 0; i < strip.numPixels(); i++)
  {
      strip.setPixelColor(i, red, 0, blue);
  }
  strip.show();
}
   


