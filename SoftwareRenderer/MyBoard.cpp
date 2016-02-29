#include "MyBoard.h"

MyBoard::MyBoard()
{

}

MyBoard::~MyBoard()
{

}

void MyBoard::Initialize()
{
	_mainCanvas = SRCreateCanvas(0, 0, SRGetWindowWidth(), SRGetWindowHeight());
	SRSetProjection(SRCreatePerspectiveLH(deg2rad(45), (float)SRGetWindowWidth() / (float)SRGetWindowHeight(), 0, 1000));
}

void MyBoard::Render(HDC hdc, float dt)
{
	static float a;
	SRClearCanvas(_mainCanvas, RGB{ 0, 0, 0 });
	Vector2 center = { 400, 300 };
	a++;
	SRDrawPolygon(_mainCanvas, center, 10, a, deg2rad(a), RGB{ 255, 255, 255 });
	SRDrawCircle(_mainCanvas, center, a, RGB{ 255, 0, 255 });
	SRSetView(SRCreateViewAtLH(Vector3{ 10 * cos(deg2rad(a)), 0, 10 * sin(deg2rad(a)) }, Vector3{ 0, 0, 0 }, Vector3{ 0, 1, 0 }));
	SRDrawLine3D(_mainCanvas, Vector3{ 0, -1.4f, 2 }, Vector3{ 2, -1.4f, 0 }, RGB{ 255, 255, 255 });
	SRDrawLine3D(_mainCanvas, Vector3{ 2, -1.4f, 0 }, Vector3{ 0, -1.4f, -2 }, RGB{ 255, 255, 255 });
	SRDrawLine3D(_mainCanvas, Vector3{ 0, -1.4f, -2 }, Vector3{ -2, -1.4f, 0 }, RGB{ 255, 255, 255 });
	SRDrawLine3D(_mainCanvas, Vector3{ -2, -1.4f, 0 }, Vector3{ 0, -1.4f, 2 }, RGB{ 255, 255, 255 });
	SRDrawLine3D(_mainCanvas, Vector3{ 0, 1.4f, 2 }, Vector3{ 2, 1.4f, 0 }, RGB{ 255, 255, 255 });
	SRDrawLine3D(_mainCanvas, Vector3{ 2, 1.4f, 0 }, Vector3{ 0, 1.4f, -2 }, RGB{ 255, 255, 255 });
	SRDrawLine3D(_mainCanvas, Vector3{ 0, 1.4f, -2 }, Vector3{ -2, 1.4f, 0 }, RGB{ 255, 255, 255 });
	SRDrawLine3D(_mainCanvas, Vector3{ -2, 1.4f, 0 }, Vector3{ 0, 1.4f, 2 }, RGB{ 255, 255, 255 });
	SRDrawLine3D(_mainCanvas, Vector3{ 0, -1.4f, 2 }, Vector3{ 0, 1.4f, 2 }, RGB{ 255, 255, 255 });
	SRDrawLine3D(_mainCanvas, Vector3{ 2, -1.4f, 0 }, Vector3{ 2, 1.4f, 0 }, RGB{ 255, 255, 255 });
	SRDrawLine3D(_mainCanvas, Vector3{ 0, -1.4f, -2 }, Vector3{ 0, 1.4f, -2 }, RGB{ 255, 255, 255 });
	SRDrawLine3D(_mainCanvas, Vector3{ -2, -1.4f, 0 }, Vector3{ -2, 1.4f, 0 }, RGB{ 255, 255, 255 });
	SRRender(hdc, _mainCanvas);
}