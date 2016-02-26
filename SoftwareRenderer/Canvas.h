#ifndef _CANVAS_H
#define _CANVAS_H
#include <memory>

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