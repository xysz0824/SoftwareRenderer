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
}

void MyBoard::Render(HDC hdc, float dt)
{
	static float a;
	SRClearCanvas(_mainCanvas, RGB{ 0, 0, 0 });
	Vector2 center = { 400, 300 };
	a++;
	SRDrawPolygon(_mainCanvas, center, 10, a, a, RGB{ 255, 255, 255 });
	SRDrawCircle(_mainCanvas, center, a, RGB{ 255, 0, 255 });
	SRRender(hdc, _mainCanvas);
}