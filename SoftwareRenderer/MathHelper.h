#ifndef _MATH_HELPER_H
#define _MATH_HELPER_H
#define _USE_MATH_DEFINES
#include <math.h>

struct Vector2 {
	float x, y;
};

struct Matrix {
	union 
	{
		struct 
		{
			float	_11, _12, _13, _14;
			float	_21, _22, _23, _24;
			float	_31, _32, _33, _34;
			float	_41, _42, _43, _44;
		};
		float e[4][4];
	};
};
Matrix mmul(Matrix a, Matrix b);
Matrix madd(Matrix a, Matrix b);
Matrix msub(Matrix a, Matrix b);

inline double deg2rad(double degree)
{
	return degree / 180.0f * M_PI;
}
inline double rad2deg(double radian)
{
	return radian / M_PI * 180.0f;
}
#endif