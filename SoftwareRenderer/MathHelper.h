#ifndef _MATH_HELPER_H
#define _MATH_HELPER_H
#define _USE_MATH_DEFINES
#include <math.h>

struct Vector2 {
	float x, y;
};

inline double deg2rad(double degree)
{
	return degree / 180.0f * M_PI;
}
inline double rad2deg(double radian)
{
	return radian / M_PI * 180.0f;
}
#endif