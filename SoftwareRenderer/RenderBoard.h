#ifndef _RENDER_BOARD_H
#define _RENDER_BOARD_H
#include "SoftwareRenderer.h"

class RenderBoard {
public:
	RenderBoard() {}
	virtual ~RenderBoard() {}
	virtual void Initialize() = 0;
	virtual void Render(HDC hdc, float dt) = 0;
private:
	RenderBoard(const RenderBoard&) = delete;
	RenderBoard& operator=(const RenderBoard&) = delete;
};
#endif