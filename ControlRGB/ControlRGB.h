#ifndef ControlRGB_H
#define ControlRGB_H

#include "Arduino.h"

class ControlRGB
{
  public:
   	ControlRGB();
   	ControlRGB(int rPin, int gPin, int bPin);
   	void turnOn(int rBrightness, int gBrightness, int bBrightness, int delayTime); //delay in ms (delay is time to reach full brightness)
   	void turnOff();
  private:
    int _rPin;
    int _gPin;
    int _bPin;
};


#endif
