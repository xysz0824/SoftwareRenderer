#include <assert.h>
#include "SystemWindow.h"
#include "RenderBoard.h"

static bool			_hasWindow;
static int			_width;
static int			_height;
static RenderBoard* _renderBoard = NULL;
static Canvas		_canvas;
static BYTE*		_bits;
static HDC			_memDC;
static HBITMAP		_bitmap;

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

HWND SWCreateWindow(HINSTANCE hInstance, const char* name, int width, int height)
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
	wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndClass.lpszMenuName = name;
	wndClass.lpszClassName = name;

	if (!RegisterClassEx(&wndClass))
	{
		MessageBox(NULL, "This program requires Windows NT!", name, MB_ICONERROR);
		return NULL;
	}

	//Get correct window rect
	RECT rect;
	SetRect(&rect, 0, 0, width, height);
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_THICKFRAME, false);

	//Create normal window
	HWND hwnd = CreateWindow(
		name,									//window class name
		name,									//window caption
		WS_OVERLAPPEDWINDOW &
		~WS_MAXIMIZEBOX & ~WS_THICKFRAME,		//window style
		CW_USEDEFAULT,							//initial x position
		CW_USEDEFAULT,							//initial y position
		rect.right - rect.left,					//initial x size
		rect.bottom - rect.top,					//initial y size
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

int	SWGetWindowWidth()
{
	return _width;
}
int	SWGetWindowHeight()
{
	return _height;
}

void SWSetRenderBoard(RenderBoard* renderBoard)
{
	_renderBoard = renderBoard;
}

WPARAM SWRunWindow(HWND hwnd, int nCmdShow)
{
	static int		msec, fps;
	static DWORD	d1, d2;
	static float	dt;
	static char		str[30] = "0";
	static int		strLen;
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
			d1 = GetTickCount();
			HDC hdc = GetDC(hwnd);
			_renderBoard->Render(dt);
			SWRender(hdc);
			if (msec >= 200)
			{
				strLen = sprintf_s(str, "FPS:%d", fps * (1000 / msec));
				msec = 0;
				fps = 0;
			}
			SetBkMode(hdc, TRANSPARENT);
			SetTextColor(hdc, 0xFFFFFF);
			TextOut(hdc, 0, 0, str, strLen);
			d2 = GetTickCount();
			dt = (d2 - d1) / 1000.0f;
			msec += d2 - d1;
			fps++;
			ReleaseDC(hwnd, hdc);
		}
	}

	if (_memDC)
	{
		DeleteObject(_bitmap);
		DeleteDC(_memDC);
	}

	return msg.wParam;
}

void SWRender(HDC hdc)
{
	assert(_hasWindow);

	if (_memDC == NULL)
	{
		_memDC = CreateCompatibleDC(hdc);
		//create bitmapinfo buffer
		BYTE bmibuf[sizeof(BITMAPINFO) + 256 * sizeof(RGBQUAD)];
		memset(bmibuf, 0, sizeof(bmibuf));
		BITMAPINFO* pbmi = (BITMAPINFO*)bmibuf;
		pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		pbmi->bmiHeader.biWidth = _width;
		pbmi->bmiHeader.biHeight = _height;
		pbmi->bmiHeader.biPlanes = 1;
		pbmi->bmiHeader.biBitCount = 24;
		pbmi->bmiHeader.biCompression = BI_RGB;
		//create device-independent bitmap
		_bitmap = CreateDIBSection(_memDC, pbmi, DIB_RGB_COLORS, (void**)&_bits, 0, 0);
		SelectObject(_memDC, _bitmap);
	}
	for (int y = 0; y < _height; ++y)
	{
		for (int x = 0; x < _width; ++x)
		{
			int bIndex = y * _canvas.w + x;
			int pIndex = (_height - (y + _canvas.y) - 1) * _width * 3 + (x + _canvas.x) * 3;
			if (x + _canvas.x < 0 || x + _canvas.x >= _width ||
				y + _canvas.y < 0 || y + _canvas.y >= _height)
				continue;

			_bits[pIndex + 2] = _canvas.buffer[bIndex].r;
			_bits[pIndex + 1] = _canvas.buffer[bIndex].g;
			_bits[pIndex] = _canvas.buffer[bIndex].b;
		}
	}
	//output new bitmap to memory
	BitBlt(hdc, 0, 0, _width, _height, _memDC, 0, 0, SRCCOPY);
}

Canvas SWCreateCanvas(int x, int y, int w, int h)
{
	assert(_hasWindow);

	Pixel* buffer = new Pixel[_width * _height];
	float* zBuffer = new float[_width * _height];
	Canvas canvas = { buffer, zBuffer, x, y, w, h };
	return canvas;
}

void SWClearCanvas(Canvas canvas, RGB color)
{
	for (int i = 0; i < canvas.w * canvas.h; ++i)
	{
		canvas.buffer[i].r = color.r;
		canvas.buffer[i].g = color.g;
		canvas.buffer[i].b = color.b;
		canvas.zBuffer[i] = 0;
	}
}

void SWSetCanvas(Canvas canvas)
{
	_canvas = canvas;
}