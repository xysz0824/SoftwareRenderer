#include <stdio.h>
#include <assert.h>
#include "SoftwareRenderer.h"
#include "RenderBoard.h"

static bool			_hasWindow;
static int			_width;
static int			_height;
static BYTE*		_bits;
static HDC			_memDC;
static HBITMAP		_bitmap;
static Matrix		_view;
static Matrix		_projection;
static RenderBoard*	_renderBoard = NULL;

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
	wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
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

int	SRGetWindowWidth()
{
	return _width;
}
int	SRGetWindowHeight()
{
	return _height;
}

void SRSetRenderBoard(RenderBoard* renderBoard)
{
	_renderBoard = renderBoard;
}

WPARAM SRRunWindow(HWND hwnd, int nCmdShow)
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
			_renderBoard->Render(hdc, dt);
			if (msec >= 200)
			{
				strLen = sprintf_s(str, "%d", fps * (1000 / msec));
				msec = 0;
				fps = 0;
			}
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

void SRRender(HDC hdc, Canvas canvas)
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
			int bIndex = y * canvas.w + x;
			int pIndex = (_height - (y + canvas.y) - 1) * _width * 3 + (x + canvas.x) * 3;
			if (x + canvas.x < 0 || x + canvas.x >= _width ||
				y + canvas.y < 0 || y + canvas.y >= _height)
				continue;

			_bits[pIndex + 2] = canvas.buffer[bIndex].r;
			_bits[pIndex + 1] = canvas.buffer[bIndex].g;
			_bits[pIndex] = canvas.buffer[bIndex].b;
		}
	}
	//output new bitmap to memory
	BitBlt(hdc, 0, 0, _width, _height, _memDC, 0, 0, SRCCOPY);
}

Canvas SRCreateCanvas(int x, int y, int w, int h)
{
	assert(_hasWindow);

	Pixel* buffer = new Pixel[_width * _height];
	Canvas canvas = { buffer, x, y, w, h };
	return canvas;
}

void SRClearCanvas(Canvas canvas, RGB color)
{
	for (int i = 0; i < canvas.w * canvas.h; ++i)
	{
		canvas.buffer[i].r = color.r;
		canvas.buffer[i].g = color.g;
		canvas.buffer[i].b = color.b;
	}
}

void SRDrawPoint(Canvas canvas, Vector2 position, RGB color)
{
	canvas.buffer[int(position.y) * canvas.w + int(position.x)] = color;
}

bool SRClipLine(Canvas canvas, Vector2* start, Vector2* end)
{
	//Liang-Barsky line clipping
	float u1 = 0, u2 = 1;
	float p[4], q[4];
	float r;
	p[0] = start->x - end->x;
	p[1] = end->x - start->x;
	p[2] = start->y - end->y;
	p[3] = end->y - start->y;

	q[0] = start->x;
	q[1] = (canvas.w - 1) - start->x;
	q[2] = start->y;
	q[3] = (canvas.h - 1) - start->y;

	for (int i = 0; i < 4; ++i)
	{
		r = q[i] / p[i];
		if (p[i] < 0)
		{
			u1 = max(u1, r);
			if (u1 > u2)
				return false;
		}
		else if (p[i] > 0)
		{
			u2 = min(u2, r);
			if (u1 > u2)
				return false;
		}
		else if (p[i] == 0 && q[i] < 0)
			return false;
	}
	Vector2 resultStart, resultEnd;
	resultStart.x = start->x + u1 * (end->x - start->x);
	resultStart.y = start->y + u1 * (end->y - start->y);
	resultEnd.x = start->x + u2 * (end->x - start->x);
	resultEnd.y = start->y + u2 * (end->y - start->y);
	start->x = resultStart.x;
	start->y = resultStart.y;
	end->x = resultEnd.x;
	end->y = resultEnd.y;

	return true;
}

void SRDrawLine(Canvas canvas, Vector2 start, Vector2 end, RGB color)
{
	if (!SRClipLine(canvas, &start, &end))
		return;

	float x = start.x;
	float y = start.y;
	float dx = abs(end.x - start.x);
	float dy = abs(end.y - start.y);
	float stepX = 1;
	if (start.x > end.x) stepX = -1;
	float stepY = 1;
	if (start.y > end.y) stepY = -1;
	//Draw start point
	SRDrawPoint(canvas, start, color);
	//Bresenham line drawing
	if (dy < dx)
	{
		float p = dx - 2 * dy;
		for (float i = 0; i < dx - 1; ++i)
		{
			x += stepX;
			if (p > 0)
				p += -2 * dy;
			else
			{
				y += stepY;
				p += 2 * dx - 2 * dy;
			}
			SRDrawPoint(canvas, Vector2{ x, y }, color);
		}
	}
	else if (dy > dx)
	{
		float p = dy - 2 * dx;
		for (float i = 0; i < dy - 1; ++i)
		{
			y += stepY;
			if (p > 0)
				p += -2 * dx;
			else
			{
				x += stepX;
				p += 2 * dy - 2 * dx;
			}
			SRDrawPoint(canvas, Vector2{ x, y }, color);
		}
	}
	else if (dy == dx)
	{
		for (float i = 0; i < dx - 1; ++i)
		{
			x += stepX;
			y += stepY;
			SRDrawPoint(canvas, Vector2{ x, y }, color);
		}
	}
}

