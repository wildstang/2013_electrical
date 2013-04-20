/*
Wildstang Lights 2013
By: Josh Smith and Steve Garward
*/

//Since the cRIO operates at 38.4kHz for its I2C clock, we need to set this manually here.
//This accesses the low level TWI library files found in /Wire/utility/
//This needs to be defined before we include as there is a check for a define in twi.c
#define TWI_FREQ 38400L

#include <LPD8806.h>
#include "SPI.h"
#include <Wire.h>
#include "definitions.h"

//#define WS_DEBUG

// This will use the following pins:
// Data (SDA):  11
// Clock (SCL): 13
LPD8806 strip = LPD8806(STRIP_LENGTH);

uint32_t trailPattern[TRAIL_LENGTH + 1];

//This the address to use for the arduino in slave mode when talking to the cRIO
//This number may differ from the address that the cRIO expects. Sniff the I2C lines
//to check what address correlates to the actual integer given here
static const byte i2cAddress = 82;

//This is the number of the digital IO pin that will be pulled to 5V to signify that
//the sound board is connected
static const byte soundBoardCheck = 7;

//This boolean gets set when we have new data that has been verified to be correct following our checks
boolean dataChanged = false;

//Create the variables that are used for raw data packets
//These should not be altered as they are handled internally by the recieveData function
unsigned char dataByte1 = 0;
unsigned char dataByte2 = 0;
unsigned char dataByte3 = 0;
unsigned char dataByte4 = 0;
unsigned char dataByte5 = 0;

//Create the variables that are actually used within the light functions.
//These are the actual variables that should be compared with but they should not be manually altered
unsigned char commandByte  = 0;
unsigned char payloadByte1 = 0;
unsigned char payloadByte2 = 0;

/****************************** Sound Board Code ******************************/
static const byte PIXELS_PER_SEGMENT = 2; //How many LEDs we want to use for each segment of the "VU meter"
boolean IS_SOUND_BOARD_IN = false; //Leave this alone as it will be automatically set to true if you are using our sound board
boolean DUAL_SIDES = true; //Do you want both sides of the LED strip to have the meter or just one?
float SENSITIVITY = 2.3; //This sets the sensitivity of the mic (increase for quiet environment (3.0 should be upper bound))
static const byte analogPin = 0; // read from multiplexer using analog input 0
static const byte strobePin = 3; // strobe is attached to digital pin 3
static const byte resetPin = 6; // reset is attached to digital pin 6

int baseline[7];
int spectrumValue[7]; // to hold a2d values

boolean initialise = false;
int totalVolume = 0;
int relativeLevel = 0;
int averageVolume = 0;
/******************************************************************************/

//This is used for the initial 30 second wait period where our robot would calibrate the gyro and could not be touched.
//Just switch this to "false" if you do not want this initial wait period.
boolean firstRun = true;

byte loops = 0;

/****************************** PICK YOUR ALLIANCE ******************************/
/*                             (uncomment alliance)                             */

#define RED_ALLIANCE
//#define BLUE_ALLIANCE

/******************************************************************************/

