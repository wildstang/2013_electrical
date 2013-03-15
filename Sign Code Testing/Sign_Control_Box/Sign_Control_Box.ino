/*
WildStang Sign Control Box 2013
By: Josh Smith and Steve Garward
*/

#include "SPI.h"
#include "Wire.h"
#include "LiquidCrystal.h"

#define PATTERN_RAINBOW 1
#define PATTERN_RED_FILL 2
#define PATTERN_BLUE_FILL 3
#define PATTERN_RED_FILL_SHIMMER 4
#define PATTERN_BLUE_FILL_SHIMMER 5
#define PATTERN_RED_FILL_TILT 6
#define PATTERN_BLUE_FILL_TILT 7
#define PATTERN_TWINKLE 8


#define RAINBOW_TEXT "Rainbow"
#define RED_FILL_TEXT "Red fill"
#define BLUE_FILL_TEXT "Blue fill"
#define RED_FILL_SHIMMER_TEXT "Red fill shimmer"
#define BLUE_FILL_SHIMMER_TEXT "Blue fill shimmer"
#define RED_FILL_TILT_TEXT "Red fill tilt"
#define BLUE_FILL_TILT_TEXT "Blue fill tilt"
#define TWINKLE_TEXT "Twinkle"

//Define pins used for the MSGEQ7 chip
#define RESET_PIN 0
#define STROBE_PIN 4
#define AUDIO_READ_PIN A2

//Define pins used for the LCD RGB LEDs
#define LCD_RED_PIN 5
#define LCD_GREEN_PIN 6
#define LCD_BLUE_PIN 9

//Define pins used for the Switches
#define SWITCH_1_PIN 8
#define SWITCH_2_PIN 7
#define SWITCH_3_PIN 1

//Define pins used for the Encoder
#define ENCODER_A_PIN 2
#define ENCODER_B_PIN 3
#define ENCODER_SWITCH_PIN A1

// Define the sign addresses
#define SIGN_1 0x0a
#define SIGN_2 0x0b
#define SIGN_3 0x0c

// This means send to all signs
#define SIGN_ALL 255

boolean patternChanged = false;
short pattern = 0;

//This both initializes the Wire library and connects to the LCD backpack on address 0
LiquidCrystal lcd(0);


//#define WS_DEBUG

void setup()
{
  //This sets up the lcd with the dimensions of (columns, rows)
  lcd.begin(16, 2);
  
  //Setup the pins for the MSGEQ7 chip
  pinMode(RESET_PIN, OUTPUT);
  pinMode(STROBE_PIN, OUTPUT);
  pinMode(AUDIO_READ_PIN, INPUT);
  
  //Setup the pins for the LCD RGB LEDs
  pinMode(LCD_RED_PIN, OUTPUT);
  pinMode(LCD_GREEN_PIN, OUTPUT);
  pinMode(LCD_BLUE_PIN, OUTPUT);
  
  //Setup the pins for the switches
  pinMode(SWITCH_1_PIN, INPUT);
  pinMode(SWITCH_2_PIN, INPUT);
  pinMode(SWITCH_3_PIN, INPUT);
  
  //Setup the pins for the encoder
  pinMode(ENCODER_A_PIN, INPUT);
  pinMode(ENCODER_B_PIN, OUTPUT);
  pinMode(ENCODER_SWITCH_PIN, INPUT);

#ifdef WS_DEBUG
  Serial.begin(9600);
#endif
}


void loop()
{
  if (hasPatternChanged())
  {
    switch (pattern)
    {
    case PATTERN_RAINBOW:
      sendPattern(SIGN_ALL, PATTERN_RAINBOW);
      break;
    case PATTERN_RED_FILL:
      sendPattern(SIGN_ALL, PATTERN_RED_FILL);
      break;
    case PATTERN_BLUE_FILL:
      sendPattern(SIGN_ALL, PATTERN_BLUE_FILL);
      break;
    case PATTERN_RED_FILL_TILT:
      sendPattern(SIGN_1, PATTERN_RED_FILL);
//      while (accelAngle(X_AXIS, 30))
      {
        // Do nothing
      }
      sendPattern(SIGN_2, PATTERN_RED_FILL_SHIMMER);
      timedWait(1000);
      sendPattern(SIGN_3, PATTERN_RED_FILL_SHIMMER);
      break;
    case PATTERN_BLUE_FILL_TILT:
      sendPattern(SIGN_1, PATTERN_BLUE_FILL);
//      while (accelAngle(X_AXIS, 30))
      {
        // Do nothing
      }
      sendPattern(SIGN_2, PATTERN_BLUE_FILL_SHIMMER);
      timedWait(1000);
      sendPattern(SIGN_3, PATTERN_BLUE_FILL_SHIMMER);
      break;
    case PATTERN_TWINKLE:
      sendPattern(SIGN_ALL, PATTERN_TWINKLE);
      break;
    default:
      sendPattern(SIGN_ALL, PATTERN_RAINBOW);
      break;
    }
  }
}




void sendPattern(int sign, int pattern)
{
  if (sign == SIGN_ALL)
  {
    sendPatternMessage(SIGN_1, pattern);
    sendPatternMessage(SIGN_2, pattern);
    sendPatternMessage(SIGN_3, pattern);
  }
  else
  {
    sendPatternMessage(sign, pattern);
  }
}

void sendPatternMessage(byte address, byte pattern)
{
  Wire.beginTransmission(address);
  Wire.write(pattern);
  Wire.endTransmission();
}

boolean hasPatternChanged()
{
  return patternChanged;
}


void setPattern(int newPattern)
{
  pattern = newPattern;
  patternChanged = true;
}

boolean timedWait(int waitTime)
{
  unsigned long previousMillis = millis();
  unsigned long currentMillis = millis();
  for(previousMillis; (currentMillis - previousMillis) < waitTime; currentMillis = millis())
  {
    if (patternChanged == true)
    {
      return true;
    }
  }
  return false;
}

