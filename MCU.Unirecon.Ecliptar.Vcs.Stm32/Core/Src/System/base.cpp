#include "System/base.h"

i32 mapInt(i32 y, i32 y_min, i32 y_max, i32 x_min, i32 x_max)
{
	return (y-y_min)/(y_max-y_min)*(x_max-x_min)+x_min;
}

float mapFloat(float y, float y_min, float y_max, float x_min, float x_max)
{
	return (y-y_min)/(y_max-y_min)*(x_max-x_min)+x_min;
}

template<typename T>
T map(T y, T y_min, T y_max, T x_min, T x_max)
{
	return (y-y_min)/(y_max-y_min)*(x_max-x_min)+x_min;
}
