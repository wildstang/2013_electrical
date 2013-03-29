package org.wildstang.signs.gui;

import org.wildstang.signs.app.SignTestingApp;
import org.wildstang.signs.data.Sign;

import javax.swing.*;
import java.awt.*;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;

/**
 * User: Steve
 * Date: 3/28/13
 */
public class AppFrame extends JFrame
{
   private SignTestingApp m_app;

   public Sign getSign1()
   {
      return m_sign1;
   }

   public Sign getSign2()
   {
      return m_sign2;
   }

   public Sign getSign3()
   {
      return m_sign3;
   }

   private Sign m_sign1, m_sign2, m_sign3;

   public AppFrame(SignTestingApp p_app, String p_title)
   {
      super(p_title);
      m_app = p_app;
      addWindowListener(new WindowAdapter()
      {
         @Override
         public void windowClosing(WindowEvent e)
         {
            m_app.exit();
         }
      });
   }

   public void init()
   {
      m_sign1 = new Sign(this, 1);

      setSize(600, 600);
      setLayout(new BorderLayout());

      add(m_sign1, BorderLayout.CENTER);

      setVisible(true);
   }

}
