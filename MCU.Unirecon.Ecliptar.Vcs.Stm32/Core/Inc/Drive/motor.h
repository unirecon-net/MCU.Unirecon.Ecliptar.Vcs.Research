/*
 * ecliptar_drive.h
 *
 *  Created on: Mar 15, 2021
 *      Author: merlin
 */

#ifndef __DRIVE_MOTOR_H
#define __DRIVE_MOTOR_H

#include <inttypes.h>

#include "Drive/base.h"

#define MIN_MOTOR_POWER                 70
#define MAX_MOTOR_POWER                 200

typedef struct
{
  PwmTimer*		  		  	 MotorPwm;
  volatile int               powerLevel;
  volatile float             dutyLevel;
  volatile int               innerPowerLevel;
  volatile float             innerDutyLevel;
  volatile int               maxPowerLevel;
  volatile int               minPowerLevel;
  volatile uint16_t          revolutionSpeed;
  volatile uint8_t           running : 1;
  volatile uint8_t           started : 1;
  volatile uint8_t           powerOn : 1;
  volatile uint8_t           powerPwmOn : 1;
  volatile uint8_t           powerLocked : 1;
} MotorProperties;

class MotorDriver
{
public:
  MotorDriver();

  void                    Attach(PwmTimer* motorPwm);
  void                    Attach(PwmTimer* motorPwm, int minPower, int maxPower);

  bool                    Attached();

  void                    PowerLevel(int pwmValue);
  void                    PowerLevel(float dutyValue);

  void                    revolutionSpeed(uint16_t rpm);

  MotorProperties        Properties;

  PwmTimer*               MotorPwm = NULL;

private:

  void                    innerPowerLevel(int pwmValue);
  void                    innerPowerLevel(float dutyValue);

};

#endif /* __DRIVE_MOTOR_H */
