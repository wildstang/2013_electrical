#include "LPD8806.h"
#include "SPI.h"

#define NUM_PIXELS_TOTAL 52
#define HALF_LENGTH NUM_PIXELS_TOTAL / 2

// This will use the following pins:
// Data (SDA):  11
// Clock (SCL): 13
LPD8806 strip = LPD8806(NUM_PIXELS_TOTAL);


void setup()
{
  // Start up the LED strip
  strip.begin();

  // Update the strip, to start they are all 'off'
  strip.show();
}


void loop()
{
   colorChase(127, 0, 0, 50);  // Good
   colorChaseTrail(0, 127, 127, 50, 8);  // Good
   doubleRainbow(2);     // Good
//   colorChase(127, 0, 0, 50);  	// full brightness red
//   colorChase(127, 127, 0, 50);	// orange
   colorChase(0, 127 ,0, 20);		// green
//   colorChase(0, 127, 127, 50);	// teal
//   colorChase(0, 0, 127, 50);		// blue
//   colorChase(127, 0, 127, 50);	// violet
   rainbowFromCenter(10);  // Good
   twinkle(100); 
}


/*
 * Sets all pixels off to blank the entire strip.
 */
void blankStrip()
{
   for (int i = 0; i < NUM_PIXELS_TOTAL; i++)
   {
      strip.setPixelColor(i, 0);
   }
   strip.show();
}

void rainbowFromCenter(uint8_t wait)
{
   int i, j;
   uint32_t color;
   
   int center = HALF_LENGTH / 2;
   
   blankStrip();

   for (i = (5 * 384) - 1; i >= 0 ; i--)
   {
      // 3 cycles of all 384 colors in the wheel
      for (j=0; j < center; j++)
      {
         color = Wheel( ((j * 384 / center / 2) + i) % 384);   // Wheel( (i + j) % 384);
         strip.setPixelColor(center - 1 - j, color);
         strip.setPixelColor(center + j, color);
         strip.setPixelColor(NUM_PIXELS_TOTAL - center - 1 - j, color);
         strip.setPixelColor(NUM_PIXELS_TOTAL - center + j, color);
      }  
   
      strip.show();   // write all the pixels out
// TODO: Change to timedWait()
      delay(wait);
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
      for (j=0; j < HALF_LENGTH; j++)
      {
         color = Wheel( ((j * 384 / HALF_LENGTH) + i) % 384);   // Wheel( (i + j) % 384);
         strip.setPixelColor(j, color);
         strip.setPixelColor(NUM_PIXELS_TOTAL - 1 - j, color);
      }  
   
      strip.show();   // write all the pixels out
      delay(wait);
   }
}

/*
 * Chase the colour down the strip.  This leaves a diminishing trail of pixels behind it, the
 * length of which is `
 */
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
   for (int i = 0; i < NUM_PIXELS_TOTAL; i++)
   {
      pixels[i] = 0;
   }

   int lastStart = 0;

   // Fill in colours
   for (int i = 0; i < NUM_PIXELS_TOTAL; i++)
   {
      lastStart = i - trailLength;

      for (int j = 0; j <= trailLength; j++)
      {
         currentPixel = lastStart + j;
         
         if (currentPixel < 0)
         {
            // Work out position at end
            currentPixel = NUM_PIXELS_TOTAL + currentPixel;  // subtracts from length to get index
         }
         strip.setPixelColor(currentPixel, trailPattern[j]);
      }

      strip.show();
      delay(wait);
   }

}

//uint32_t createTrailColor(uint8_t red, uint8_t green, uint8_t blue)
//{
//}

void colorChase(uint8_t red, uint8_t green, uint8_t blue, uint8_t wait)
{
   // Clear the last pattern
   blankStrip();
   
   for (int i = 0; i < NUM_PIXELS_TOTAL; i++)
   {
      // Set the pixel colour
      strip.setPixelColor(i, strip.Color(red, green, blue));
      
      // If we are the start of the strip, loop around to the end of the strip
      if (i == 0)
      {
         strip.setPixelColor(NUM_PIXELS_TOTAL - 1, 0);
      }
      else
      {
         strip.setPixelColor(i - 1, 0);
      }
      
      strip.show();
      delay(wait);
   }
}

void twinkle(int times)
{
   int numLit = 6;
   

   for (int i = 0; i < times; i++)
   {  
      int pixels[NUM_PIXELS_TOTAL] = {0};
      randomSeed(micros());
   
      for (int i = 0; i < numLit; i++)
      {
         pixels[random(NUM_PIXELS_TOTAL)] = 1;
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
      delay(50);
   }
   
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