void setup()
{
  //This reads the noise off of Analog input 2 and seeds the random() function
  randomSeed(analogRead(2));
  //Sound Board Setup
  byte state = 0;
  pinMode(soundBoardCheck, INPUT); //Set the soundBoardCheck value as our input pin
  state = digitalRead(soundBoardCheck); //Get the state (is the board in?)
  
  //Although this check is check is reduntant, it allows us to 
  if (state == 0)
  {
    IS_SOUND_BOARD_IN = false;
  }
  else if (state == 1)
  {
    IS_SOUND_BOARD_IN = true;
  }
  
  if (IS_SOUND_BOARD_IN == true)
  {
    
    pinMode(analogPin, INPUT);
    pinMode(strobePin, OUTPUT);
    pinMode(resetPin, OUTPUT);
    analogReference(DEFAULT);

    digitalWrite(resetPin, LOW);
    digitalWrite(strobePin, HIGH);
    
    //Normally we can't use delay but during setup, it doesn't matter since I2C
    delay(5000);
    // Initialise and get a baseline
    digitalWrite(resetPin, HIGH);
    digitalWrite(resetPin, LOW);

    for (byte i = 0; i < 7; i++)
    {
       baseline[i] = 0;
    }

    for (byte j = 0; j < 10; j++)
    {
       for (byte i = 0; i < 7; i++)
       {
          digitalWrite(strobePin, LOW);
          delayMicroseconds(30);  // to allow the output to settle
          baseline[i] += analogRead(analogPin);
          digitalWrite(strobePin, HIGH);
       }
    }

    for (byte i = 0; i < 7; i++)
    {
       // Average the baseline readings
       baseline[i] = baseline[i] / 10;
    }
  }

  initShootingTrailPattern(0, 0, 127);

  //start the LED strip
  strip.begin();

  //update the strip to ensure that all the LEDs are all off at the beginning
  strip.show();
  
  //Begin I2C communications as a SLAVE. receiveData will be called when new data arrives
  //We call this last to avoid a nasty bug involving the LED initialization code
  Wire.begin(i2cAddress);
  Wire.onReceive(receiveData);
}



void loop()
{
//  colorChase(strip.Color(0,127,127), 50);
//
//   testArrows();
//   
////   for (int i = 0; i < 3; i++)
////   {
//      shoot();
//      timedWait(1000);
////   }
//   
////   twinkle(30);
//   setArrow1Colour(127, 0, 0);
//   setArrow2Colour(0, 127, 0);
//   setArrow3Colour(0, 0, 127);
//   setArrow4Colour(0, 127, 127);
//   strip.show();
//   timedWait(2000);
//   colorFlowDown(0, 0, 127);
//   colourFlowDownShimmer(0, 0, 127);
//   colorFill(0, 0, 127);
//   timedWait(3000);
//   arrowRainbow();

#ifdef RED_ALLIANCE   
   if (firstRun == true)
   {
      gyroCalibrate(5, 50, 120, 127, 0, 0);
      for(loops = 0; loops < 20; loops++)
      {
         faderRed(30);
      }
      firstRun = false;
   }
   for(loops = 0; loops < 2; loops++)
   {
      rainbowWheel(3);
   }
   for(loops = 0; loops < 5; loops++)
   {
      scanner(127, 0, 0, 10, true);
   }
   alternatingColor(127, 0, 0, 127, 0, 0, 150, 150, 50);
   for(loops = 0; loops < 3; loops++)
   {
      testArrows();
   }
   for(loops = 0; loops < 4; loops++)
   {
      faderRed(30);
   }
   alternatingColor(127, 0, 0, 127, 0, 0, 150, 150, 50);
#endif

#ifdef BLUE_ALLIANCE   
   if (firstRun == true)
   {
      gyroCalibrate(5, 50, 120, 0, 0, 127);
      for(loops = 0; loops < 20; loops++)
      {
         faderBlue(30);
      }
      firstRun = false;
   }
   for(loops = 0; loops < 2; loops++)
   {
      rainbowWheel(3);
   }
   for(loops = 0; loops < 5; loops++)
   {
      scanner(0, 0, 127, 10, true);
   }
   alternatingColor(0, 0, 127, 0, 0, 127, 150, 150, 50);
   for(loops = 0; loops < 3; loops++)
   {
      testArrows();
   }
   for(loops = 0; loops < 4; loops++)
   {
      faderBlue(30);
   }
   alternatingColor(0, 0, 127, 0, 0, 127, 150, 150, 50);
#endif
//   scanner(127, 0, 0, 20, true);
}

void colorChase(uint32_t c, uint8_t wait)
{
  int i;
  
  for (i=0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, 0);  // turn all pixels off
  } 
  
  for (i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      if (i == 0) { 
        strip.setPixelColor(strip.numPixels()-1, 0);
      } else {
        strip.setPixelColor(i-1, 0);
      }
      strip.show();
      timedWait(wait);
  }
}

void blankStrip()
{
   for (int i = 0; i < strip.numPixels(); i++)
   {
      strip.setPixelColor(i, 0);
   }
   strip.show();
}

