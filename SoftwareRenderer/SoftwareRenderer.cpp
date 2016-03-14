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
static Texture		_texture;
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
	float* zBuffer = new float[_width * _height];
	Canvas canvas = { buffer, zBuffer, x, y, w, h };
	return canvas;
}

void SRClearCanvas(Canvas canvas, RGB color)
{
	for (int i = 0; i < canvas.w * canvas.h; ++i)
	{
		canvas.buffer[i].r = color.r;
		canvas.buffer[i].g = color.g;
		canvas.buffer[i].b = color.b;
		canvas.zBuffer[i] = 0;
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
		start.x + length * float(cos(angle)), 
		start.y + length * float(sin(angle)) };
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
		v1.x = center.x + radius * (float)cos(rotation);
		v1.y = center.y + radius * (float)sin(rotation);
		rotation += (float)deg2rad(360.0f / edge);
		v2.x = center.x + radius * (float)cos(rotation);
		v2.y = center.y + radius * (float)sin(rotation);
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
	float c = cos(angle);
	float s = sin(angle);
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
	float c = cos(angle);
	float s = sin(angle);
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
	float c = cos(angle);
	float s = sin(angle);
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

Matrix SRCreateViewAtLH(Vector3 position, Vector3 target, Vector3 up)
{
	return SRCreateViewToLH(position, v3sub(target, position), up);
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

void SRSetView(Matrix view)
{
	_view = view;
}

void SRSetProjection(Matrix projection)
{
	_projection = projection;
}

void SRDrawLine3D(Canvas canvas, Vector3 start, Vector3 end, RGB color)
{
	Vector4 v4Start = { start.x, start.y, start.z, 1 };
	Vector4 v4End	= { end.x, end.y, end.z, 1 };
	//Transform world space to view space
	v4Start = v4mul(v4Start, _view);
	v4End = v4mul(v4End, _view);
	//Transform view space to projection space
	v4Start = v4mul(v4Start, _projection);
	v4End = v4mul(v4End, _projection);
	//Transform homogeneous coordinates to point coordinates
	v4Start = v4div(v4Start, v4Start.w);
	v4End = v4div(v4End, v4End.w);
	//Transform projection space to canvas space
	Vector2 v2Start = Vector2{ (v4Start.x + 1) / 2.0f * _width, (v4Start.y + 1) / 2.0f * _height };
	Vector2 v2End = Vector2{ (v4End.x + 1) / 2.0f * _width, (v4End.y + 1) / 2.0f * _height };
	SRDrawLine(canvas, v2Start, v2End, color);
}

Object SRCreateCube(Vector3 position, Vector3 eulerAngle, float side)
{
	Object cube;
	cube.totalVertex = 24;
	cube.mesh = new Vertex[cube.totalVertex];
	cube.mesh[0] =	{ Vector3{ -0.5f, -0.5f, -0.5f }, Vector3{ 0, 0, -1 }, Vector2{ 0, 1 } };
	cube.mesh[1] =	{ Vector3{ -0.5f, 0.5f, -0.5f }, Vector3{ 0, 0, -1 }, Vector2{ 0, 0 } };
	cube.mesh[2] =	{ Vector3{ 0.5f, 0.5f, -0.5f }, Vector3{ 0, 0, -1 }, Vector2{ 1, 0 } };
	cube.mesh[3] =	{ Vector3{ 0.5f, -0.5f, -0.5f }, Vector3{ 0, 0, -1 }, Vector2{ 1, 1 } };
	cube.mesh[4] =	{ Vector3{ -0.5f, -0.5f, 0.5f }, Vector3{ -1, 0, 0 }, Vector2{ 0, 1 } };
	cube.mesh[5] =	{ Vector3{ -0.5f, 0.5f, 0.5f }, Vector3{ -1, 0, 0 }, Vector2{ 0, 0 } };
	cube.mesh[6] =	{ Vector3{ -0.5f, 0.5f, -0.5f }, Vector3{ -1, 0, 0 }, Vector2{ 1, 0 } };
	cube.mesh[7] =	{ Vector3{ -0.5f, -0.5f, -0.5f }, Vector3{ -1, 0, 0 }, Vector2{ 1, 1 } };
	cube.mesh[8] =	{ Vector3{ 0.5f, -0.5f, 0.5f }, Vector3{ 0, 0, 1 }, Vector2{ 0, 1 } };
	cube.mesh[9] =	{ Vector3{ 0.5f, 0.5f, 0.5f }, Vector3{ 0, 0, 1 }, Vector2{ 0, 0 } };
	cube.mesh[10] = { Vector3{ -0.5f, 0.5f, 0.5f }, Vector3{ 0, 0, 1 }, Vector2{ 1, 0 } };
	cube.mesh[11] = { Vector3{ -0.5f, -0.5f, 0.5f }, Vector3{ 0, 0, 1 }, Vector2{ 1, 1 } };
	cube.mesh[12] = { Vector3{ 0.5f, -0.5f, -0.5f }, Vector3{ 1, 0, 0 }, Vector2{ 0, 1 } };
	cube.mesh[13] = { Vector3{ 0.5f, 0.5f, -0.5f }, Vector3{ 1, 0, 0 }, Vector2{ 0, 0 } };
	cube.mesh[14] = { Vector3{ 0.5f, 0.5f, 0.5f }, Vector3{ 1, 0, 0 }, Vector2{ 1, 0 } };
	cube.mesh[15] = { Vector3{ 0.5f, -0.5f, 0.5f }, Vector3{ 1, 0, 0 }, Vector2{ 1, 1 } };
	cube.mesh[16] = { Vector3{ -0.5f, 0.5f, -0.5f }, Vector3{ 0, 1, 0 }, Vector2{ 0, 1 } };
	cube.mesh[17] = { Vector3{ -0.5f, 0.5f, 0.5f }, Vector3{ 0, 1, 0 }, Vector2{ 0, 0 } };
	cube.mesh[18] = { Vector3{ 0.5f, 0.5f, 0.5f }, Vector3{ 0, 1, 0 }, Vector2{ 1, 0 } };
	cube.mesh[19] = { Vector3{ 0.5f, 0.5f, -0.5f }, Vector3{ 0, 1, 0 }, Vector2{ 1, 1 } };
	cube.mesh[20] = { Vector3{ -0.5f, -0.5f, 0.5f }, Vector3{ 0, -1, 0 }, Vector2{ 0, 1 } };
	cube.mesh[21] = { Vector3{ -0.5f, -0.5f, -0.5f }, Vector3{ 0, -1, 0 }, Vector2{ 0, 0 } };
	cube.mesh[22] = { Vector3{ 0.5f, -0.5f, -0.5f }, Vector3{ 0, -1, 0 }, Vector2{ 1, 0 } };
	cube.mesh[23] = { Vector3{ 0.5f, -0.5f, 0.5f }, Vector3{ 0, -1, 0 }, Vector2{ 1, 1 } };
	cube.totalIndices = 36;
	cube.indices = new int[cube.totalIndices]
	{
		0, 1, 2, 0, 2, 3,
		4, 5, 6, 4, 6, 7,
		8, 9, 10, 8, 10, 11,
		12, 13, 14, 12, 14, 15,
		16, 17, 18, 16, 18, 19,
		20, 21, 22, 20, 22, 23
	};
	cube.position = position;
	cube.eulerAngle = eulerAngle;
	cube.scale = Vector3{ side, side, side };
	return cube;
}

void SRDrawWireFrame(Canvas canvas, Object object, RGB color)
{
	Matrix world = SRCreateScaling(object.scale);
	world = mmul(world, SRCreateRotateX(object.eulerAngle.x));
	world = mmul(world, SRCreateRotateX(object.eulerAngle.y));
	world = mmul(world, SRCreateRotateX(object.eulerAngle.z));
	world = mmul(world, SRCreateTranslation(object.position));
	for (int i = 0; i < object.totalIndices - 2; i+=3)
	{
		Vector2 points[3];
		for (int j = 0; j < 3; ++j)
		{
			Vector3 current = object.mesh[object.indices[i + j]].position;
			Vector4 v4Current = { current.x, current.y, current.z, 1 };
			//Transform object space to world space
			v4Current = v4mul(v4Current, world);
			//Transform world space to view space
			v4Current = v4mul(v4Current, _view);
			//Transform view space to projection space
			v4Current = v4mul(v4Current, _projection);
			//Transform homogeneous coordinates to point coordinates
			v4Current = v4div(v4Current, v4Current.w);
			//Transform projection space to canvas space
			points[j] = Vector2{ (v4Current.x + 1) / 2.0f * _width, (v4Current.y + 1) / 2.0f * _height};
		}
		if (!SRIsBackface(points))
		{
			SRDrawTriangle(canvas, points, color);
		}
	}
}

#pragma pack(1)
Texture	SRLoadBitmap24(const char* path)
{
	Texture tex;
	tex.data = NULL;
	FILE* file;
	fopen_s(&file, path, "rb");
	assert(file);
	struct BITMAPFILEHEADER
	{
		WORD bfType;
		DWORD bfSize;
		WORD bfReserved1;
		WORD bfReserved2;
		DWORD bfOffBits;
	} fileHeader;
	fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1, file);
	struct BITMAPINFOHEADER{
		DWORD biSize;
		LONG biWidth;
		LONG biHeight;
		WORD biPlanes;
		WORD biBitCount;
		DWORD biCompression;
		DWORD biSizeImage;
		LONG biXPelsPerMeter;
		LONG biYPelsPerMeter;
		DWORD biClrUsed;
		DWORD biClrImportant;
	} infoHeader;
	fread(&infoHeader, sizeof(BITMAPINFOHEADER), 1, file);
	tex.w = infoHeader.biWidth;
	tex.h = infoHeader.biHeight;
	tex.data = new Pixel[tex.w * tex.h];
	for (int i = 0; i < tex.w * tex.h; ++i)
	{
		fread(&tex.data[i], sizeof(Pixel), 1, file);
		byte temp = tex.data[i].b;
		tex.data[i].b = tex.data[i].r;
		tex.data[i].r = temp;
	}
	fclose(file);
	return tex;
}
#pragma pack()

void SRSetTexture(Texture texture)
{
	_texture = texture;
}

bool SRIsInvisible(Vector3* points)
{
	int outofZ = 0;
	for (int i = 0; i < 3; ++i)
	{
		if (points[i].z <= 0 || points[i].z >= 1)
		{
			outofZ++;
		}
	}
	if (outofZ == 3)
	{
		return true;
	}
	for (int i = 0; i < 3; ++i)
	{
		if (points[i].x > -1 && points[i].x < 1 && 
			points[i].y > -1 && points[i].y < 1)
		{
			return false;
		}
		float u = points[(i + 1) % 3].x - points[i].x;
		float v = points[(i + 1) % 3].y - points[i].y;
		//Top edge
		float p = (-1 - points[i].y) * u / v + points[i].x;
		if (p > -1 && p < 1)
		{
			return false;
		}
		//Bottom edge
		p = (1 - points[i].y) * u / v + points[i].x;
		if (p > -1 && p < 1)
		{
			return false;
		}
		//Left edge
		p = (-1 - points[i].x) * v / u + points[i].y;
		if (p > -1 && p < 1)
		{
			return false;
		}
		//Right edge
		p = (1 - points[i].x) * v / u + points[i].y;
		if (p > -1 && p < 1)
		{
			return false;
		}
	}
	return true;
}

bool SRIsBackface(Vector2* points)
{
	//Use z value of cross product between first and second vector
	Vector2 first	= { points[1].x - points[0].x, points[1].y - points[0].y };
	Vector2 second	= { points[2].x - points[1].x, points[2].y - points[1].y };
	return first.x * second.y < first.y * second.x;
}

bool SRIsBackface(Vector3* points)
{
	//Use z value of cross product between first and second vector
	Vector2 first = { points[1].x - points[0].x, points[1].y - points[0].y };
	Vector2 second = { points[2].x - points[1].x, points[2].y - points[1].y };
	return first.x * second.y < first.y * second.x;
}

void SRRasterize(Canvas canvas, Vertex4* triangle)
{
	int topVertex = triangle[0].position.y < triangle[1].position.y ? 0 : 1;
	topVertex = triangle[topVertex].position.y < triangle[2].position.y ? topVertex : 2;
	int bottomVertex = triangle[0].position.y > triangle[1].position.y ? 0 : 1;
	bottomVertex = triangle[bottomVertex].position.y > triangle[2].position.y ? bottomVertex : 2;
	int middleVertex = 0;
	for (int i = 0; i < 3;++i)
	if (topVertex != i && bottomVertex != i)
	{
		middleVertex = i;
		break;
	}
	Vector4 top = triangle[topVertex].position;
	Vector4 mid = triangle[middleVertex].position;
	Vector4 btm = triangle[bottomVertex].position;
	//Use perspective texture mapping
	for (float y = top.y < 0 ? 0 : top.y; y < (btm.y > canvas.h ? canvas.h : btm.y); ++y)
	{
		float x1, x2;
		float ozTop, ozBtm, oz1, oz2;
		float sozTop, sozBtm, soz1, soz2;
		float tozTop, tozBtm, toz1, toz2;
		x2 = (y - top.y) * (btm.x - top.x) / (btm.y - top.y) + top.x;
		if (y < mid.y)
		{
			x1 = (y - top.y) * (mid.x - top.x) / (mid.y - top.y) + top.x;
			ozTop = 1.0f / top.w;
			ozBtm = 1.0f / mid.w;
			oz1 = (y - top.y) * (ozBtm - ozTop) / (mid.y - top.y) + ozTop;
			sozTop = triangle[topVertex].texcoord.x * ozTop;
			sozBtm = triangle[middleVertex].texcoord.x * ozBtm;
			soz1 = (y - top.y) * (sozBtm - sozTop) / (mid.y - top.y) + sozTop;
			tozTop = triangle[topVertex].texcoord.y * ozTop;
			tozBtm = triangle[middleVertex].texcoord.y * ozBtm;
			toz1 = (y - top.y) * (tozBtm - tozTop) / (mid.y - top.y) + tozTop;
		}
		else
		{
			x1 = (y - mid.y) * (btm.x - mid.x) / (btm.y - mid.y) + mid.x;
			ozTop = 1.0f / mid.w;
			ozBtm = 1.0f / btm.w;
			oz1 = (y - mid.y) * (ozBtm - ozTop) / (btm.y - mid.y) + ozTop;
			sozTop = triangle[middleVertex].texcoord.x * ozTop;
			sozBtm = triangle[bottomVertex].texcoord.x * ozBtm;
			soz1 = (y - mid.y) * (sozBtm - sozTop) / (btm.y - mid.y) + sozTop;
			tozTop = triangle[middleVertex].texcoord.y * ozTop;
			tozBtm = triangle[bottomVertex].texcoord.y * ozBtm;
			toz1 = (y - mid.y) * (tozBtm - tozTop) / (btm.y - mid.y) + tozTop;
		}
		ozTop = 1.0f / top.w;
		ozBtm = 1.0f / btm.w;
		oz2 = (y - top.y) * (ozBtm - ozTop) / (btm.y - top.y) + ozTop;
		sozTop = triangle[topVertex].texcoord.x * ozTop;
		sozBtm = triangle[bottomVertex].texcoord.x * ozBtm;
		soz2 = (y - top.y) * (sozBtm - sozTop) / (btm.y - top.y) + sozTop;
		tozTop = triangle[topVertex].texcoord.y * ozTop;
		tozBtm = triangle[bottomVertex].texcoord.y * ozBtm;
		toz2 = (y - top.y) * (tozBtm - tozTop) / (btm.y - top.y) + tozTop;
		if (x2 == x1)
		{
			float s = soz1 / oz1;
			float t = toz1 / oz1;
			float tx = s * _texture.w;
			float ty = t * _texture.h;
			int bufferIndex = (int)y * canvas.w + (int)x1;
			int dataIndex = (int)(_texture.h - ty - 1) * _texture.w + (int)tx;
			if (x1 >= 0 && x1 < _width && canvas.zBuffer[bufferIndex] < oz1)
			{
				canvas.buffer[bufferIndex] = _texture.data[dataIndex];
				canvas.zBuffer[bufferIndex] = oz1;
			}
		}
		else
		{
			float ozstep = (oz2 - oz1) / (x2 - x1);
			float sozstep = (soz2 - soz1) / (x2 - x1);
			float tozstep = (toz2 - toz1) / (x2 - x1);
			float oz = oz1, soz = soz1, toz = toz1;
			for (float i = 0, x = x1;; ++i)
			{
				float s = soz / oz;
				float t = toz / oz;
				float tx = s * _texture.w;
				float ty = t * _texture.h;
				int bufferIndex = (int)y * canvas.w + (int)x;
				int dataIndex = (int)(_texture.h - ty - 1) * _texture.w + (int)tx;
				if (x >= 0 && x < _width && canvas.zBuffer[bufferIndex] < oz)
				{
					canvas.buffer[bufferIndex] = _texture.data[dataIndex];
					canvas.zBuffer[bufferIndex] = oz;
				}
				if (x1 < x2)
				{
					x++;
					if (x > x2)
						break;
					oz += ozstep;
					soz += sozstep;
					toz += tozstep;
				}
				else
				{
					x--;
					if (x < x2)
						break;
					oz -= ozstep;
					soz -= sozstep;
					toz -= tozstep;
				}
			}
		}
	}
}

void SRDrawObject(Canvas canvas, Object object)
{
	Matrix world = SRCreateScaling(object.scale);
	world = mmul(world, SRCreateRotateX(object.eulerAngle.x));
	world = mmul(world, SRCreateRotateY(object.eulerAngle.y));
	world = mmul(world, SRCreateRotateZ(object.eulerAngle.z));
	world = mmul(world, SRCreateTranslation(object.position));
	for (int i = 0; i < object.totalIndices - 2; i += 3)
	{
		Vector3 points[3];
		Vertex4 vertices[3];
		for (int j = 0; j < 3; ++j)
		{
			Vector3 current = object.mesh[object.indices[i + j]].position;
			Vector4 v4Current = { current.x, current.y, current.z, 1 };
			//Transform object space to world space
			v4Current = v4mul(v4Current, world);
			//Transform world space to view space
			v4Current = v4mul(v4Current, _view);
			//Transform view space to projection space
			v4Current = v4mul(v4Current, _projection);
			//Transform homogeneous coordinates to point coordinates
			v4Current.x /= v4Current.w;
			v4Current.y /= v4Current.w;
			v4Current.z /= v4Current.w;
			//Primitive assembly
			points[j] = Vector3{ v4Current.x, v4Current.y, v4Current.z };
			vertices[j].position = Vector4
			{ 
				(v4Current.x + 1) / 2.0f * _width,
				(v4Current.y + 1) / 2.0f * _height,
				v4Current.z,
				v4Current.w
			};
			vertices[j].normal = object.mesh[object.indices[i + j]].normal;
			vertices[j].texcoord = object.mesh[object.indices[i + j]].texcoord;
		}
		if (!SRIsInvisible(points) && !SRIsBackface(points))
		{
			SRRasterize(canvas, vertices);
		}
	}
}

void SRDrawBitmap(Canvas canvas, Texture texture)
{
	for (int x = 0; x < texture.w; ++x)
	{
		for (int y = 0; y < texture.h; ++y)
		{
			canvas.buffer[y * canvas.w + x] = texture.data[(texture.h - y - 1) * texture.w + x];
		}
	}
}