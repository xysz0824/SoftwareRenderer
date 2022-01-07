#ifndef _MATH_HELPER_H
#define _MATH_HELPER_H
#define _USE_MATH_DEFINES
#include <math.h>

struct Vector2 {
	float x, y;
};
struct Vector3 {
	float x, y, z;
};
inline Vector3 v3add(Vector3 a, Vector3 b)
{
	return Vector3{ a.x + b.x, a.y + b.y, a.z + b.z };
}
inline Vector3 v3sub(Vector3 a, Vector3 b)
{
	return Vector3{ a.x - b.x, a.y - b.y, a.z - b.z };
}
inline Vector3 v3cross(Vector3 a, Vector3 b)
{
	return Vector3{ a.y * b.z - b.y * a.z, a.z * b.x - b.z * a.x, a.x * b.y - b.x * a.y };
}
inline float v3dot(Vector3 a, Vector3 b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}
inline Vector3 v3normalize(Vector3 v)
{
	float length = (float)sqrt((double)v.x * v.x + (double)v.y * v.y + (double)v.z * v.z);
	return length == 0.0f ? v : Vector3{ v.x / length, v.y / length, v.z / length };
}

struct Matrix {
	union 
	{
		struct 
		{
			float	_11, _12, _13, _14;
			float	_21, _22, _23, _24;
			float	_31, _32, _33, _34;
			float	_41, _42, _43, _44;
		} m;
		float e[4][4];
	};
};
Matrix mmul(Matrix a, Matrix b);
Matrix madd(Matrix a, Matrix b);
Matrix msub(Matrix a, Matrix b);

struct Vector4 {
	float x, y, z, w;
};
Vector4 v4mul(Vector4 v, Matrix m);
inline Vector4 v4div(Vector4 v, float n)
{
	return Vector4{ v.x / n, v.y / n, v.z / n, v.w / n };
}

inline float deg2rad(float degree)
{
	return (float)(degree / 180.0f * M_PI);
}
inline float rad2deg(float radian)
{
	return (float)(radian / M_PI * 180.0f);
}
#endif