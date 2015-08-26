#include <Windows.h>
#include <stdio.h>

#define WINDOW_NAME "Software Renderer"
#define WINDOW_SIZE_X 800
#define WINDOW_SIZE_Y 600

struct PIXELCOLOR
{
	byte r, g, b;
};

void RenderBuffer(HDC hdc, PIXELCOLOR* buffer, UINT width, UINT height)
{
	BYTE *pBits;
	HDC hMemDC;
	HBITMAP hBmp, hOldBmp;	//double buffer
	hMemDC = CreateCompatibleDC(hdc);
	BYTE bmibuf[sizeof(BITMAPINFO)+256 * sizeof(RGBQUAD)];	//create bitmapinfo buffer
	memset(bmibuf, 0, sizeof(bmibuf));
	BITMAPINFO* pbmi = (BITMAPINFO*)bmibuf;
	pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	pbmi->bmiHeader.biWidth = width;
	pbmi->bmiHeader.biHeight = height;
	pbmi->bmiHeader.biPlanes = 1;
	pbmi->bmiHeader.biBitCount = 24;
	pbmi->bmiHeader.biCompression = BI_RGB;
	hBmp = CreateDIBSection(hMemDC, pbmi, DIB_RGB_COLORS, (void**)&pBits, 0, 0); //create device-independent bitmap
	hOldBmp = (HBITMAP)SelectObject(hMemDC, hBmp);	//save the bitmap drawed last time
	BitBlt(hMemDC, 0, 0, width, height, hdc, 0, 0, SRCCOPY);	//output old bitmap to screen
	unsigned char rSrc;
	unsigned char gSrc;
	unsigned char bSrc;
	for (UINT y = 0; y < height; ++y)
	{
		for (UINT x = 0; x < width; ++x)
		{
			pBits[y * width * 3 + x * 3 + 2] = buffer[y * width + x].r;
			pBits[y * width * 3 + x * 3 + 1] = buffer[y * width + x].g;
			pBits[y * width * 3 + x * 3] = buffer[y * width + x].b;
		}
	}
	BitBlt(hdc, 0, 0, width, height, hMemDC, 0, 0, SRCCOPY);	//output new bitmap to memory
	SelectObject(hMemDC, hOldBmp);
	DeleteObject(hBmp);
	DeleteDC(hMemDC);
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
	case WM_COMMAND:
		MessageBox(hwnd, "You select one!", "Tips", 0);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow)
{

	WNDCLASSEX wndClass = { 0 };
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = WndProc;
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.hInstance = hInstance;
	wndClass.hIcon = NULL;
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.lpszMenuName = WINDOW_NAME;
	wndClass.lpszClassName = WINDOW_NAME;

	if (!RegisterClassEx(&wndClass))
	{
		MessageBox(NULL, "This program requires Windows NT!", WINDOW_NAME, MB_ICONERROR);
		return 0;
	}

	HWND hwnd = CreateWindow(
		WINDOW_NAME,							//window class name
		WINDOW_NAME,				//window caption
		WS_OVERLAPPEDWINDOW,		//window style
		CW_USEDEFAULT,						//initial x position
		CW_USEDEFAULT,						//initial y position
		WINDOW_SIZE_X,						//initial x size
		WINDOW_SIZE_Y,						//initial y size
		NULL,										//parent window handle
		NULL,										//window menu handle
		hInstance,									//program instance handle
		NULL);										//creation parameters

	if (!hwnd)
	{
		MessageBox(NULL, "Failed to create window!", WINDOW_NAME, MB_ICONERROR);
		return 0;
	}

	ShowWindow(hwnd, nCmdShow);

	PIXELCOLOR* buffer = new PIXELCOLOR[WINDOW_SIZE_X * WINDOW_SIZE_Y];
	for (int i = 0; i < WINDOW_SIZE_X * WINDOW_SIZE_Y; ++i)
	{
		buffer[i].r = 255;
		buffer[i].g = 0;
		if (i % 10 == 0)
			buffer[i].b = 0;
		else
			buffer[i].b = 255;
	}

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
			auto timeStart = GetTickCount();
			RenderBuffer(hdc, buffer, WINDOW_SIZE_X, WINDOW_SIZE_Y);
			auto timeEnd = GetTickCount();
			auto dtime = 1000 / (timeEnd - timeStart + 1);
			char a[30];
			int len = sprintf_s(a, "%d", dtime);
			TextOut(hdc, 0, 0, a, len);
			ReleaseDC(hwnd, hdc);
		}
	}

	delete[] buffer;
	return msg.wParam;
}