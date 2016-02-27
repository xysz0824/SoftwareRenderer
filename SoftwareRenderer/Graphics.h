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
#endif