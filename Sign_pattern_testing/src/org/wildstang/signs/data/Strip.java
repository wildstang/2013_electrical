package org.wildstang.signs.data;

import java.awt.*;
import java.util.ArrayList;

/**
 * User: Steve
 * Date: 3/28/13
 */
public class Strip
{
   Panel m_parent;
   public int m_numPixels;
   private ArrayList<Pixel> m_pixels = new ArrayList<Pixel>(52);

   public Strip(Panel p_parent, int p_numPixels)
   {
      m_parent = p_parent;
      m_numPixels = p_numPixels;

      for (int i = 0; i < m_numPixels; i++)
      {
         m_pixels.add(new Pixel());
      }
   }

   public void setPixelColor(int index, int color)
   {
      int green = (color >> 16) & 0xFF;
      int red = (color >> 8) & 0xFF;
      int blue =  (color & 0xFF);

      System.out.println("setPixelColor(): Pixel(" + index + ") Red = " + red + ", green = " + green + ", blue = " + blue);

      Pixel pixel = m_pixels.get(index);
      pixel.setRGB(red, green, blue);
   }

   public int Color(int r, int g, int b)
   {
//      System.out.println("Color(): Red = " + r + ", green = " + g + ", blue = " + b);
      return ((int)(g * 2) << 16) | ((int)(r * 2) << 8) | (int)(b * 2);
   }

   public ArrayList<Pixel> getPixels()
   {
      return m_pixels;
   }

   public int numPixels()
   {
      return m_numPixels;
   }

   public void show()
   {
      m_parent.repaint();
   }
}
