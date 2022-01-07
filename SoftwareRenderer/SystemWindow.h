#ifndef _SYSTEM_WINDOW_H
#define _SYSTEM_WINDOW_H
#include <Windows.h>
#include "Graphics.h"

#define Main() WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow)

class RenderBoard;

HWND	SWCreateWindow(HINSTANCE hInstance, const char* name, int width, int height);
Canvas	SWCreateCanvas(int x, int y, int w, int h);
void	SWClearCanvas(Canvas canvas, RGB color);
void	SWSetCanvas(Canvas canvas);
void	SWSetRenderBoard(RenderBoard* renderBoard);
WPARAM	SWRunWindow(HWND hwnd, int nCmdShow);
void	SWRender(HDC hdc);
int		SWGetWindowWidth();
int		SWGetWindowHeight();

#endif