//  Sets all pixels off to blank the entire strip.
void blankRange(int p_start, int p_end)
{
   for (int i = p_start; i < p_end; i++)
   {
      strip.setPixelColor(i, 0);
   }
   strip.show();
}

void testArrows()
{
   uint32_t color;
 
   blankStrip();
  
   for (byte i = 0; i < 3; i++)
   {
      // pick the color
      switch (i)
      {
         case 0: color = strip.Color(127, 0, 0);
         break;
         case 1: color = strip.Color(127, 40, 0);
         break;
         case 2: color = strip.Color(0, 127, 0);
         break;
      }
      
      // Now light up each arrow in turn
      for (byte arrow = 1; arrow <= 4; arrow++)
      {
         for (int pixel = 0; pixel < arrow * ARROW_LENGTH; pixel++)
         {
            strip.setPixelColor(ARROWS_START + pixel, color);
         }
         strip.show();
         timedWait(100);
      }
   }
   
   timedWait(1000);

   for (int j = 0; j < 384 * 5; j++)
   {     // 5 cycles of all 384 colors in the wheel
      for (int i = 0; i < ARROW_STRIP_LENGTH; i++)
      {
         // tricky math! we use each pixel as a fraction of the full 384-color wheel
         // (thats the i / strip.numPixels() part)
         // Then add in j which makes the colors go around per pixel
         // the % 384 is to make the wheel cycle around
         strip.setPixelColor(ARROWS_START + i, Wheel( ((i * 384 / ARROW_STRIP_LENGTH) + j) % 384) );
      }
      strip.show();   // write all the pixels out
      timedWait(0);
   }
}


void arrowRainbow()
{
   for (int j = 0; j < 384 * 5; j++)
   {     // 5 cycles of all 384 colors in the wheel
      setArrow1Colour(Wheel( ((0 * 384 / ARROW_STRIP_LENGTH) + j) % 384) );
      setArrow2Colour(Wheel( ((1 * 384 / ARROW_STRIP_LENGTH) + j) % 384) );
      setArrow3Colour(Wheel( ((2 * 384 / ARROW_STRIP_LENGTH) + j) % 384) );
      setArrow4Colour(Wheel( ((3 * 384 / ARROW_STRIP_LENGTH) + j) % 384) );
         
      strip.show();   // write all the pixels out
      timedWait(20);
   }

}

void setArrow1Colour(uint8_t red, uint8_t green, uint8_t blue)
{
   setArrow1Colour(strip.Color(red, green, blue));
}

void setArrow2Colour(uint8_t red, uint8_t green, uint8_t blue)
{
   setArrow2Colour(strip.Color(red, green, blue));
}

void setArrow3Colour(uint8_t red, uint8_t green, uint8_t blue)
{
   setArrow3Colour(strip.Color(red, green, blue));
}

void setArrow4Colour(uint8_t red, uint8_t green, uint8_t blue)
{
   setArrow4Colour(strip.Color(red, green, blue));
}

void setArrow1Colour(uint32_t color)
{
   for (uint8_t i = 0; i < ARROW_LENGTH; i++)
   {
      strip.setPixelColor(ARROW_1_START + i, color);
   }
}

void setArrow2Colour(uint32_t color)
{
   for (uint8_t i = 0; i < ARROW_LENGTH; i++)
   {
      strip.setPixelColor(ARROW_2_START + i, color);
   }
}

void setArrow3Colour(uint32_t color)
{
   for (uint8_t i = 0; i < ARROW_LENGTH; i++)
   {
      strip.setPixelColor(ARROW_3_START + i, color);
   }
}

void setArrow4Colour(uint32_t color)
{
   for (uint8_t i = 0; i < ARROW_LENGTH; i++)
   {
      strip.setPixelColor(ARROW_4_START + i, color);
   }
}

