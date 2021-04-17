#ifndef _SYSTEM_PWM_H
#define _SYSTEM_PWM_H

#include <tim.h>
#include <inttypes.h>

class PwmTimer
{
public:
	PwmTimer(TIM_HandleTypeDef* timer, uint32_t channel);

	void SetPulse(uint32_t pulseWidth);
	uint32_t SetDuty(float dutyPercents);

	TIM_HandleTypeDef *timer = NULL;
	uint32_t channel = 0;
};


#endif /* _SYSTEM_PWM_H */
