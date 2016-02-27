#include "MathHelper.h"

Matrix mmul(Matrix a, Matrix b)
{
	Matrix r;
	r._11 = a._11 * b._11 + a._12 * b._21 + a._13 * b._31 + a._14 * b._41;
	r._12 = a._11 * b._12 + a._12 * b._22 + a._13 * b._32 + a._14 * b._42;
	r._13 = a._11 * b._13 + a._12 * b._23 + a._13 * b._33 + a._14 * b._43;
	r._14 = a._11 * b._14 + a._12 * b._24 + a._13 * b._34 + a._14 * b._44;

	r._21 = a._21 * b._11 + a._22 * b._21 + a._23 * b._31 + a._24 * b._41;
	r._22 = a._21 * b._12 + a._22 * b._22 + a._23 * b._32 + a._24 * b._42;
	r._23 = a._21 * b._13 + a._22 * b._23 + a._23 * b._33 + a._24 * b._43;
	r._24 = a._21 * b._14 + a._22 * b._24 + a._23 * b._34 + a._24 * b._44;

	r._31 = a._31 * b._11 + a._32 * b._21 + a._33 * b._31 + a._34 * b._41;
	r._32 = a._31 * b._12 + a._32 * b._22 + a._33 * b._32 + a._34 * b._42;
	r._33 = a._31 * b._13 + a._32 * b._23 + a._33 * b._33 + a._34 * b._43;
	r._34 = a._31 * b._14 + a._32 * b._24 + a._33 * b._34 + a._34 * b._44;

	r._41 = a._41 * b._11 + a._42 * b._21 + a._43 * b._31 + a._44 * b._41;
	r._42 = a._41 * b._12 + a._42 * b._22 + a._43 * b._32 + a._44 * b._42;
	r._43 = a._41 * b._13 + a._42 * b._23 + a._43 * b._33 + a._44 * b._43;
	r._44 = a._41 * b._14 + a._42 * b._24 + a._43 * b._34 + a._44 * b._44;
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
	r.x = v.x * m._11 + v.y * m._21 + v.z * m._31 + v.w * m._41;
	r.y = v.x * m._12 + v.y * m._22 + v.z * m._32 + v.w * m._42;
	r.z = v.x * m._13 + v.y * m._23 + v.z * m._33 + v.w * m._43;
	r.w = v.x * m._14 + v.y * m._24 + v.z * m._34 + v.w * m._44;
	return r;
}