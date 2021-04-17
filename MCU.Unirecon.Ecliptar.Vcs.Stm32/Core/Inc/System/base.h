#ifndef __SYSTEM_BASE_H
#define __SYSTEM_BASE_H

#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <gpio.h>

#define	 	LOW 	GPIO_PIN_RESET
#define 	HIGH 	GPIO_PIN_SET


#define DMA_DATA_LENGTH(s, u) (s - u->Instance->NDTR)

#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))

typedef uint8_t  u8;

typedef uint16_t u16;

typedef uint32_t u32;

typedef uint64_t u64;

typedef int8_t  i8;

typedef int16_t i16;

typedef int32_t i32;

typedef int64_t i64;

typedef float	  f32;

typedef double  f64;


i32 mapInt(i32 y, i32 y_min, i32 y_max, i32 x_min, i32 x_max);

float mapFloat(float y, float y_min, float y_max, float x_min, float x_max);

template<typename T>
T map(T y, T y_min, T y_max, T x_min, T x_max);

#endif /* __SYSTEM_BASE_H */
