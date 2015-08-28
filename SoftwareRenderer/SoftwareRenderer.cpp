#include <Windows.h>
#include <stdio.h>
#include <assert.h>
#include "MathHelper.h"
#include "SoftwareRenderer.h"
#include "RenderBoard.h"

static bool			_hasWindow;
static int			_width;
static int			_height;
static RenderBoard*	_renderBoard = NULL;
LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

HWND SRCreateWindow(HINSTANCE hInstance, const char* name, int width, int height)
{
	_width = width;
	_height = height;
	//Prepare for registering window class
	WNDCLASSEX wndClass = { 0 };
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = WndProc;
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.hInstance = hInstance;
	wndClass.hIcon = NULL;
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.lpszMenuName = name;
	wndClass.lpszClassName = name;

	if (!RegisterClassEx(&wndClass))
	{
		MessageBox(NULL, "This program requires Windows NT!", name, MB_ICONERROR);
		return NULL;
	}

	//Create normal window
	HWND hwnd = CreateWindow(
		name,									//window class name
		name,									//window caption
		WS_OVERLAPPEDWINDOW &
		~WS_MAXIMIZEBOX & ~WS_THICKFRAME,		//window style
		CW_USEDEFAULT,							//initial x position
		CW_USEDEFAULT,							//initial y position
		width,									//initial x size
		height,									//initial y size
		NULL,									//parent window handle
		NULL,									//window menu handle
		hInstance,								//program instance handle
		NULL);									//creation parameters

	if (!hwnd)
	{
		MessageBox(NULL, "Failed to create window!", name, MB_ICONERROR);
		return NULL;
	}

	_hasWindow = true;

	return hwnd;
}

void SRSetRenderBoard(RenderBoard* renderBoard)
{
	_renderBoard = renderBoard;
}

WPARAM SRRunWindow(HWND hwnd, int nCmdShow)
{
	assert(_hasWindow);

	ShowWindow(hwnd, nCmdShow);

	_renderBoard->Initialize();

	MSG msg = { 0 };
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			HDC hdc;
			hdc = GetDC(hwnd);
			static DWORD d1, d2;
			static float dt;
			d1 = GetTickCount();
			_renderBoard->Render(hdc, dt);
			d2 = GetTickCount();
			dt = (d2 - d1) / 1000.0f;
			char a[30];
			int len = sprintf_s(a, "%f", dt);
			TextOut(hdc, 0, 0, a, len);
			ReleaseDC(hwnd, hdc);
		}
	}

	return msg.wParam;
}

void SRRender(HDC hdc, Canvas canvas)
{
	assert(_hasWindow);

	BYTE *pBits;
	HDC hMemDC;
	HBITMAP hBmp, hOldBmp;	//double buffer

	hMemDC = CreateCompatibleDC(hdc);
	BYTE bmibuf[sizeof(BITMAPINFO)+256 * sizeof(RGBQUAD)];	//create bitmapinfo buffer
	memset(bmibuf, 0, sizeof(bmibuf));
	BITMAPINFO* pbmi = (BITMAPINFO*)bmibuf;
	pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	pbmi->bmiHeader.biWidth = _width;
	pbmi->bmiHeader.biHeight = _height;
	pbmi->bmiHeader.biPlanes = 1;
	pbmi->bmiHeader.biBitCount = 24;
	pbmi->bmiHeader.biCompression = BI_RGB;
	hBmp = CreateDIBSection(hMemDC, pbmi, DIB_RGB_COLORS, (void**)&pBits, 0, 0); //create device-independent bitmap
	hOldBmp = (HBITMAP)SelectObject(hMemDC, hBmp);	//save the bitmap drawed last time
	BitBlt(hMemDC, 0, 0, _width, _height, hdc, 0, 0, SRCCOPY);	//output old bitmap to screen
	for (int y = 0; y < _height; ++y)
	{
		for (int x = 0; x < _width; ++x)
		{
			pBits[(_height - y - 1) * _width * 3 + x * 3 + 2] = canvas.buffer[y * canvas.width + x].r;
			pBits[(_height - y - 1) * _width * 3 + x * 3 + 1] = canvas.buffer[y * canvas.width + x].g;
			pBits[(_height - y - 1) * _width * 3 + x * 3	] = canvas.buffer[y * canvas.width + x].b;
		}
	}
	BitBlt(hdc, 0, 0, _width, _height, hMemDC, 0, 0, SRCCOPY);	//output new bitmap to memory
	SelectObject(hMemDC, hOldBmp);
	DeleteObject(hBmp);
	DeleteDC(hMemDC);
}

Canvas SRCreateCanvas(Pixel color)
{
	assert(_hasWindow);

	Pixel* buffer = new Pixel[_width * _height];
	for (int i = 0; i < _width * _height; ++i)
	{
		buffer[i].r = color.r;
		buffer[i].g = color.g;
		buffer[i].b = color.b;
	}
	Canvas canvas = { buffer, _width, _height };
	return canvas;
}

void SRDrawPoint(Canvas canvas, Vector2 position, Pixel color)
{
	Pixel* buffer = canvas.buffer;
	buffer[int(position.y) * canvas.width + int(position.x)] = color;
}

void SRDrawLine(Canvas canvas, Vector2 start, Vector2 end, Pixel color)
{
	float x = start.x;
	float y = start.y;
	float dx = end.x - start.x;
	float dy = end.y - start.y;
	float stepX = 1;
	if (start.x > end.x) stepX = -1;
	float stepY = 1;
	if (start.y > end.y) stepY = -1;
	//Draw start point
	SRDrawPoint(canvas, start, color);
	//Bresenham line drawing
	if (abs(dx) < abs(dy))
	{
		float p = dx - 2 * dy;
		while (x < end.x)
		{
			x += stepX;
			if (dx > 0)
			{
				if (p > 0)
					p += -2 * dy;
				else
				{
					y += stepY;
					p += 2 * dx - 2 * dy;
				}
			}
			SRDrawPoint(canvas, Vector2{ x, y }, color);
		}
	}
	else if (abs(dx) > abs(dy))
	{
		float p = dy - 2 * dx;
		while (y < end.y)
		{
			y += stepY;
			if (dy > 0)
			{
				if (p > 0)
					p += -2 * dx;
				else
				{
					x += stepX;
					p += 2 * dy - 2 * dx;
				}
			}
			SRDrawPoint(canvas, Vector2{ x, y }, color);
		}
	}
	else
	{
		while (x < end.x)
		{
			x += stepX;
			y += stepY;
			SRDrawPoint(canvas, Vector2{ x, y }, color);
		}
	}
}

void SRDrawLine(Canvas canvas, Vector2 start, float angle, float length, Pixel color)
{
	Vector2 end = { 
		start.x + length * float(cos(deg2rad(angle))), 
		start.y + length * float(sin(deg2rad(angle))) };
	SRDrawLine(canvas, start, end, color);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT paintStruct;
	HDC hdc;
	int horzRes, vertRes;

	switch (uMsg)
	{
	case WM_SIZE:
		horzRes = LOWORD(lParam);
		vertRes = HIWORD(lParam);
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &paintStruct);
		EndPaint(hwnd, &paintStruct);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}