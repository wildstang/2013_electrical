//MSGEQ7 Library

#ifndef MSGEQ7_h
#define MSGEQ7_h

#include "Arduino.h"

class MSGEQ7
{
   public:
      MSGEQ7(uint8_t _analogPin, uint8_t _strobePin, uint8_t _resetPin, bool _dualSides, uint8_t _pixelsPerSegment);
      void read();
   private:
      int baseline[7];
      int spectrumValue[7];
      int totalVolume;
      int relativeLevel;
      int averageVolume;
      uint8_t analogPin;
      uint8_t strobePin;
      uint8_t resetPin;
      bool dualSides;
      uint8_t pixelsPerSegment;
};

#endif