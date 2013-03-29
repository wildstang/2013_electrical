package org.wildstang.signs.data;

import javax.swing.*;
import java.awt.*;
import java.util.ArrayList;

/**
 * User: Steve
 * Date: 3/28/13
 */
public class Sign extends Panel
{
   private JFrame m_parentFrame;
   private int m_number;
   private int NUM_PIXELS_TOTAL = 52;
   private int HALF_LENGTH = NUM_PIXELS_TOTAL / 2;

   public Strip getStrip()
   {
      return strip;
   }

   private Strip strip;


   public Sign(JFrame p_parent, int p_signNumber)
   {
      m_parentFrame = p_parent;
      m_number = p_signNumber;
      strip = new Strip(this, NUM_PIXELS_TOTAL);
   }

   public void registerPattern(int p_patternNumber, Class p_patternClass)
   {

   }

   public void drawPattern()
   {
//      colorChase();
//      rainbowFromCenter(1000);
        colorFlowDown(0, 0, 127);
  }

   private void colorChase()
   {
      Strip strip = getStrip();

      for (int i=0; i < strip.numPixels(); i++) {
         strip.setPixelColor(i, 0);  // turn all pixels off
      }

      for (int i=0; i < strip.numPixels(); i++) {
         //         strip.setPixelColor(i, c);
         strip.setPixelColor(i, strip.Color(127, 0, 0));
         if (i == 0) {
            strip.setPixelColor(strip.numPixels()-1, 0);
         } else {
            strip.setPixelColor(i-1, 0);
         }
         strip.show();
         timedWait(50);
      }
   }


   void rainbowFromCenter(int wait)
   {
      int i, j;
      int color;

      int center = HALF_LENGTH / 2;

      //blankStrip();

      // Note: Due to the layout of the strip, if we count up with i, it goes towards the center,
      // but if we count down, we go from the center towards the end
      i = 384 - 1;

      // Iterate through all colours
      while(i >= 0)
      {
         // Loop through the pixels, from the center out
         for (j=0; j < center; j++)
         {
            // Get the colour for this position
            color = Wheel( ((j * 384 / center / 2) + i) % 384);

            // Set the pixels on each side of the ceneter, on both strips
            // TODO: Change to use the height, not strip position
            strip.setPixelColor(center - 1 - j, color);
            strip.setPixelColor(center + j, color);
            strip.setPixelColor(NUM_PIXELS_TOTAL - center - 1 - j, color);
            strip.setPixelColor(NUM_PIXELS_TOTAL - center + j, color);
         }

         strip.show();   // write all the pixels out
         // TODO: Change to timedWait()
         timedWait(wait);

         i--;
      }
   }

   int Wheel(int WheelPos)
   {
      int r = 0, g = 0, b = 0;
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


   void setStripColor(int red, int green, int blue)
   {
      int color = strip.Color(red, green, blue);

      for (int i = 0; i < NUM_PIXELS_TOTAL; i++)
      {
         strip.setPixelColor(i, color);
      }
   }

   void solidColor(int red, int green, int blue)
   {
      // Set all pixels to the same colour
      for (int i = 0; i < NUM_PIXELS_TOTAL; i++)
      {
         strip.setPixelColor(i, strip.Color(red, green, blue));
      }

      // Show the pixels
      strip.show();

      timedWait(10);
   }

   void colorFlowDown(int red, int green, int blue)
   {
      //      blankStrip();

      int height;
      int pixels[] = new int[4];

      for (int count = 19; count >= 0; count--)
      {
         for (height = count; height <= 19; height++)
         {
            pixels = heightToPixels(height);
            // Loop through pixels returned
            for (int i = 0; i < 4 && pixels[i] > -1; i++)
            {
               strip.setPixelColor(pixels[i], strip.Color(red, green, blue));
            }
         }
         strip.show();
         // TODO: Change to timedWait()
         timedWait(100);
      }
   }

   int[] heightToPixels(int height)
   {
      int pixels[] = new int[4];
      pixels[0] = pixels[1] = pixels[2] = pixels[3] = -1;

      if (height >= 0 && height < 20)
      {
         pixels[0] = height;
         pixels[1] = (NUM_PIXELS_TOTAL - 1 - height);
      }

      // Special cases
      if (height == 13)
      {
         pixels[2] = 26;
      }
      else if (height == 19)
      {
         pixels[2] = 20;
      }
      else if (height > 13 && height < 19)
      {
         // Top row: max height + first position we care about - height = position
         pixels[2] = 18 + 21 - height;

         // Bottom row: first pixel we care about - max height + height = position
         pixels[3] = 31 - 18 + height;
      }

      return pixels;
   }

   public void timedWait(int wait)
   {
      try
      {
         Thread.sleep(wait);
      }
      catch (InterruptedException e)
      {
         e.printStackTrace();  //To change body of catch statement use File | Settings | File Templates.
      }
   }

   public void paint(Graphics g)
   {
      Graphics2D g2d = (Graphics2D)g;
      ArrayList<Pixel> pixels = strip.getPixels();
      int gap = 20;

      int startX = 40;
      int startY = 500;

      for (int i = 0; i < 20; i++)
      {
         pixels.get(i).draw(g2d, startX + (7 * gap), startY - (i * gap));
      }

      for (int i = 51; i > 31; i--)
      {
         pixels.get(i).draw(g2d, startX + (6 * gap), startY - ((51 - i) * gap));
      }

      for (int i = 0; i < 6; i++)
      {
         pixels.get(26 + i).draw(g2d, startX + (i * gap), startY - ((12 + i) * gap));
         pixels.get(26 - 1 - i).draw(g2d, startX + (i * gap), startY - ((13 + i) * gap));
      }
   }


}
