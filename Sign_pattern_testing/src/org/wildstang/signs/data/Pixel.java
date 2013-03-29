package org.wildstang.signs.data;

import java.awt.*;
import java.awt.geom.Ellipse2D;

/**
 * User: Steve
 * Date: 3/28/13
 */
public class Pixel
{
   int m_red;
   int m_green;
   int m_blue;

   public Pixel()
   {

   }

   public void setRGB(int red, int green, int blue)
   {
      m_red = red;
      m_green = green;
      m_blue = blue;

//      System.out.println("setRGB(): Red = " + m_red + ", green = " + m_green + ", blue = " + m_blue);

   }


   public void draw(Graphics2D g, int x, int y)
   {
//      System.out.println("Red = " + m_red + ", green = " + m_green + ", blue = " + m_blue);
      Color color = new Color(m_red, m_green, m_blue);

      g.setPaint(color);
      g.fill(new Ellipse2D.Double(x, y, 10, 10));
   }
}
