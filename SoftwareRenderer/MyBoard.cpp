#include "MyBoard.h"
#include "SoftwareRenderer.h"

MyBoard::MyBoard()
{

}

MyBoard::~MyBoard()
{

}

void MyBoard::Initialize()
{
	Pixel color = { 255, 0, 0 };
	_mainCanvas = SRCreateCanvas(color);
}

void MyBoard::Render(HDC hdc, float dt)
{
	for (float angle = 0; angle < 360; ++angle)
		SRDrawLine(_mainCanvas, Vector2{ 400, 300 }, angle, 100, Pixel{ 255, 255, 255 });
	SRRender(hdc, _mainCanvas);
}