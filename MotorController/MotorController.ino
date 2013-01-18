//Two Victor Variable Speed Controller

#include <Servo.h>
#include <LiquidCrystal.h>

//These are the various pins connected to the victors and the pots
int VictorPin1 = 9;
int VictorPin2 = 10;
int VictorPot1 = A0;
int VictorPot2 = A1;

//Leave these variables alone. These are set every time through loop
int PotValue1 = 0;
int PotValue2 = 0;
int Rpm1 = 0;
int Rpm2 = 0;

//Set this to the maximum RPM that the motor can run at
int MaxRpm = 7000;
//Put up to 16 characters here of the name of the motor you are testing.
char MotorName[16] = "CIM MOTOR";

//Set the pins to be used for the LCD display
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void setup()
{
  //Set our pins to output to send the pwm signal to the victors
  pinMode(VictorPin1, OUTPUT);
  pinMode(VictorPin2, OUTPUT);
  
  //Initialize the LCD
  lcd.begin(16, 2);
  lcd.clear();
  
  //Starting up screens
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
  
  //We start with a "zeroed out" display to make things a bit easier.
  //The RPM section is simply rewritten as needed while leaving the static bits.
  lcd.setCursor(0, 0);
  lcd.print("Victor1: 0000RPM");
  lcd.setCursor(0, 1);
  lcd.print("Victor2: 0000RPM");
}

void loop()
{
  //Grab the values from the pots
  PotValue1 = analogRead(VictorPot1);
  PotValue2 = analogRead(VictorPot2);
  
  //Map these values to a new range of 0-255. This is what the victors expect.
  PotValue1 = map(PotValue1, 0, 1023, 0, 255);
  PotValue2 = map(PotValue2, 0, 1023, 0, 255);
  
  //For displaying the values, we want the max rpm to be shown instead of our 255 range.
  //This will handle both positive and negative values (forward and reverse)
  Rpm1 = map(PotValue1, 0, 255, (-1*MaxRpm), MaxRpm);
  Rpm2 = map(PotValue2, 0, 255, (-1*MaxRpm), MaxRpm);
  
  //We set the first victor's rpm display here
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
      
      //This line covers up the previous bits from other values.
      //It is a bit easier than clearing the whole display and rewriting the whole thing.
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
  
  //We set the second victor's rpm display here
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
  
  //Finally, we write these values to the victors. This bit still needs to be tested!
  analogWrite(VictorPin1, PotValue1);
  analogWrite(VictorPin2, PotValue2);
}