void initShootingTrailPattern(uint8_t red, uint8_t green, uint8_t blue)
{
   uint8_t dimRed = red / TRAIL_LENGTH;
   uint8_t dimGreen = green / TRAIL_LENGTH;
   uint8_t dimBlue = blue / TRAIL_LENGTH;

   // Set up the trail pattern
   trailPattern[0] = 0;
   for (int i = 1; i <= TRAIL_LENGTH; i++)
   {
      int index = TRAIL_LENGTH + 1 - i;
      trailPattern[index] = strip.Color(max(red - (dimRed * (i - 1)), 0), max(green - (dimGreen * (i - 1)), 0), max(blue - (dimBlue * (i - 1)), 0));
   }

}

void shoot()
{

   int currentPixel;
   
   blankRange(SHOOT_TRAIL_START, STRIP_LENGTH - SHOOT_TRAIL_START);

//      // reset colour array
//   for (int i = SHOOT_TRAIL_START; i <= SHOOT_TRAIL_END; i++)
//   {
//      pixels[i] = 0;
//   }

   int lastStart = 0;

   // Fill in colours
   for (int i = SHOOT_TRAIL_START; i <= SHOOT_TRAIL_END; i++)
   {
      lastStart = i - TRAIL_LENGTH;

      for (int j = 0; j <= TRAIL_LENGTH; j++)
      {
         currentPixel = lastStart + j;
         
         if (currentPixel < 0)
         {
            // Work out position at end
            currentPixel = SHOOT_TRAIL_END + 1 + currentPixel;  // subtracts from length to get index
         }
         strip.setPixelColor(currentPixel, trailPattern[j]);
         strip.setPixelColor(STRIP_LENGTH - 1 - currentPixel, trailPattern[j]);
      }

      if ((i - SHOOT_TRAIL_START) >= ARROW_TRIGGER_1)
      {
         setArrow1Colour(0, 127, 0);
      }
      if ((i - SHOOT_TRAIL_START) >= ARROW_TRIGGER_2)
      {
         setArrow2Colour(0, 127, 0);
      }
      if ((i - SHOOT_TRAIL_START) >= ARROW_TRIGGER_3)
      {
         setArrow3Colour(0, 127, 0);
      }
      if ((i - SHOOT_TRAIL_START) >= ARROW_TRIGGER_4)
      {
         setArrow4Colour(0, 127, 0);
      }

      strip.show();
// TODO: Change to timedWait()
      timedWait(10);

   }
}


