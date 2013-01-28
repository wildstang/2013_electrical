/*
WildStang Signs 2013
By: Josh Smith and Steve Garward
*/

#include "LPD8806.h"
#include "SPI.h"
#include <Wire.h>

//Define Data Pins here
int dataPin = 9;
int clockPin = 11;

//The number of LEDs on the sign
int numLeds = 52;

//The global variable that determines if the device joins as a master or slave for I2C
boolean master = true;

//This is the address to use if this is a slave device
int i2cAddress = 82;  //HEX 0x52

//Defines if old data = new data. Leave this alone as it is set automatically
boolean dataChanged = false;

//Create the variables that are sent to the slave devices from the master
unsigned char lightFunction = 0;
unsigned char functionVariable1 = 0;
unsigned char functionVariable2 = 0;

//Create the Variables that are sent/receieved when a request is handled (universal for both slave and master)
unsigned char currentFunction = 0;
unsigned char positionInFunction1 = 0;
unsigned char positionInFunction2 = 0;

//Define the number of LEDs in use (the first number in the function) along with data and clock pins
LPD8806 strip = LPD8806(numLeds, dataPin, clockPin);

void setup()
{
  //start the LED strip
  strip.begin();

  //update the strip to ensure that all the LEDs are all off at the beginning
  strip.show();
  
  if(master == true)
  {
    //Begin I2C communications as a MASTER
    //We call this last to avoid a nasty bug involving the LED initialization code
    Wire.begin();
  }
  else
  {
    //Begin I2C communications as a SLAVE. receiveData will be called when new data arrives
    //We call this last to avoid a nasty bug involving the LED initialization code
    Wire.begin(i2cAddress);
    Wire.onReceive(receiveData);
  }
}

//function prototypes
void rainbowWheel();
void rainbowWheelStrobe(uint32_t onWait, uint32_t offWait);
void scanner(uint8_t r, uint8_t g, uint8_t b, uint32_t wait, boolean bounce);

void loop()
{
  //scanner(127,127,0, 50000, true);
  twinkle(100);
}

void receiveData(int byteCount)
{
  //Check if there are the proper number of bytes
  if (3 == byteCount)
  {
    //Strip off the last byte and read the value
    currentFunction = (0x000000FF & Wire.read()); //What function is currently running
    positionInFunction1 = (0x000000FF & Wire.read()); //Where it is at in the function
    positionInFunction2 = (0x000000FF & Wire.read()); //Where it is at in the function
    
    //Set the flag to say that we have new data
    dataChanged = true;
  }
  
  //If there are too many bytes, they are all removed
  else if (byteCount > 3)
  {
    while (Wire.available() > 0)
    {
      Wire.read();
    }
  }
}

/***************************************************************************************************
                                   Actual light logic functions
                          (setting colors and writing to the LED strips)
***************************************************************************************************/

//Moves a set of LEDs across the strip and bounces them
void scanner(uint8_t r, uint8_t g, uint8_t b, uint32_t wait, boolean bounce)
{
  int h, i, j;

  int pos = 0;
  int dir = 1;

  // Erase the strip initially to be sure that we do not leave
  // LEDs on from previous functions
  for(h=0; h < strip.numPixels(); h++)
  {
    strip.setPixelColor(h, 0);
  }

  for(i=0; i<((strip.numPixels()-1) * 8); i++)
  {
    // Draw 5 pixels centered on pos.  setPixelColor() will clip
    // any pixels off the ends of the strip, no worries there.
    // we'll make the colors dimmer at the edges for a nice pulse
    // look
    strip.setPixelColor(pos - 2, strip.Color(r/4, g/4, b/4));
    strip.setPixelColor(pos - 1, strip.Color(r/2, g/2, b/2));
    strip.setPixelColor(pos, strip.Color(r, g, b));
    strip.setPixelColor(pos + 1, strip.Color(r/2, g/2, b/2));
    strip.setPixelColor(pos + 2, strip.Color(r/4, g/4, b/4));

    strip.show();
    //Wait function with interrupt
    if (true == timedWaitFunction(wait))
    {
      break;
    }

    // If we wanted to be sneaky we could erase just the tail end
    // pixel, but it's much easier just to erase the whole thing
    // and draw a new one next time.
    for(j=-2; j<= 2; j++) 
    strip.setPixelColor(pos+j, strip.Color(0,0,0));
    // Bounce off ends of strip
    pos += dir;
    if(pos < 0)
    {
      pos = 1;
      dir = -dir;
    }
    else if(pos >= strip.numPixels())
    {
      if(bounce == true)
      {
        pos = strip.numPixels() - 2;
        dir = -dir;
      }
      else
      {
        pos = 0;
      } 
    }
  }
}

