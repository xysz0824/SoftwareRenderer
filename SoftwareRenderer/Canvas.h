#ifndef _CANVAS_H
#define _CANVAS_H
#include <memory>
#include <Windows.h>

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