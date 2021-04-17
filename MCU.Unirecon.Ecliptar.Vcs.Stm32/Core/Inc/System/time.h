/*
 * EcliptarTools.h
 *
 *  Created on: Mar 15, 2021
 *      Author: merlin
 */

#ifndef _TIME_H
#define _TIME_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"
#include <inttypes.h>

uint32_t tickms(void);
uint64_t tickus(void);
uint64_t tick(void);

#ifdef __cplusplus
}
#endif

#endif /* _TIME_H */
