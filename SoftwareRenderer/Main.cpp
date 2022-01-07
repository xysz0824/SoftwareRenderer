#include "SystemWindow.h"
#include "MyBoard.h"

int WINAPI Main()
{
	HWND hwnd = SWCreateWindow(hInstance, "Software Renderer", 800, 600);
	RenderBoard* board = new MyBoard;
	SWSetRenderBoard(board);
	return SWRunWindow(hwnd, nCmdShow);
}