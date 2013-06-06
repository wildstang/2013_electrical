//Robot cRIO I2C Library

// Since the cRIO operates at 38.4kHz for its I2C clock, we need to set this manually here.
// This accesses the low level TWI library files found in /Wire/utility/
// This needs to be defined before we include as there is a check for a define in twi.c
#define TWI_FREQ 38400L

#include "FRCI2C.h"
#include "Arduino.h"
#include "Wire.h"

I2C::I2C(uint8_t i2cAddress, uint8_t _numBytes, bool _verifyBytes)
{
   dataChanged = false;
   numBytes = _numBytes;

   if (_numBytes % 2 == 0)
   {
      verifyBytes = false;
   }
   else
   {
      verifyBytes = _verifyBytes;
   }

   const unsigned char dataArray[(numBytes - 1)] = {0};

   dataBytes = dataArray;

}

void I2C::readBytes(int byteCount)
{
   if (byteCount == numBytes)
   {
      if (verifyBytes == true)
      {
         unsigned char unverifiedDataArray[(numBytes - 1)] = {0};
         for (uint8_t i = 0; i < (numbytes - 1); i++)
         {
            unverifiedDataArray[i] = (0x000000FF & Wire.read());
         }
         bool cleanData = true;
         for (uint8_t i == 1; i <= (numbytes - 1); i=i+2)
         {
            if((*(dataBytes + i) ^ *(dataBytes + i + 1)) != 0xFF)
            {
               cleanData = false;
               break;
            }
         }
         if (cleanData == true)
         {
            for (uint8_t i = 0; i < (numbytes - 1); i++)
            {
               *(dataBytes + i) = unverifiedDataArray[i];
            }
            dataChanged = true;
         }
      }
      else
      {
         for (uint8_t i = 0; i < (numbytes - 1); i++)
         {
            *(dataBytes + i) = (0x000000FF & Wire.read());
         }
         dataChanged = true;
      }
   }
}

void I2C::setNewData(bool value)
{
   dataChanged = value;
}

bool I2C::isNewData()
{
   return dataChanged;
}

unsigned char returnDataByte(uint8_t byteToReturn)
{
   return *(dataBytes + byteToReturn);
}