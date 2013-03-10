/*
Wildstang Lights 2013
By: Josh Smith and Steve Garward
*/

#include "LPD8806.h"
#include "SPI.h"
#include <Wire.h>

//Define the number of leds that are on the robot and connected to this arduino
#define numLeds 52
#define halfNumLeds numLeds

//This the address to use for the arduino in slave mode when talking to the cRIO
//This number may differ from the address that the cRIO expects. Sniff the I2C lines
//to check what address correlates to the actual integer given here
int i2cAddress = 82;

//This is the number of the digital IO pin that will be pulled to 5V to signify that
//the sound board is connected
int soundBoardCheck = 7;

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
int PIXELS_PER_SEGMENT = 2; //How many LEDs we want to use for each segment of the "VU meter"
boolean IS_SOUND_BOARD_IN = false; //Leave this alone as it will be automatically set to true if you are using our sound board
boolean DUAL_SIDES = true; //Do you want both sides of the LED strip to have the meter or just one?
float SENSITIVITY = 2.3; //This sets the sensitivity of the mic (increase for quiet environment (3.0 should be upper bound))
int analogPin = 0; // read from multiplexer using analog input 0
int strobePin = 3; // strobe is attached to digital pin 3
int resetPin = 6; // reset is attached to digital pin 6

int spectrumValue[7]; // to hold a2d values
int baseline[7];
boolean initialise = false;
int totalVolume = 0;
int relativeLevel = 0;
int averageVolume = 0;
/******************************************************************************/

//Define the number of LEDs in use (the first number in the function). This uses pin 11 for SDA (data) and pin 13
//for SCL (clock)
LPD8806 strip = LPD8806(numLeds);

//This is used for the initial 30 second wait period where our robot would calibrate the gyro and could not be touched.
//Just switch this to "false" if you do not want this initial wait period.
boolean firstRun = true;

void setup()
{
  //This reads the noise off of Analog input 2 and seeds the random() function
  randomSeed(analogRead(2));
  //Sound Board Setup
  int state = 0;
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

    for (int i = 0; i < 7; i++)
    {
       baseline[i] = 0;
    }

    for (int j = 0; j < 10; j++)
    {
       for (int i = 0; i < 7; i++)
       {
          digitalWrite(strobePin, LOW);
          delayMicroseconds(30);  // to allow the output to settle
          baseline[i] += analogRead(analogPin);
          digitalWrite(strobePin, HIGH);
       }
    }

    for (int i = 0; i < 7; i++)
    {
       // Average the baseline readings
       baseline[i] = baseline[i] / 10;
    }
  }
  //start the LED strip
  strip.begin();

  //update the strip to ensure that all the LEDs are all off at the beginning
  strip.show();
  
  //Begin I2C communications as a SLAVE. receiveData will be called when new data arrives
  //We call this last to avoid a nasty bug involving the LED initialization code
  Wire.begin(i2cAddress);
  Wire.onReceive(receiveData);
}

//Function Prototypes

void loop()
{
  
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

/***************************************************************************************************
                                   Actual light logic functions
                          (setting colors and writing to the LED strips)
***************************************************************************************************/

//Lights up the whole strip 
void rainbowWheel()
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
   
    if (true == timedWaitFunction(50))
    {
      break;
    }
  }
}

/***************************************************************************************************
                                        Utility functions
                                 Assisting actual light functions
***************************************************************************************************/

//Input a value 0 to 384 to get a color value.
//The colours are a transition r - g - b - back to r
uint32_t Wheel(uint16_t WheelPos)
{
  byte r, g, b;
  switch(WheelPos / 128)
  {
    case 0:
      r = 127 - WheelPos % 128; // red down
      g = WheelPos % 128;       // green up
      b = 0;                    // blue off
      break;
    case 1:
      g = 127 - WheelPos % 128; // green down
      b = WheelPos % 128;       // blue up
      r = 0;                    // red off
      break;
    case 2:
      b = 127 - WheelPos % 128; // blue down
      r = WheelPos % 128;       // red up
      g = 0;                    // green off
      break;
  }
  return(strip.Color(r,g,b));
}

boolean timedWaitFunction(int waitTime)
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
