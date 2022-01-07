#ifndef _MY_BOARD_H
#define _MY_BOARD_H
#include "Graphics.h"
#include "RenderBoard.h"

class MyBoard : public RenderBoard {
public:
	MyBoard();
	virtual ~MyBoard();
	virtual void Initialize();
	virtual void Render(float dt);
private:
	Canvas	_mainCanvas;
	Object	_cube;
	Texture _texture;
};
#endif