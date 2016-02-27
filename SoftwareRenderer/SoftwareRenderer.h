#ifndef _SOFTWARE_RENDERER_H
#define _SOFTWARE_RENDERER_H
#include <Windows.h>
#include "Canvas.h"
#include "MathHelper.h"

class RenderBoard;

HWND	SRCreateWindow(HINSTANCE hInstance, const char* name, int width, int height);
Canvas	SRCreateCanvas(int x, int y, int w, int h);
void	SRClearCanvas(Canvas canvas, RGB color);
void	SRSetRenderBoard(RenderBoard* renderBoard);
WPARAM	SRRunWindow(HWND hwnd, int nCmdShow);
void	SRRender(HDC hdc, Canvas canvas);
void	SRDrawPoint(Canvas canvas, Vector2 position, RGB color);
bool	SRClipLine(Canvas canvas, Vector2* start, Vector2* end);
void	SRDrawLine(Canvas canvas, Vector2 start, Vector2 end, RGB color);
void	SRDrawLine(Canvas canvas, Vector2 start, float angle, float length, RGB color);
void	SRDrawTriangle(Canvas canvas, Vector2 vertex[3], RGB color);
void	SRDrawPolygon(Canvas canvas, Vector2 center, int edge, float radius, float rotation, RGB color);void	SRDrawCircle(Canvas canvas, Vector2 center, float radius, RGB color);
Matrix	SRCreateCamera();
void	SRSetCamera(Matrix camera);
void	SRDrawLine3D(Canvas canvas, Vector3 start, Vector3 end, RGB color);

int		SRGetWindowWidth();
int		SRGetWindowHeight();

#define Main() WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow)
#endif

