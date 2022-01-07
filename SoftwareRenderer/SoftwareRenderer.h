#ifndef _SOFTWARE_RENDERER_H
#define _SOFTWARE_RENDERER_H
#include "Graphics.h"
#include "MathHelper.h"

void	SRDrawPoint(Canvas canvas, Vector2 position, RGB color);
bool	SRClipLine(Canvas canvas, Vector2* start, Vector2* end);
void	SRDrawLine(Canvas canvas, Vector2 start, Vector2 end, RGB color);
void	SRDrawLine(Canvas canvas, Vector2 start, float angle, float length, RGB color);
void	SRDrawTriangle(Canvas canvas, Vector2 vertex[3], RGB color);
void	SRDrawPolygon(Canvas canvas, Vector2 center, int edge, float radius, float rotation, RGB color);
void	SRDrawCircle(Canvas canvas, Vector2 center, float radius, RGB color);
void	SRSetView(Matrix view);
void	SRSetProjection(Matrix projection);
void	SRDrawLine3D(Canvas canvas, Vector3 start, Vector3 end, RGB color);
void	SRDrawWireFrame(Canvas canvas, Object object, RGB color);
void	SRRasterize(Canvas canvas, Vertex* triangle, Texture texture);
void	SRDrawObject(Canvas canvas, Object object, Texture texture);

#endif