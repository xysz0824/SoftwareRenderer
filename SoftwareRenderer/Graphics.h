#ifndef _GRAPHICS_H
#define _GRAPHICS_H
#include <memory>
#include "MathHelper.h"

struct Pixel
{
	byte r, g, b;
};
typedef Pixel RGB;

struct Canvas
{
	Pixel* buffer;
	float* zBuffer;
	int x, y, w, h;
};

struct Vertex
{
	Vector4 position;
	Vector3 normal;
	Vector2 texcoord;
};

struct Object
{
	Vertex* mesh;
	int		totalVertex;
	int*	indices;
	int		totalIndices;
	Vector3 position;
	Vector3 eulerAngle;
	Vector3 scale;
};

struct Texture
{
	Pixel* data;
	int w, h;
};

Matrix	CreateTranslation(Vector3 position);
Matrix	CreateRotateX(float angle);
Matrix	CreateRotateY(float angle);
Matrix	CreateRotateZ(float angle);
Matrix	CreateScaling(Vector3 scale);
Matrix	CreateViewToLH(Vector3 position, Vector3 direction, Vector3 up);
Matrix	CreateViewAtLH(Vector3 position, Vector3 target, Vector3 up);
Matrix	CreatePerspectiveLH(float fieldofView, float aspectRadio, float zNear, float zFar);
Object	CreateCube(Vector3 position, Vector3 eulerAngle, float side);
Texture	LoadBitmap24(const char* path);
bool	IsBackface(Vector2* points);
bool	IsBackface(Vector3* points);
bool	IsInvisible(Vector3* points);

#endif