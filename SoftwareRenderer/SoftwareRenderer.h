#ifndef _SOFTWARE_RENDERER_H
#define _SOFTWARE_RENDERER_H
#include <Windows.h>
#include "Canvas.h"
#include "Vector.h"

class RenderBoard;

HWND	SRCreateWindow(HINSTANCE hInstance, const char* name, int width, int height);
Canvas	SRCreateCanvas(Pixel color);
void	SRSetRenderBoard(RenderBoard* renderBoard);
WPARAM	SRRunWindow(HWND hwnd, int nCmdShow);
void	SRRender(HDC hdc, Canvas canvas);
void	SRClearCanvas(Canvas canvas, Pixel color);
void	SRDrawPoint(Canvas canvas, Vector2 position, Pixel color);
void	SRDrawLine(Canvas canvas, Vector2 start, Vector2 end, Pixel color);
void	SRDrawLine(Canvas canvas, Vector2 start, float angle, float length, Pixel color);

#define Main() WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow)
#endif

