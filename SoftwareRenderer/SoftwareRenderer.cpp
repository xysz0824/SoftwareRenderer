#include <stdio.h>
#include <assert.h>
#include "SystemWindow.h"
#include "SoftwareRenderer.h"

static Matrix		_view;
static Matrix		_projection;

void SRDrawPoint(Canvas canvas, Vector2 position, RGB color)
{
	canvas.buffer[int(position.y) * canvas.w + int(position.x)] = color;
}

bool SRClipLine(Canvas canvas, Vector2* start, Vector2* end)
{
	//Liang-Barsky line clipping
	float u1 = 0, u2 = 1;
	float p[4], q[4];
	float r;
	p[0] = start->x - end->x;
	p[1] = end->x - start->x;
	p[2] = start->y - end->y;
	p[3] = end->y - start->y;

	q[0] = start->x;
	q[1] = (canvas.w - 1) - start->x;
	q[2] = start->y;
	q[3] = (canvas.h - 1) - start->y;

	for (int i = 0; i < 4; ++i)
	{
		r = q[i] / p[i];
		if (p[i] < 0)
		{
			u1 = max(u1, r);
			if (u1 > u2)
				return false;
		}
		else if (p[i] > 0)
		{
			u2 = min(u2, r);
			if (u1 > u2)
				return false;
		}
		else if (p[i] == 0 && q[i] < 0)
			return false;
	}
	Vector2 resultStart, resultEnd;
	resultStart.x = start->x + u1 * (end->x - start->x);
	resultStart.y = start->y + u1 * (end->y - start->y);
	resultEnd.x = start->x + u2 * (end->x - start->x);
	resultEnd.y = start->y + u2 * (end->y - start->y);
	start->x = resultStart.x;
	start->y = resultStart.y;
	end->x = resultEnd.x;
	end->y = resultEnd.y;

	return true;
}

void SRDrawLine(Canvas canvas, Vector2 start, Vector2 end, RGB color)
{
	if (!SRClipLine(canvas, &start, &end))
		return;

	float x = start.x;
	float y = start.y;
	float dx = abs(end.x - start.x);
	float dy = abs(end.y - start.y);
	float stepX = 1;
	if (start.x > end.x) stepX = -1;
	float stepY = 1;
	if (start.y > end.y) stepY = -1;
	//Draw start point
	SRDrawPoint(canvas, start, color);
	//Bresenham line drawing
	if (dy < dx)
	{
		float p = dx - 2 * dy;
		for (float i = 0; i < dx - 1; ++i)
		{
			x += stepX;
			if (p > 0)
				p += -2 * dy;
			else
			{
				y += stepY;
				p += 2 * dx - 2 * dy;
			}
			SRDrawPoint(canvas, Vector2{ x, y }, color);
		}
	}
	else if (dy > dx)
	{
		float p = dy - 2 * dx;
		for (float i = 0; i < dy - 1; ++i)
		{
			y += stepY;
			if (p > 0)
				p += -2 * dx;
			else
			{
				x += stepX;
				p += 2 * dy - 2 * dx;
			}
			SRDrawPoint(canvas, Vector2{ x, y }, color);
		}
	}
	else if (dy == dx)
	{
		for (float i = 0; i < dx - 1; ++i)
		{
			x += stepX;
			y += stepY;
			SRDrawPoint(canvas, Vector2{ x, y }, color);
		}
	}
}

void SRDrawLine(Canvas canvas, Vector2 start, float angle, float length, RGB color)
{
	Vector2 end = { 
		start.x + length * float(cos(angle)), 
		start.y + length * float(sin(angle)) };
	SRDrawLine(canvas, start, end, color);
}

void SRDrawTriangle(Canvas canvas, Vector2 vertex[3], RGB color)
{
	SRDrawLine(canvas, vertex[0], vertex[1], color);
	SRDrawLine(canvas, vertex[1], vertex[2], color);
	SRDrawLine(canvas, vertex[2], vertex[0], color);
}