void twinkle(uint8_t times, uint8_t numLit)
{
   for (uint8_t i = 0; i < times; i++)
   {  
      int pixels[STRIP_LENGTH] = {0};
      randomSeed(micros());
   
      for (uint8_t i = 0; i < numLit; i++)
      {
         pixels[random(STRIP_LENGTH)] = 1;
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
      timedWait(50);
   }
   
}

void twinkle(uint8_t times, uint8_t numLit, uint8_t bgred, uint8_t bggreen, uint8_t bgblue, uint8_t fgred, uint8_t fggreen, uint8_t fgblue, int wait)
{  
   for (uint8_t i = 0; i < times; i++)
   {  
      int pixels[STRIP_LENGTH] = {0};
      randomSeed(micros());
   
      for (uint8_t i = 0; i < numLit; i++)
      {
         pixels[random(STRIP_LENGTH)] = 1;
      }

      for (int i=0; i < strip.numPixels(); i++)
      {
         if (pixels[i])
         {
            strip.setPixelColor(i, strip.Color(fgred, fggreen, fgblue));
         }
         else
         {
            strip.setPixelColor(i, strip.Color(bgred, bggreen, bgblue));
         }
      }  
      strip.show();   // write all the pixels out
      timedWait(wait);
   }
   
}


void colourFlowDownShimmer(uint8_t red, uint8_t green, uint8_t blue)
{
   blankStrip();
   
   int height;
   int pixels[4];
   int shimmerRow1[4];
   int shimmerRow2[4];
   
   for (int count = HALF_STRIP_LENGTH; count >= 0; count--)
   {
      for (height = count; height <= HALF_STRIP_LENGTH; height++)
      {
         // Loop through pixels returned
         strip.setPixelColor(height, strip.Color(red, green, blue));
         strip.setPixelColor(STRIP_LENGTH - 1 - height, strip.Color(red, green, blue));

         strip.show();

         if (height == count || height == count + 1)
         {
   
            for (byte j = 1; j < 3; j++)
            {
               if (random(6) > 3)
               {
                  strip.setPixelColor(count, strip.Color(127, 127, 127));
                  strip.setPixelColor(STRIP_LENGTH - 1 - count, strip.Color(127, 127, 127));

                  strip.setPixelColor(count + 1, strip.Color(0, 0, 0));
                  strip.setPixelColor(STRIP_LENGTH - 1 - (count + 1), strip.Color(0, 0, 0));
               }
               else
               {
                  strip.setPixelColor(count, strip.Color(0, 0, 0));
                  strip.setPixelColor(STRIP_LENGTH - 1 - count, strip.Color(0, 0, 0));

                  strip.setPixelColor(count + 1, strip.Color(127, 127, 127));
                  strip.setPixelColor(STRIP_LENGTH - 1 - (count + 1), strip.Color(127, 127, 127));
               }            
            strip.show();
            }
//            timedWait(5);
         }
      }
   //// TODO: Change to timedWait()
//   timedWait(500);
   }

}
   

void colorFlowDown(uint8_t red, uint8_t green, uint8_t blue)
{
   blankStrip();
   
   int height;
 
   for (int count = HALF_STRIP_LENGTH; count >= 0; count--)
   {
         // Loop through pixels returned
         strip.setPixelColor(count, strip.Color(red, green, blue));
         strip.setPixelColor(STRIP_LENGTH - 1 - count, strip.Color(red, green, blue));

         strip.show();
         timedWait(20);
   }

}
   

void colorFill(uint8_t red, uint8_t green, uint8_t blue)
{
   for (int i = 0; i < STRIP_LENGTH; i++)
   {
      strip.setPixelColor(i, strip.Color(red, green, blue));
   }
   
   strip.show();
}


uint32_t Wheel(uint16_t WheelPos)
{
  byte r, g, b;
  switch(WheelPos / 128)
  {
    case 0:
      r = 127 - WheelPos % 128;   //Red down
      g = WheelPos % 128;      // Green up
      b = 0;                  //blue off
      break; 
    case 1:
      g = 127 - WheelPos % 128;  //green down
      b = WheelPos % 128;      //blue up
      r = 0;                  //red off
      break; 
    case 2:
      b = 127 - WheelPos % 128;  //blue down 
      r = WheelPos % 128;      //red up
      g = 0;                  //green off
      break; 
  }
  return(strip.Color(r,g,b));
}

void receiveData(int byteCount)
{
  //Check the byte count to ensure that we are recieving a 5 byte packet
  if (5 == byteCount)
  {
    //Strip off the last byte and read the value
    dataByte1 = (0x000000FF & Wire.read()); //Command Byte
    dataByte2 = (0x000000FF & Wire.read()); //Payload Data
    dataByte3 = (0x000000FF & Wire.read()); //Payload Data
    dataByte4 = (0x000000FF & Wire.read()); //Flipped Version of byte 2
    dataByte5 = (0x000000FF & Wire.read()); //Flipped version of byte 3
    
    //Check if the payload bytes and the flipped counterparts are indeed opposite using XOR
    //If so, then set the bytes we actually use in loop()
    if ((0xFF == (dataByte2 ^ dataByte4)) &&
        (0xFF == (dataByte3 ^ dataByte5)))
    {
      //Check to see if the new data is the same as the old data after verifying that it is correct
      if((commandByte != dataByte1) || (payloadByte1 != dataByte2) || (payloadByte2 != dataByte3))
      {
       //Finally set the data to the variables we actually use in loop() 
       commandByte = dataByte1;
       payloadByte1 = dataByte2;
       payloadByte2 = dataByte3;
       
       //Set the flag to say that we have new data
       dataChanged = true;
      }
    }    
  }
  //The following logic needs to be checked because if too many packets are recieved at once, it may continue
  //"deleting" them
  else if (byteCount > 5) //This should clear out any packets that are bigger than the required 5 bytes
  {
    //Keep on reading the bytes from the buffer until they are gone. They are simply not used and thus
    //will be thrown away.
    while (Wire.available() > 0)
    {
      Wire.read();
    }
  }
}

boolean timedWait(unsigned int waitTime)
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
*/
boolean infiniteWaitFunction()
{
  while(dataChanged == false)
  {
    //Do nothing. We just sit in here until new data is recieved.
  }
  //If we break out of the while loop, then dataChanged must be true so we can return true
  return true;
}

//this is a simple fader that goes on and off (RED)
void faderRed(uint32_t wait)
{
   uint8_t i;
   int p;
   for(i=0; i <= 120; i+=5)
   {
      for(p=0; p < strip.numPixels(); p++)
      {
         strip.setPixelColor(p, strip.Color(i,0,0));
      }
      strip.show();

      if (true == timedWait(wait))
      {
         break;
      }
   }
   for(i; i >= 0; i-=5)
   {
      for(p=0; p < strip.numPixels(); p++)
      {
         strip.setPixelColor(p, strip.Color(i,0,0));
      }
      strip.show();

      if (true == timedWait(wait))
      {
         break;
      }
   }
}

//this is a simple fader that goes on and off (BLUE)
void faderBlue(uint32_t wait)
{
   uint8_t i;
   int p;
   for(i=0; i <= 120; i+=5)
   {
      for(p=0; p < strip.numPixels(); p++)
      {
         strip.setPixelColor(p, strip.Color(0,0,i));
      }
      strip.show();

      if (true == timedWait(wait))
      {
         break;
      }

   }
   for(i; i >= 0; i-=5)
   {
      for(p=0; p < strip.numPixels(); p++)
      {
         strip.setPixelColor(p, strip.Color(0,0,i));
      }
      strip.show();

      if (true == timedWait(wait))
      {
         break;
      }
   }
}

//Rainbow cycle
void rainbowWheel(int wait)
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
   
    if (true == timedWait(wait))
    {
      break;
    }
  }
}

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
    if (true == timedWait(wait))
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

