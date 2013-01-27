/*
Wildstang Signs 2013
By: Josh Smith and Steve Garward
*/

/***********************************************************************************************
   NOTES:
   -
***********************************************************************************************/

#include "LPD8806.h"
#include "SPI.h"
#include <Wire.h>

//Define Data Pins here
int dataPin = 2;
int clockPin = 3;

//The number of LEDs on the sign
int numLeds = 82;

//The global variable that determines if the device joins as a master or slave for I2C
boolean master = true;
//This is the address to use if this is a slave device.
int i2cAddress = 82;  //HEX 0x52
//Defines if old data = new data
boolean dataChanged = false;

//Define the number of LEDs in use (the first number in the function) along with data and clock pins
LPD8806 strip = LPD8806(numLeds, dataPin, clockPin);

setup()
{
  //start the LED strip
  strip.begin();

  //update the strip to ensure that all the LEDs are all off at the beginning
  strip.show();
  
  if(master == true)
  {
    //Begin I2C communications as a MASTER. receiveData will be called when new data arrives
    //We call this last to avoid a nasty bug involving the LED initialization code
    Wire.begin();
  }
  else
  {
    //Begin I2C communications as a SLAVE. receiveData will be called when new data arrives
    //We call this last to avoid a nasty bug involving the LED initialization code
    Wire.begin(i2cAddress);
    Wire.onReceive(receiveData);
  }
}

loop()
{
  
}
