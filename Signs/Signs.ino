/*
WildStang Signs 2013
By: Josh Smith and Steve Garward
*/

#include "LPD8806.h"
#include "SPI.h"
#include <Wire.h>

//The number of LEDs on the sign
#define numLeds 52

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

//Define the number of LEDs in use (the first number in the function). This uses pin 11 for SDA (data) and pin 13
//for SCL (clock)
LPD8806 strip = LPD8806(numLeds);

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
void twinkle(int times, int wait);
void rainbowTwinkle(int times, int wait);
void rainbowFromCenter(uint8_t wait);
void doubleRainbow(uint8_t wait);
void colorChaseTrail(uint8_t red, uint8_t green, uint8_t blue, uint8_t wait, uint8_t trailLength);

void loop()
{
  //scanner(127,127,0, 100, true);
  //rainbowTwinkle(100, 100);
  twinkle(100, 100);
  //rainbowWheelStrobe(20, 20);
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
void scanner(uint8_t r, uint8_t g, uint8_t b, int wait, boolean bounce)
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
/***************************************************************
//START OF TESTING FOR I2C COMMUNICATION BETWEEN SIGNS
***************************************************************/

//Moves a set of LEDs across the strip and bounces them
/*
void scanner(uint8_t r, uint8_t g, uint8_t b, int wait, boolean bounce)
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
}*/

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

void rainbowWheelStrobe(int onWait, int offWait)
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

void twinkle(int times, int wait)
{
   int numLit = 6;
   
   for (int i = 0; i < times; i++)
   {  
      int pixels[numLeds] = {0};
      randomSeed(micros());
   
      for (int i = 0; i < numLit; i++)
      {
         pixels[random(numLeds)] = 1;
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
      if (true == timedWaitFunction(wait))
      {
        break;
      }
   }
}

void rainbowTwinkle(int times, int wait)
{
   int numLit = 15;
   uint16_t j = 0;
   
   for (int i = 0; i < times; i++)
   {  
      int pixels[numLeds] = {0};
      randomSeed(micros());
   
      for (int i = 0; i < numLit; i++)
      {
         pixels[random(numLeds)] = 1;
      }
      
      if (j >= 384)
      {
        j = 0;
      }

      for (int i=0; i < strip.numPixels(); i++)
      {
         if (pixels[i])
         {
           strip.setPixelColor(i, Wheel(((i * 384 / strip.numPixels()) + j) % 384));
         }
         else
         {
           strip.setPixelColor(i, strip.Color(0, 0, 0));
         }
      }  
      strip.show();   // write all the pixels out
      j = j+10;
      if (true == timedWaitFunction(wait))
      {
        break;
      }
   }
}

void blankStrip()
{
  for (int i = 0; i < numLeds; i++)
  {
    strip.setPixelColor(i, 0);
  }
  strip.show();
}

void rainbowFromCenter(uint8_t wait)
{
   int i, j;
   uint32_t color;
   
   int center = numLeds / 2;
   
   blankStrip();

   for (i = (5 * 384) - 1; i >= 0 ; i--)
   {
      // 3 cycles of all 384 colors in the wheel
      for (j=0; j < center; j++)
      {
         color = Wheel( ((j * 384 / center / 2) + i) % 384);   // Wheel( (i + j) % 384);
         strip.setPixelColor(center - 1 - j, color);
         strip.setPixelColor(center + j, color);
         strip.setPixelColor(numLeds - center - 1 - j, color);
         strip.setPixelColor(numLeds - center + j, color);
      }  
   
      strip.show();   // write all the pixels out
      if (true == timedWaitFunction(wait))
      {
        break;
      }
   }
}

void doubleRainbow(uint8_t wait)
{
   blankStrip();
   int i, j;
   uint32_t color;
   
   for (i = (384 * 5) - 1; i >= 0 ; i--)
   {
      // 3 cycles of all 384 colors in the wheel
      for (j=0; j < (numLeds / 2); j++)
      {
         color = Wheel( ((j * 384 / (numLeds / 2)) + i) % 384);   // Wheel( (i + j) % 384);
         strip.setPixelColor(j, color);
         strip.setPixelColor(numLeds - 1 - j, color);
      }  
   
      strip.show();   // write all the pixels out
      if (true == timedWaitFunction(wait))
      {
        break;
      }
   }
}

void colorChaseTrail(uint8_t red, uint8_t green, uint8_t blue, uint8_t wait, uint8_t trailLength)
{
   // Clear the last pattern
   blankStrip();
   
   uint8_t dimRed = red / trailLength;
   uint8_t dimGreen = green / trailLength;
   uint8_t dimBlue = blue / trailLength;
   int currentPixel;

   uint32_t pixels[] = {0};
   uint32_t trailPattern[trailLength + 1];
  
   // Set up the trail pattern

   trailPattern[0] = 0;
   for (int i = 1; i <= trailLength; i++)
   {
      int index = trailLength + 1 - i;
      trailPattern[index] = strip.Color(max(red - (dimRed * (i - 1)), 0), max(green - (dimGreen * (i - 1)), 0), max(blue - (dimBlue * (i - 1)), 0));
   }

   // reset colour array
   for (int i = 0; i < numLeds; i++)
   {
      pixels[i] = 0;
   }

   int lastStart = 0;

   // Fill in colours
   for (int i = 0; i < numLeds; i++)
   {
      lastStart = i - trailLength;

      for (int j = 0; j <= trailLength; j++)
      {
         currentPixel = lastStart + j;
         
         if (currentPixel < 0)
         {
            // Work out position at end
            currentPixel = numLeds + currentPixel;  // subtracts from length to get index
         }
         strip.setPixelColor(currentPixel, trailPattern[j]);
      }

      strip.show();
      if (true == timedWaitFunction(wait))
      {
        break;
      }
   }
}

void colorChase(uint8_t red, uint8_t green, uint8_t blue, uint8_t wait)
{
   // Clear the last pattern
   blankStrip();
   
   for (int i = 0; i < numLeds; i++)
   {
      // Set the pixel colour
      strip.setPixelColor(i, strip.Color(red, green, blue));
      
      // If we are the start of the strip, loop around to the end of the strip
      if (i == 0)
      {
         strip.setPixelColor(numLeds - 1, 0);
      }
      else
      {
         strip.setPixelColor(i - 1, 0);
      }
      
      strip.show();
      if (true == timedWaitFunction(wait))
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

//Remaps the led numbers to leds that form a logical "line"
int pixelMap(int ledNum)
{
  if (ledNum <= 20)
  {
    return ledNum;
  }
  else if (ledNum == 21)
  {
    ledNum = 33;
    return ledNum;
  }
  else if (ledNum >= 21 && ledNum <= 32)
  {
    ledNum--;
    return ledNum;
  }
  else
  {
    ledNum++;
    return ledNum;
  }
}
