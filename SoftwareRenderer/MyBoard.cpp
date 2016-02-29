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
	_cube = SRCreateCube(Vector3{ 0, 0, 0 }, Vector3{ 0, 0, 0 }, 3);
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
	SRSetView(SRCreateViewAtLH(Vector3{ 10 * cos(deg2rad(a)), 2, 10 * sin(deg2rad(a)) }, Vector3{ 0, 0, 0 }, Vector3{ 0, 1, 0 }));
	SRDrawWireFrame(_mainCanvas, _cube, RGB{ 0, 255, 0 });
	SRRender(hdc, _mainCanvas);
}