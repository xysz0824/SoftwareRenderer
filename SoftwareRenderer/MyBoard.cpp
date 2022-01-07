#include "SystemWindow.h"
#include "SoftwareRenderer.h"
#include "MyBoard.h"

MyBoard::MyBoard() : _mainCanvas(), _cube(), _texture()
{

}

MyBoard::~MyBoard()
{

}

void MyBoard::Initialize()
{
	int width = SWGetWindowWidth();
	int height = SWGetWindowHeight();
	_mainCanvas = SWCreateCanvas(0, 0, width, height);
	_cube = CreateCube(Vector3{ 1, 0, 0 }, Vector3{ 0, 0, 0 }, 4);
	_texture = LoadBitmap24("lena.bmp");
	SRSetProjection(CreatePerspectiveLH(deg2rad(30), (float)width / (float)height, 1, 1000));
}

void MyBoard::Render(float dt)
{
	static float a;
	SWClearCanvas(_mainCanvas, RGB{ 0, 0, 0 });
	a += dt * 25;
	SRSetView(CreateViewAtLH(Vector3{ 10 * (float)cos(deg2rad(a)), -10 + a * 0.1f, 10 * (float)sin(deg2rad(a)) }, Vector3{ 0, 0, 0 }, Vector3{ 0, 1, 0 }));
	_cube.position = Vector3{ 0.5f, 0.5f, 0.5f };
	SRDrawObject(_mainCanvas, _cube, _texture);
	SRDrawWireFrame(_mainCanvas, _cube, RGB{ 0, 0, 255 });
	_cube.position = Vector3{ -0.5f, -0.5f, -0.5f };
	SRDrawObject(_mainCanvas, _cube, _texture);
	SRDrawWireFrame(_mainCanvas, _cube, RGB{ 0, 255, 0 });
	SWSetCanvas(_mainCanvas);
}