void SRDrawPolygon(Canvas canvas, Vector2 center, int edge, float radius, float rotation, RGB color)
{
	assert(edge >= 3);

	Vector2 v1, v2;
	for (int i = 0; i < edge; ++i)
	{
		v1.x = center.x + radius * (float)cos(rotation);
		v1.y = center.y + radius * (float)sin(rotation);
		rotation += (float)deg2rad(360.0f / edge);
		v2.x = center.x + radius * (float)cos(rotation);
		v2.y = center.y + radius * (float)sin(rotation);
		SRDrawLine(canvas, v1, v2, color);
	}
}

void SRDrawCircle(Canvas canvas, Vector2 center, float radius, RGB color)
{
	//Bresenham line drawing
	if (center.x + radius < 0 || center.y + radius < 0 ||
		center.x - radius >= canvas.w || center.y - radius >= canvas.h)
		return;

	int x = 0, y = (int)radius;
	int d = 3 - 2 * y;
	while (x <= y)
	{
		Vector2 drawPosition[8] = 
		{ 
			{ center.x + x, center.y + y }, { center.x - x, center.y + y },
			{ center.x + x, center.y - y }, { center.x - x, center.y - y },
			{ center.x + y, center.y + x }, { center.x - y, center.y + x },
			{ center.x + y, center.y - x }, { center.x - y, center.y - x }
		};
		for (int i = 0; i < 8; ++i)
		{
			if (drawPosition[i].x >= 0 && drawPosition[i].y >= 0 &&
				drawPosition[i].x < 0 + canvas.w && drawPosition[i].y < 0 + canvas.h)
			{
				SRDrawPoint(canvas, drawPosition[i], color);
			}
		}
		if (d < 0)
		{
			d = d + 4 * x + 6;
		}
		else
		{
			d = d + 4 * (x - y) + 10;
			y--;
		}
		x++;
	}
}

void SRSetView(Matrix view)
{
	_view = view;
}

void SRSetProjection(Matrix projection)
{
	_projection = projection;
}

void SRDrawLine3D(Canvas canvas, Vector3 start, Vector3 end, RGB color)
{
	int canvasWidth = SWGetWindowWidth();
	int canvasHeight = SWGetWindowHeight();
	Vector4 v4Start = { start.x, start.y, start.z, 1 };
	Vector4 v4End	= { end.x, end.y, end.z, 1 };
	//Transform world space to view space
	v4Start = v4mul(v4Start, _view);
	v4End = v4mul(v4End, _view);
	//Transform view space to projection space
	v4Start = v4mul(v4Start, _projection);
	v4End = v4mul(v4End, _projection);
	//Transform homogeneous coordinates to point coordinates
	v4Start = v4div(v4Start, v4Start.w);
	v4End = v4div(v4End, v4End.w);
	//Transform projection space to canvas space
	Vector2 v2Start = Vector2{ (v4Start.x + 1) / 2.0f * canvasWidth, (v4Start.y + 1) / 2.0f * canvasHeight };
	Vector2 v2End = Vector2{ (v4End.x + 1) / 2.0f * canvasWidth, (v4End.y + 1) / 2.0f * canvasHeight };
	SRDrawLine(canvas, v2Start, v2End, color);
}

void SRDrawWireFrame(Canvas canvas, Object object, RGB color)
{
	int canvasWidth = SWGetWindowWidth();
	int canvasHeight = SWGetWindowHeight();
	Matrix world = CreateScaling(object.scale);
	world = mmul(world, CreateRotateX(object.eulerAngle.x));
	world = mmul(world, CreateRotateX(object.eulerAngle.y));
	world = mmul(world, CreateRotateX(object.eulerAngle.z));
	world = mmul(world, CreateTranslation(object.position));
	for (int i = 0; i < object.totalIndices - 2; i+=3)
	{
		Vector2 points[3];
		for (int j = 0; j < 3; ++j)
		{
			Vector4 current = object.mesh[object.indices[i + j]].position;
			//Transform object space to world space
			current = v4mul(current, world);
			//Transform world space to view space
			current = v4mul(current, _view);
			//Transform view space to projection space
			current = v4mul(current, _projection);
			//Transform homogeneous coordinates to point coordinates
			current = v4div(current, current.w);
			//Transform projection space to canvas space
			points[j] = Vector2{ (current.x + 1) / 2.0f * canvasWidth, (current.y + 1) / 2.0f * canvasHeight };
		}
		if (!IsBackface(points))
		{
			SRDrawTriangle(canvas, points, color);
		}
	}
}