//Rainbow cycle
void rainbowWheel()
{
  uint16_t i, j;

  for (j=0; j < 384 * 5; j++)
  {     // 5 cycles of all 384 colors in the wheel
    for (i=0; i < strip.numPixels(); i++)
    {
      // tricky math! we use each pixel as a fraction of the full 384-color
      // wheel (thats the i / strip.numPixels() part)
      // Then add in j which makes the colors go around per pixel
      // the % 384 is to make the wheel cycle around
      strip.setPixelColor(i, Wheel(((i * 384 / strip.numPixels()) + j) % 384));
    }
    strip.show();   // write all the pixels out
   
    if (true == timedWaitFunction(50))
    {
      break;
    }
  }
}

void rainbowWheelStrobe(uint32_t onWait, uint32_t offWait)
{
  uint16_t i, j;

  for (j=0; j < 384 * 5; j++)
  {     // 5 cycles of all 384 colors in the wheel
    for (i=0; i < strip.numPixels(); i++)
    {
      // tricky math! we use each pixel as a fraction of the full 384-color
      // wheel (thats the i / strip.numPixels() part)
      // Then add in j which makes the colors go around per pixel
      // the % 384 is to make the wheel cycle around
      strip.setPixelColor(i, Wheel(((i * 384 / strip.numPixels()) + j) % 384));
    }
    strip.show();

    if (true == timedWaitFunction(onWait))
    {
      break;
    }
    
    for(i=0; i < strip.numPixels(); i++)
    {
      strip.setPixelColor(i, strip.Color(0,0,0));
    }
    strip.show();

    if (true == timedWaitFunction(offWait))
    {
      break;  
    }
  }
}

void twinkle(int times)
{
   int numLit = 6;
   
   for (int i = 0; i < times; i++)
   {  
      int pixels[52] = {0};
      randomSeed(micros());
   
      for (int i = 0; i < numLit; i++)
      {
         pixels[random(52)] = 1;
      }

      for (int i=0; i < strip.numPixels(); i++)
      {
         if (pixels[i])
         {
            strip.setPixelColor(i, strip.Color(127, 127, 127));
         }
         else
         {
            strip.setPixelColor(i, strip.Color(0, 0, 0));
         }
      }  
      strip.show();   // write all the pixels out
      if (true == timedWaitFunction(50))
      {
        break;
      }
   }
}

/***************************************************************************************************
                                        Utility functions
                                 Assisting actual light functions
***************************************************************************************************/

/*
Wait function (timed)
This is called when we want to wait in between events that are occuring in our functions.
Much better than using delay() because we can interrupt the parent function when new data is received.
We will sit in this function for the time provided. The input parameter is the amount of milliseconds
long that the wait should be.
*/

//This is the old code for waiting. I'm keeping it here until the new code can be fully tested with I2C.
/*boolean timedWaitFunction(uint32_t waitTime)
{
  for(uint32_t w=0; w <= waitTime; w++)
  {
    if(dataChanged == true)
    {
      return true;
    }
  }
  return false;
}*/

boolean timedWaitFunction(int waitTime)
{
  unsigned long previousMillis = millis();
  unsigned long currentMillis = millis();
  for(previousMillis; (currentMillis - previousMillis) < waitTime; currentMillis = millis())
  {
    if(dataChanged == true)
    {
      return true;
    }
  }
  return false;
}

/*
Wait function (infinite)
This is called when we wait to wait in between events that are occuring in our functions.
Much better than using delay() because we can interrupt the parent function when new data is received.
We sit in this function until dataChanged becomes true.
*/ //SITS IHERE FOREVER (need to check it out)
boolean infiniteWaitFunction()
{
  while(dataChanged == false)
  {
    //Do nothing. We just sit in here until new data is recieved.
  }
  //If we break out of the while loop, then dataChanged must be true so we can return true
  return true;
}

//Input a value 0 to 384 to get a color value.
//The colours are a transition r - g - b - back to r
uint32_t Wheel(uint16_t WheelPos)
{
  byte r, g, b;
  switch(WheelPos / 128)
  {
    case 0:
      r = 127 - WheelPos % 128; // red down
      g = WheelPos % 128;       // green up
      b = 0;                    // blue off
      break;
    case 1:
      g = 127 - WheelPos % 128; // green down
      b = WheelPos % 128;       // blue up
      r = 0;                    // red off
      break;
    case 2:
      b = 127 - WheelPos % 128; // blue down
      r = WheelPos % 128;       // red up
      g = 0;                    // green off
      break;
  }
  return(strip.Color(r,g,b));
}
