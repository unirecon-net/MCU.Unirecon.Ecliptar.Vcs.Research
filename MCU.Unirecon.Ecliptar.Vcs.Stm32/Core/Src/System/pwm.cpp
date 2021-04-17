#include "System/pwm.h"

#include <inttypes.h>
#include <tim.h>

PwmTimer::PwmTimer(TIM_HandleTypeDef *timer, uint32_t channel)
{
	this->timer = timer;
	this->channel = channel;
}

void PwmTimer::SetPulse(uint32_t pulseWidth)
{
	__HAL_TIM_SET_COMPARE(timer, channel, pulseWidth);
}

uint32_t PwmTimer::SetDuty(float dutyPercents)
{
	uint32_t pulseWidth = (dutyPercents * timer->Instance->ARR) / 100;
	__HAL_TIM_SET_COMPARE(timer, channel, pulseWidth);
	return pulseWidth;
}

