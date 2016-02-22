#include "MyBoard.h"

int WINAPI Main()
{
	HWND hwnd = SRCreateWindow(hInstance, "Software Renderer", 800, 600);
	RenderBoard* board = new MyBoard;
	SRSetRenderBoard(board);
	return SRRunWindow(hwnd, nCmdShow);
}