void SRRasterize(Canvas canvas, Vertex* triangle, Texture texture)
{
	int canvasWidth = SWGetWindowWidth();
	int topVertex = triangle[0].position.y < triangle[1].position.y ? 0 : 1;
	topVertex = triangle[topVertex].position.y < triangle[2].position.y ? topVertex : 2;
	int bottomVertex = triangle[0].position.y > triangle[1].position.y ? 0 : 1;
	bottomVertex = triangle[bottomVertex].position.y > triangle[2].position.y ? bottomVertex : 2;
	int middleVertex = 0;
	for (int i = 0; i < 3;++i)
	if (topVertex != i && bottomVertex != i)
	{
		middleVertex = i;
		break;
	}
	Vector4 top = triangle[topVertex].position;
	Vector4 mid = triangle[middleVertex].position;
	Vector4 btm = triangle[bottomVertex].position;
	//Use perspective texture mapping
	for (float y = top.y < 0 ? 0 : top.y; y < (btm.y > canvas.h ? canvas.h : btm.y); ++y)
	{
		float x1, x2;
		float ozTop, ozBtm, oz1, oz2;
		float sozTop, sozBtm, soz1, soz2;
		float tozTop, tozBtm, toz1, toz2;
		x2 = (y - top.y) * (btm.x - top.x) / (btm.y - top.y) + top.x;
		if (y < mid.y)
		{
			x1 = (y - top.y) * (mid.x - top.x) / (mid.y - top.y) + top.x;
			ozTop = 1.0f / top.w;
			ozBtm = 1.0f / mid.w;
			oz1 = (y - top.y) * (ozBtm - ozTop) / (mid.y - top.y) + ozTop;
			sozTop = triangle[topVertex].texcoord.x * ozTop;
			sozBtm = triangle[middleVertex].texcoord.x * ozBtm;
			soz1 = (y - top.y) * (sozBtm - sozTop) / (mid.y - top.y) + sozTop;
			tozTop = triangle[topVertex].texcoord.y * ozTop;
			tozBtm = triangle[middleVertex].texcoord.y * ozBtm;
			toz1 = (y - top.y) * (tozBtm - tozTop) / (mid.y - top.y) + tozTop;
		}
		else
		{
			x1 = (y - mid.y) * (btm.x - mid.x) / (btm.y - mid.y) + mid.x;
			ozTop = 1.0f / mid.w;
			ozBtm = 1.0f / btm.w;
			oz1 = (y - mid.y) * (ozBtm - ozTop) / (btm.y - mid.y) + ozTop;
			sozTop = triangle[middleVertex].texcoord.x * ozTop;
			sozBtm = triangle[bottomVertex].texcoord.x * ozBtm;
			soz1 = (y - mid.y) * (sozBtm - sozTop) / (btm.y - mid.y) + sozTop;
			tozTop = triangle[middleVertex].texcoord.y * ozTop;
			tozBtm = triangle[bottomVertex].texcoord.y * ozBtm;
			toz1 = (y - mid.y) * (tozBtm - tozTop) / (btm.y - mid.y) + tozTop;
		}
		ozTop = 1.0f / top.w;
		ozBtm = 1.0f / btm.w;
		oz2 = (y - top.y) * (ozBtm - ozTop) / (btm.y - top.y) + ozTop;
		sozTop = triangle[topVertex].texcoord.x * ozTop;
		sozBtm = triangle[bottomVertex].texcoord.x * ozBtm;
		soz2 = (y - top.y) * (sozBtm - sozTop) / (btm.y - top.y) + sozTop;
		tozTop = triangle[topVertex].texcoord.y * ozTop;
		tozBtm = triangle[bottomVertex].texcoord.y * ozBtm;
		toz2 = (y - top.y) * (tozBtm - tozTop) / (btm.y - top.y) + tozTop;
		if (x2 == x1)
		{
			float s = soz1 / oz1;
			float t = toz1 / oz1;
			float tx = s * texture.w;
			float ty = t * texture.h;
			int bufferIndex = (int)y * canvas.w + (int)x1;
			int dataIndex = (int)(texture.h - ty - 1) * texture.w + (int)tx;
			if (x1 >= 0 && x1 < canvasWidth && canvas.zBuffer[bufferIndex] < oz1)
			{
				canvas.buffer[bufferIndex] = texture.data[dataIndex];
				canvas.zBuffer[bufferIndex] = oz1;
			}
		}
		else
		{
			float ozstep = (oz2 - oz1) / (x2 - x1);
			float sozstep = (soz2 - soz1) / (x2 - x1);
			float tozstep = (toz2 - toz1) / (x2 - x1);
			float oz = oz1, soz = soz1, toz = toz1;
			for (float i = 0, x = x1;; ++i)
			{
				float s = soz / oz;
				float t = toz / oz;
				float tx = s * texture.w;
				float ty = t * texture.h;
				int bufferIndex = (int)y * canvas.w + (int)x;
				int dataIndex = (int)(texture.h - ty - 1) * texture.w + (int)tx;
				if (x >= 0 && x < canvasWidth && canvas.zBuffer[bufferIndex] < oz)
				{
					canvas.buffer[bufferIndex] = texture.data[dataIndex];
					canvas.zBuffer[bufferIndex] = oz;
				}
				if (x1 < x2)
				{
					x++;
					if (x > x2)
						break;
					oz += ozstep;
					soz += sozstep;
					toz += tozstep;
				}
				else
				{
					x--;
					if (x < x2)
						break;
					oz -= ozstep;
					soz -= sozstep;
					toz -= tozstep;
				}
			}
		}
	}
}