void SRDrawLine(Canvas canvas, Vector2 start, float angle, float length, RGB color)
{
	Vector2 end = { 
		start.x + length * float(cos(deg2rad(angle))), 
		start.y + length * float(sin(deg2rad(angle))) };
	SRDrawLine(canvas, start, end, color);
}

void SRDrawTriangle(Canvas canvas, Vector2 vertex[3], RGB color)
{
	SRDrawLine(canvas, vertex[0], vertex[1], color);
	SRDrawLine(canvas, vertex[1], vertex[2], color);
	SRDrawLine(canvas, vertex[2], vertex[0], color);
}

void SRDrawPolygon(Canvas canvas, Vector2 center, int edge, float radius, float rotation, RGB color)
{
	assert(edge >= 3);

	Vector2 v1, v2;
	for (int i = 0; i < edge; ++i)
	{
		v1.x = center.x + radius * (float)cos(deg2rad(rotation));
		v1.y = center.y + radius * (float)sin(deg2rad(rotation));
		rotation += 360.0f / edge;
		v2.x = center.x + radius * (float)cos(deg2rad(rotation));
		v2.y = center.y + radius * (float)sin(deg2rad(rotation));
		SRDrawLine(canvas, v1, v2, color);
	}
}

void SRDrawCircle(Canvas canvas, Vector2 center, float radius, RGB color)
{
	//Bresenham line drawing
	if (center.x + radius < 0 || center.y + radius < 0 ||
		center.x - radius >= canvas.w || center.y - radius >= canvas.h)
		return;

	int x = 0, y = (int)radius;
	int d = 3 - 2 * y;
	while (x <= y)
	{
		Vector2 drawPosition[8] = 
		{ 
			{ center.x + x, center.y + y }, { center.x - x, center.y + y },
			{ center.x + x, center.y - y }, { center.x - x, center.y - y },
			{ center.x + y, center.y + x }, { center.x - y, center.y + x },
			{ center.x + y, center.y - x }, { center.x - y, center.y - x }
		};
		for (int i = 0; i < 8; ++i)
		{
			if (drawPosition[i].x >= 0 && drawPosition[i].y >= 0 &&
				drawPosition[i].x < 0 + canvas.w && drawPosition[i].y < 0 + canvas.h)
			{
				SRDrawPoint(canvas, drawPosition[i], color);
			}
		}
		if (d < 0)
		{
			d = d + 4 * x + 6;
		}
		else
		{
			d = d + 4 * (x - y) + 10;
			y--;
		}
		x++;
	}
}
Matrix SRCreateTranslation(Vector3 position)
{
	float x = position.x;
	float y = position.y;
	float z = position.z;
	Matrix translation =
	{
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		x, y, z, 1
	};
	return translation;
}

Matrix SRCreateRotateX(float angle)
{
	float rad = (float)deg2rad(angle);
	float c = cos(rad);
	float s = sin(rad);
	Matrix rotate = 
	{
		1, 0, 0, 0,
		0, c, s, 0,
		0,-s, c, 0,
		0, 0, 0, 1
	};
	return rotate;
}

Matrix SRCreateRotateY(float angle)
{
	float rad = (float)deg2rad(angle);
	float c = cos(rad);
	float s = sin(rad);
	Matrix rotate =
	{
		c, 0,-s, 0,
		0, 1, 0, 0,
		s, 0, c, 0,
		0, 0, 0, 1
	};
	return rotate;
}

Matrix SRCreateRotateZ(float angle)
{
	float rad = (float)deg2rad(angle);
	float c = cos(rad);
	float s = sin(rad);
	Matrix rotate =
	{
		c, s, 0, 0,
	   -s, c, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};
	return rotate;
}

Matrix SRCreateScaling(Vector3 scale)
{
	float sx = scale.x;
	float sy = scale.y;
	float sz = scale.z;
	Matrix scaling =
	{
		sx, 0, 0, 0,
		 0,sy, 0, 0,
		 0, 0,sz, 0,
		 0, 0, 0, 1
	};
	return scaling;
}

Matrix SRCreateViewToLH(Vector3 position, Vector3 direction, Vector3 up)
{
	//Calculate d
	Vector3 d = v3normalize(direction);
	//Calculate r
	Vector3 r = v3cross(up, d);
	r = v3normalize(r);
	//Calculate u
	Vector3 u = v3cross(r, d);
	u = v3normalize(u);
	//Fill the view matrix
	float pr = -v3dot(position, r);
	float pu = -v3dot(position, u);
	float pd = -v3dot(position, d);
	Matrix view =
	{
		r.x, u.x, d.x, 0,
		r.y, u.y, d.y, 0,
		r.z, u.z, d.z, 0,
		pr,  pu,  pd,  1
	};
	return view;
}

Matrix SRCreatePerspectiveLH(float fieldofView, float aspectRadio, float zNear, float zFar)
{
	float ys = 1.0f / tan(fieldofView / 2.0f);
	float xs = ys / aspectRadio;
	float za = zFar / (zFar - zNear);
	float zb = (zNear * zFar) / (zNear - zFar);
	Matrix perspective = 
	{
		xs, 0 , 0 , 0 ,
		0 , ys, 0 , 0 ,
		0 , 0 , za, 1 ,
		0 , 0 , zb, 0
	};
	return perspective;
}

Matrix SRCreateViewAtLH(Vector3 position, Vector3 target, Vector3 up)
{
	return SRCreateViewToLH(position, v3sub(target, position), up);
}

void SRSetView(Matrix view)
{
	_view = view;
}

void SRSetProjection(Matrix projection)
{
	_projection = projection;
}