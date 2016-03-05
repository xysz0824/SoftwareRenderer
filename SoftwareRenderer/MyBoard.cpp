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
	SRSetTexture(SRLoadBitmap24("lena.bmp"));
	SRSetProjection(SRCreatePerspectiveLH(deg2rad(30), (float)SRGetWindowWidth() / (float)SRGetWindowHeight(), 10, 1000));
}

void MyBoard::Render(HDC hdc, float dt)
{
	static float a;
	SRClearCanvas(_mainCanvas, RGB{ 0, 0, 0 });
	a += 0.5f;
	SRSetView(SRCreateViewAtLH(Vector3{ 10 * cos(deg2rad(a)), 5, 10 * sin(deg2rad(a)) }, Vector3{ 0, 0, 0 }, Vector3{ 0, 1, 0 }));
	SRDrawObject(_mainCanvas, _cube);
	//SRDrawWireFrame(_mainCanvas, _cube, RGB{ 0, 255, 0 });
	SRRender(hdc, _mainCanvas);
}