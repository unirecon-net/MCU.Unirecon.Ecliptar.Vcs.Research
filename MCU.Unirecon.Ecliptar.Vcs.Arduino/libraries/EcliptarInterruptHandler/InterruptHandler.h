#ifndef INTERRUPTHANDLER_H
#define INTERRUPTHANDLER_H

#include "Arduino.h"


class EcliptarInterruptHandler {
public:
  virtual void handleInterrupt(int8_t interruptNum) = 0;
  void attachInterrupt(int8_t interruptNum, uint8_t mode);
  void detachInterrupt(int8_t interruptNum);
};

#endif
