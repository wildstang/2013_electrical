//MSGEQ7 Library
#include "MSGEQ7.h"
#include "Arduino.h"

MSGEQ7::MSGEQ7(uint8_t _analogPin, uint8_t _strobePin, uint8_t _resetPin, bool _dualSides, uint8_t _pixelsPerSegment)
{
   analogPin = _analogPin;
   strobePin = _strobePin;
   resetPin = _resetPin;
   dualSides = _dualSides;
   pixelsPerSegment = _pixelsPerSegment;
   pinMode(analogPin, INPUT);
   pinMode(strobePin, OUTPUT);
   pinMode(resetPin, OUTPUT);
   analogReference(DEFAULT);

   digitalWrite(resetPin, LOW);
   digitalWrite(strobePin, HIGH);
 
   // Initialise and get a baseline
   digitalWrite(resetPin, HIGH);
   digitalWrite(resetPin, LOW);

   for (byte i = 0; i < 7; i++)
   {
      baseline[i] = 0;
   }

   for (byte j = 0; j < 10; j++)
   {
      for (byte i = 0; i < 7; i++)
      {
         digitalWrite(strobePin, LOW);
         delayMicroseconds(30);  // Wait to allow the output to settle
         baseline[i] += analogRead(analogPin);
         digitalWrite(strobePin, HIGH);
      }
   }

   for (byte i = 0; i < 7; i++)
   {
      // Average the baseline readings
      baseline[i] = baseline[i] / 10;
   }
}

void MSGEQ7::read()
{
   //We first need to read the pot value and map it to between 0 and 30 because our realistic max is 3.0.
   //Since map only returns whole numbers, we then divide by 10 to get our true max of 3.0.
   //SENSITIVITY = map(analogRead(MIC_SENSITIVITY), 0, 1023, 0, 100) / 10; //THIS HAS BEEN CHANGED TO MAX OF 10.0!
   
   digitalWrite(resetPin, HIGH); 
   digitalWrite(resetPin, LOW);
   totalVolume = 0;

   for (uint8_t i=0; i < 7; i++)
   {
     digitalWrite(strobePin, LOW);
     delayMicroseconds(30);  // to allow the output to settle
     spectrumValue[i] = analogRead(analogPin) - baseline[i];
     digitalWrite(strobePin, HIGH);
     if (spectrumValue[i] < 0)
     {
       spectrumValue[i] = 0;
     }
     
     // Spectrum is now 0-30 (NOW 0-20)
     spectrumValue[i] = map((spectrumValue[i] * sensitivity / 100), 0, 100, 0, 20);

     totalVolume += spectrumValue[i];
   }

   // Average the volume - now a value 0-20
   averageVolume = totalVolume / 7;
}