//this blinks all the LEDs to the rgb values passed in. Time can be changed for on and off with onWait and offWait when calling.
void colorBlink(uint32_t onWait, uint32_t offWait, uint8_t r, uint8_t g, uint8_t b)
{
  int i;
  //First Pulse
  for(i=0; i < strip.numPixels(); i++)
  {
    strip.setPixelColor(i, strip.Color(r,g,b));
  }
  strip.show();
  
  if (true == timedWait(onWait))
  {
     return;
  }

  for(i=0; i < strip.numPixels(); i++)
  {
    strip.setPixelColor(i, 0);
  }
  strip.show();
  //Wait function with interrupt
  if (true == timedWait(offWait))
  {
     return;  
  }
}

void gyroCalibrate(uint8_t flashes, int blinkTime, int blinkTime2, int red, int green, int blue)
{
   int pixel, h;
   uint8_t q;
   // Erase the strip initially to be sure that we do not leave
   // LEDs on from previous functions
   for(h=0; h < strip.numPixels(); h++)
   {
     strip.setPixelColor(h, 0);
   }
   strip.show();
   timedWait(2000);
   strip.setPixelColor(0, strip.Color(127, 0, 0));
   strip.setPixelColor(1, strip.Color(127, 0, 0));
   strip.setPixelColor(83, strip.Color(127, 0, 0));
   strip.setPixelColor(82, strip.Color(127, 0, 0));
   for (pixel = 0; pixel < 1 * ARROW_LENGTH; pixel++)
   {
      strip.setPixelColor(ARROWS_START + pixel, strip.Color(red, green, blue));
   }
   strip.show();
   timedWait(1000);
   strip.setPixelColor(2, strip.Color(127, 127, 0));
   strip.setPixelColor(3, strip.Color(127, 127, 0));
   strip.setPixelColor(81, strip.Color(127, 127, 0));
   strip.setPixelColor(80, strip.Color(127, 127, 0));
   strip.show();
   timedWait(1000);
   strip.setPixelColor(4, strip.Color(0, 127, 0));
   strip.setPixelColor(5, strip.Color(0, 127, 0));
   strip.setPixelColor(79, strip.Color(0, 127, 0));
   strip.setPixelColor(78, strip.Color(0, 127, 0));
   for (pixel = 0; pixel < 2 * ARROW_LENGTH; pixel++)
   {
      strip.setPixelColor(ARROWS_START + pixel, strip.Color(red, green, blue));
   }
   strip.show();
   timedWait(1000);
   strip.setPixelColor(6, strip.Color(0, 127, 127));
   strip.setPixelColor(7, strip.Color(0, 127, 127));
   strip.setPixelColor(77, strip.Color(0, 127, 127));
   strip.setPixelColor(76, strip.Color(0, 127, 127));
   strip.show();
   timedWait(1000);
   strip.setPixelColor(8, strip.Color(0, 0, 127));
   strip.setPixelColor(9, strip.Color(0, 0, 127));
   strip.setPixelColor(75, strip.Color(0, 0, 127));
   strip.setPixelColor(74, strip.Color(0, 0, 127));
   for (pixel = 0; pixel < 3 * ARROW_LENGTH; pixel++)
   {
      strip.setPixelColor(ARROWS_START + pixel, strip.Color(red, green, blue));
   }
   strip.show();
   timedWait(1000);
   strip.setPixelColor(10, strip.Color(127, 0, 127));
   strip.setPixelColor(11, strip.Color(127, 0, 127));
   strip.setPixelColor(73, strip.Color(127, 0, 127));
   strip.setPixelColor(72, strip.Color(127, 0, 127));
   strip.show();
   timedWait(1000);
   strip.setPixelColor(12, strip.Color(127, 127, 127));
   strip.setPixelColor(13, strip.Color(127, 127, 127));
   strip.setPixelColor(71, strip.Color(127, 127, 127));
   strip.setPixelColor(70, strip.Color(127, 127, 127));
   for (pixel = 0; pixel < 4 * ARROW_LENGTH; pixel++)
   {
      strip.setPixelColor(ARROWS_START + pixel, strip.Color(red, green, blue));
   }
   strip.show();
   timedWait(1000);
   for(uint8_t i = 0; i < flashes; i++)
   {
      for(q = 0; q < 14; q++)
      {
         strip.setPixelColor(q, strip.Color(red, green, blue));
      }
      for(q = 83; q > 69; q--)
      {
         strip.setPixelColor(q, strip.Color(red, green, blue));
      }
      for (pixel = 0; pixel < 4 * ARROW_LENGTH; pixel++)
      {
         strip.setPixelColor(ARROWS_START + pixel, strip.Color(red, green, blue));
      }
      strip.show();
      timedWait(blinkTime);
      for(q = 0; q < 14; q++)
      {
         strip.setPixelColor(q, 0);
      }
      for(q = 83; q > 69; q--)
      {
         strip.setPixelColor(q, 0);
      }
      for (pixel = 0; pixel < 4 * ARROW_LENGTH; pixel++)
      {
         strip.setPixelColor(ARROWS_START + pixel, 0);
      }
      strip.show();
      timedWait(blinkTime2);
   }
}

void alternatingColor(int red1, int green1, int blue1, int red2, int green2, int blue2, int wait1, int wait2, int times)
{
   int h, i;
   for(h=0; h < strip.numPixels(); h++)
   {
     strip.setPixelColor(h, 0);
   }
   for(i=0; i < times; i++)
   {
      for(h=0; h < strip.numPixels(); h++)
      {
        strip.setPixelColor(h, 0);
      }
      for(h=0; h < strip.numPixels(); h=h+2)
      {
        strip.setPixelColor(h, strip.Color(red1, green1, blue1));
      }
      strip.show();
      timedWait(wait1);
      for(h=0; h < strip.numPixels(); h++)
      {
        strip.setPixelColor(h, 0);
      }
      for(h=1; h < strip.numPixels(); h=h+2)
      {
        strip.setPixelColor(h, strip.Color(red2, green2, blue2));
      }
      strip.show();
      timedWait(wait2);
   }
}
