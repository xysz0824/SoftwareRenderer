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
	Matrix a = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
	Matrix b = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
	Matrix c = madd(a, b);
	Matrix d = msub(a, b);
	Matrix e = mmul(a, b);
}

void MyBoard::Render(HDC hdc, float dt)
{
	static float a;
	SRClearCanvas(_mainCanvas, RGB{ 0, 0, 0 });
	Vector2 center = { 400, 300 };
	a++;
	SRDrawPolygon(_mainCanvas, center, 10, a, a, RGB{ 255, 255, 255 });
	SRDrawCircle(_mainCanvas, center, 200, RGB{ 255, 0, 255 });
	SRRender(hdc, _mainCanvas);
}