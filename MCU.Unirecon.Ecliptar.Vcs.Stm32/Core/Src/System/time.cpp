/*
 * EcliptarTools.cpp
 *
 *  Created on: Mar 15, 2021
 *      Author: merlin
 */
#include "System/time.h"


uint32_t tickms(void)
{
	return HAL_GetTick();
}
uint64_t tickus(void)
{
	return (HAL_GetTick() * 1000) + (1000 - (SysTick->VAL / 180)); // @suppress("Field cannot be resolved")
}
uint64_t tick(void)
{
	return (HAL_GetTick() * 180000) + (180000 - SysTick->VAL); // @suppress("Field cannot be resolved")
}




