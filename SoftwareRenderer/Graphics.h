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
	int x, y, w, h;
};

struct Vertex
{
	Vector3 position;
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
#endif