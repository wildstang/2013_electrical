package org.wildstang.signs.app;

import org.wildstang.signs.data.Sign;
import org.wildstang.signs.data.Strip;
import org.wildstang.signs.gui.AppFrame;

/**
 * User: Steve
 * Date: 3/28/13
 */
public class SignTestingApp
{
   AppFrame m_appFrame = null;

   public static void main(String[] args)
   {
      SignTestingApp app = new SignTestingApp();
   }


   public SignTestingApp()
   {
      m_appFrame = new AppFrame(this, "Sign Pattern Testing App");
      m_appFrame.init();
      m_appFrame.setLocationRelativeTo(null);
      m_appFrame.setVisible(true);

      try
      {
         Thread.sleep(1000);
      }
      catch (InterruptedException e)
      {
         e.printStackTrace();  //To change body of catch statement use File | Settings | File Templates.
      }

      Sign sign1 = m_appFrame.getSign1();

      sign1.drawPattern();
   }

   public void exit()
   {
      // Do any shutdown activity here
      m_appFrame.setVisible(false);
      m_appFrame.dispose();
      System.exit(0);
   }


}
