/*
 * ecliptar_drive.c
 *
 *  Created on: Mar 15, 2021
 *      Author: merlin
 */


#include "Drive/motor.h"


MotorDriver::MotorDriver()
{
  Properties.powerLocked = 0;
  Properties.running = 0;
  Properties.powerOn = 0;
  Properties.powerPwmOn = 0;
  Properties.powerLevel = 0;
  Properties.innerPowerLevel = 0;
}
void                    MotorDriver::Attach(PwmTimer* motorPwm)
{
  Attach(motorPwm, 50, 200);
}
void                    MotorDriver::Attach(PwmTimer* motorPwm, int minPower, int maxPower)
{
  Properties.MotorPwm = motorPwm;
  MotorPwm = Properties.MotorPwm;
  Properties.minPowerLevel = minPower;
  Properties.maxPowerLevel = maxPower;

  HAL_TIM_PWM_Start(MotorPwm->timer, MotorPwm->channel);
}

bool                    MotorDriver::Attached()
{
  return  Properties.MotorPwm->timer != NULL ? true : false;
}

void                    MotorDriver::PowerLevel(int pwmValue)
{
  if (Properties.powerLevel != pwmValue)
  {
    bool running = pwmValue > 3;
    if (running)
      pwmValue = map<u32>(pwmValue, 4, 255, Properties.minPowerLevel, Properties.maxPowerLevel);
    else
      pwmValue = 0;

    Properties.running = running;
    Properties.powerLevel = pwmValue;
    innerPowerLevel(pwmValue);
  }
}
inline void            MotorDriver::innerPowerLevel(int pwmValue)
{
  if (Properties.innerPowerLevel != pwmValue)
  {
    Properties.innerPowerLevel = pwmValue;
    Properties.powerOn = pwmValue > 0;
    Properties.MotorPwm->SetPulse(pwmValue);
  }
}
void                    MotorDriver::PowerLevel(float dutyValue)
{
  if (Properties.dutyLevel != dutyValue)
  {
    bool running = dutyValue > 3;
	if (running)
	{
		Properties.powerLevel = (int)map<float>(dutyValue, 0.00F, 100.00F, (float)Properties.minPowerLevel,
							(float)Properties.maxPowerLevel);
	}
	else
	{
		Properties.powerLevel = 0;
		dutyValue = 0;
	}

    Properties.running = running;
    Properties.dutyLevel = dutyValue;
    innerPowerLevel(dutyValue);
  }
}
inline void            MotorDriver::innerPowerLevel(float dutyValue)
{
  if (Properties.innerDutyLevel != dutyValue)
  {
    Properties.innerDutyLevel = dutyValue;
    Properties.powerOn = dutyValue > 0;
    Properties.innerPowerLevel = Properties.MotorPwm->SetDuty(dutyValue);
  }
}


void                    MotorDriver::revolutionSpeed(uint16_t rpm)
{

}


