#ifndef _CANVAS_H
#define _CANVAS_H
#include <memory>
#include <Windows.h>

struct Pixel
{
	byte r, g, b;
};

struct Canvas
{
	Pixel* buffer;
	int width;
	int height;
};
#endif