void SRDrawObject(Canvas canvas, Object object, Texture texture)
{
	int canvasWidth = SWGetWindowWidth();
	int canvasHeight = SWGetWindowHeight();
	Matrix world = CreateScaling(object.scale);
	world = mmul(world, CreateRotateX(object.eulerAngle.x));
	world = mmul(world, CreateRotateY(object.eulerAngle.y));
	world = mmul(world, CreateRotateZ(object.eulerAngle.z));
	world = mmul(world, CreateTranslation(object.position));
	for (int i = 0; i < object.totalIndices - 2; i += 3)
	{
		Vector3 points[3];
		Vertex vertices[3];
		for (int j = 0; j < 3; ++j)
		{
			Vector4 current = object.mesh[object.indices[i + j]].position;
			//Transform object space to world space
			current = v4mul(current, world);
			//Transform world space to view space
			current = v4mul(current, _view);
			//Transform view space to projection space
			current = v4mul(current, _projection);
			//Transform homogeneous coordinates to point coordinates
			current.x /= current.w;
			current.y /= current.w;
			current.z /= current.w;
			//Primitive assembly
			points[j] = Vector3{ current.x, current.y, current.z };
			vertices[j].position = Vector4
			{ 
				(current.x + 1) / 2.0f * canvasWidth,
				(current.y + 1) / 2.0f * canvasHeight,
				current.z,
				current.w
			};
			vertices[j].normal = object.mesh[object.indices[i + j]].normal;
			vertices[j].texcoord = object.mesh[object.indices[i + j]].texcoord;
		}
		if (!IsInvisible(points) && !IsBackface(points))
		{
			SRRasterize(canvas, vertices, texture);
		}
	}
}