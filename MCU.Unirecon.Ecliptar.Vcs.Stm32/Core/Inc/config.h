/*
 * ecliptar_steering.h
 *
 *  Created on: Mar 15, 2021
 *      Author: merlin
 */

#ifndef _CONFIG_H_
#define _CONFIG_H_

#include "drive.h"
#include "rc.h"

void DriveSystem_Config()
{
	Drive.LeftEcliptor.DefineAreas(580, 620, 690, 600, 735, 860, 735);
	Drive.LeftEcliptor.DefineRotator(600, 2400, 1500, 20, 160, normal);
	Drive.LeftEcliptor.Attach(new PwmTimer(&htim1, TIM_CHANNEL_1),
								new PwmTimer(&htim3, TIM_CHANNEL_1),
								new AdcData(&hadc1, ADC_CHANNEL_10, 100),
								625, 870, 660, 760, 100, 2047);

	Drive.RightEcliptor.DefineAreas(580, 620, 690, 600, 735, 860, 735);
	Drive.RightEcliptor.DefineRotator(600, 2400, 1500, 20, 160, reverse);
	Drive.RightEcliptor.Attach(new PwmTimer(&htim1, TIM_CHANNEL_2),
								new PwmTimer(&htim3, TIM_CHANNEL_2),
								new AdcData(&hadc2, ADC_CHANNEL_11, 100),
								625, 870, 660, 760, 100, 2047);

	Drive.Motor.Attach(new PwmTimer(&htim2, TIM_CHANNEL_3), 100, 2047);

	uint32_t chnls[]	{ ADC_CHANNEL_10, ADC_CHANNEL_11 };
	Drive.Init(new AdcData(&hadc3, 2, chnls, 4));
}

void RemoteController_Config()
{
	uint16_t rcMinPulses[] =
	{ 910, 910, 920, 920, 910, 910, 1100, 1100, 1100, 1100 };
	uint16_t rcMidPulses[] =
	{ 1500, 1500, 920, 1500, 910, 910, 1100, 1100, 1100, 1100 };
	uint16_t rcMaxPulses[] =
	{ 2090, 2090, 2080, 2080, 2090, 2090, 1900, 1900, 1900, 1900 };
	RC.Attach(10, &huart5, rcMinPulses, rcMidPulses, rcMaxPulses);
}

void SerialUSB_Config()
{
#ifndef DEBUG
	if(SerialUSB == NULL)
	{
		SerialUSB = new SerialDevice(&huart2);
	}

	setPrinter(SerialUSB);
	setScanner(SerialUSB);
#endif
}

#endif /* _CONFIG_H_ */
