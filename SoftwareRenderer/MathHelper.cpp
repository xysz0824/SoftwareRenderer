#include "MathHelper.h"

Matrix mmul(Matrix a, Matrix b)
{
	Matrix r;
	r.m._11 = a.m._11 * b.m._11 + a.m._12 * b.m._21 + a.m._13 * b.m._31 + a.m._14 * b.m._41;
	r.m._12 = a.m._11 * b.m._12 + a.m._12 * b.m._22 + a.m._13 * b.m._32 + a.m._14 * b.m._42;
	r.m._13 = a.m._11 * b.m._13 + a.m._12 * b.m._23 + a.m._13 * b.m._33 + a.m._14 * b.m._43;
	r.m._14 = a.m._11 * b.m._14 + a.m._12 * b.m._24 + a.m._13 * b.m._34 + a.m._14 * b.m._44;

	r.m._21 = a.m._21 * b.m._11 + a.m._22 * b.m._21 + a.m._23 * b.m._31 + a.m._24 * b.m._41;
	r.m._22 = a.m._21 * b.m._12 + a.m._22 * b.m._22 + a.m._23 * b.m._32 + a.m._24 * b.m._42;
	r.m._23 = a.m._21 * b.m._13 + a.m._22 * b.m._23 + a.m._23 * b.m._33 + a.m._24 * b.m._43;
	r.m._24 = a.m._21 * b.m._14 + a.m._22 * b.m._24 + a.m._23 * b.m._34 + a.m._24 * b.m._44;

	r.m._31 = a.m._31 * b.m._11 + a.m._32 * b.m._21 + a.m._33 * b.m._31 + a.m._34 * b.m._41;
	r.m._32 = a.m._31 * b.m._12 + a.m._32 * b.m._22 + a.m._33 * b.m._32 + a.m._34 * b.m._42;
	r.m._33 = a.m._31 * b.m._13 + a.m._32 * b.m._23 + a.m._33 * b.m._33 + a.m._34 * b.m._43;
	r.m._34 = a.m._31 * b.m._14 + a.m._32 * b.m._24 + a.m._33 * b.m._34 + a.m._34 * b.m._44;

	r.m._41 = a.m._41 * b.m._11 + a.m._42 * b.m._21 + a.m._43 * b.m._31 + a.m._44 * b.m._41;
	r.m._42 = a.m._41 * b.m._12 + a.m._42 * b.m._22 + a.m._43 * b.m._32 + a.m._44 * b.m._42;
	r.m._43 = a.m._41 * b.m._13 + a.m._42 * b.m._23 + a.m._43 * b.m._33 + a.m._44 * b.m._43;
	r.m._44 = a.m._41 * b.m._14 + a.m._42 * b.m._24 + a.m._43 * b.m._34 + a.m._44 * b.m._44;
	return r;
}

Matrix madd(Matrix a, Matrix b)
{
	Matrix r;
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			r.e[i][j] = a.e[i][j] + b.e[i][j];
		}
	}
	return r;
}

Matrix msub(Matrix a, Matrix b)
{
	Matrix r;
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			r.e[i][j] = a.e[i][j] - b.e[i][j];
		}
	}
	return r;
}

Vector4 v4mul(Vector4 v, Matrix m)
{
	Vector4 r;
	r.x = v.x * m.m._11 + v.y * m.m._21 + v.z * m.m._31 + v.w * m.m._41;
	r.y = v.x * m.m._12 + v.y * m.m._22 + v.z * m.m._32 + v.w * m.m._42;
	r.z = v.x * m.m._13 + v.y * m.m._23 + v.z * m.m._33 + v.w * m.m._43;
	r.w = v.x * m.m._14 + v.y * m.m._24 + v.z * m.m._34 + v.w * m.m._44;
	return r;
}