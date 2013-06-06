//Robot cRIO I2C Library

#ifndef FRCI2C_h
#define FRCI2C_h

#include "Arduino.h"
#include "Wire.h"

class I2C
{
   public:
      I2C::I2C(uint8_t i2cAddress, uint8_t _numBytes, bool _verifyBytes);
      bool isNewData();
      void setNewData(bool value);
      unsigned char returnDataByte(uint8_t byteToReturn);

   private:
      void readBytes(int byteCount);
      char * dataBytes;
      uint8_t numBytes;
      bool verifyBytes;
      bool dataChanged;
};

#endif