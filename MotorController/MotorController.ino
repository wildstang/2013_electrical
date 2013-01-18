//Two Victor Variable Speed Controller

#include <Servo.h>
#include <LiquidCrystal.h>

int VictorPin1 = 9;
int VictorPin2 = 10;
int VictorPot1 = A0;
int VictorPot2 = A1;
int PotValue1 = 0;
int PotValue2 = 0;
int Rpm1 = 0;
int Rpm2 = 0;

//Set this to the maximum RPM that the motor can run at
int MaxRpm = 7000;
//Put up to 16 characters here of the name of the motor you are testing.
char MotorName[16] = "CIM MOTOR";

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void setup()
{
  pinMode(VictorPin1, OUTPUT);
  pinMode(VictorPin2, OUTPUT);
  
  lcd.begin(16, 2);
  lcd.clear();
  
  lcd.print("Wildstang Motor");
  lcd.setCursor(0, 1);
  lcd.print("Controller");
  
  delay(4000);
  lcd.clear();
  
  lcd.setCursor(0, 0);
  lcd.print("    Running:");
  lcd.setCursor(0, 1);
  lcd.print(MotorName);
  
  delay(4000);
  lcd.clear();
  
  lcd.setCursor(0, 0);
  lcd.print("Victor1: 0000RPM");
  lcd.setCursor(0, 1);
  lcd.print("Victor2: 0000RPM");
}

void loop()
{
  PotValue1 = analogRead(VictorPot1);
  PotValue2 = analogRead(VictorPot2);
  
  PotValue1 = map(PotValue1, 0, 1023, 0, 255);
  PotValue2 = map(PotValue2, 0, 1023, 0, 255);
  
  Rpm1 = map(PotValue1, 0, 255, (-1*MaxRpm), MaxRpm);
  Rpm2 = map(PotValue2, 0, 255, (-1*MaxRpm), MaxRpm);
  
  /*
  if(Rpm1 < 0)
  {
    lcd.setCursor(8, 0);
    lcd.print("-");
  }
  else
  {
    lcd.setCursor(8, 0);
    lcd.print(" ");
  }
  
  if(Rpm2 < 0)
  {
    lcd.setCursor(8, 1);
    lcd.print("-");
  }
  else
  {
    lcd.setCursor(8, 1);
    lcd.print(" ");
  }
  */
  
  if(Rpm1 < 0)
  {
    if(Rpm1 < -999)
    {
      lcd.setCursor(8, 0);
      lcd.print(Rpm1);
    }
    else if(Rpm1 < -99)
    {
      lcd.setCursor(8, 0);
      lcd.print(" ");
      lcd.setCursor(9, 0);
      lcd.print(Rpm1);
    }
    else if(Rpm1 < -9)
    {
      lcd.setCursor(8, 0);
      lcd.print("  ");
      lcd.setCursor(10, 0);
      lcd.print(Rpm1);
    }
    else
    {
      lcd.setCursor(8, 0);
      lcd.print("   ");
      lcd.setCursor(11, 0);
      lcd.print(Rpm1);
    }
  }
  else
  {
    if(Rpm1 > 999)
    {
      lcd.setCursor(9, 0);
      lcd.print(Rpm1);
    }
    else if(Rpm1 > 99)
    {
      lcd.setCursor(9, 0);
      lcd.print(" ");
      lcd.setCursor(10, 0);
      lcd.print(Rpm1);
    }
    else if(Rpm1 > 9)
    {
      lcd.setCursor(9, 0);
      lcd.print("  ");
      lcd.setCursor(11, 0);
      lcd.print(Rpm1);
    }
    else
    {
      lcd.setCursor(9, 0);
      lcd.print("   ");
      lcd.setCursor(12, 0);
      lcd.print(Rpm1);
    }
  }
  
  if(Rpm2 < 0)
  {
    if(Rpm2 < -999)
    {
      lcd.setCursor(8, 1);
      lcd.print(Rpm2);
    }
    else if(Rpm2 < -99)
    {
      lcd.setCursor(8, 1);
      lcd.print(" ");
      lcd.setCursor(9, 1);
      lcd.print(Rpm2);
    }
    else if(Rpm2 < -9)
    {
      lcd.setCursor(8, 1);
      lcd.print("  ");
      lcd.setCursor(10, 1);
      lcd.print(Rpm2);
    }
    else
    {
      lcd.setCursor(8, 1);
      lcd.print("   ");
      lcd.setCursor(11, 1);
      lcd.print(Rpm2);
    }
  }
  else
  {
    if(Rpm2 > 999)
    {
      lcd.setCursor(9, 1);
      lcd.print(Rpm2);
    }
    else if(Rpm2 > 99)
    {
      lcd.setCursor(9, 1);
      lcd.print(" ");
      lcd.setCursor(10, 1);
      lcd.print(Rpm2);
    }
    else if(Rpm2 > 9)
    {
      lcd.setCursor(9, 1);
      lcd.print("  ");
      lcd.setCursor(11, 1);
      lcd.print(Rpm2);
    }
    else
    {
      lcd.setCursor(8, 1);
      lcd.print("   ");
      lcd.setCursor(12, 1);
      lcd.print(Rpm2);
    }
  }
  
  analogWrite(VictorPin1, PotValue1);
  analogWrite(VictorPin2, PotValue2);
}

