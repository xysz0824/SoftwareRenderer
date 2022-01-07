#include <assert.h>
#include <Windows.h>
#include "Graphics.h"

Matrix CreateTranslation(Vector3 position)
{
	float x = position.x;
	float y = position.y;
	float z = position.z;
	Matrix translation =
	{
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		x, y, z, 1
	};
	return translation;
}

Matrix CreateRotateX(float angle)
{
	float c = (float)cos(angle);
	float s = (float)sin(angle);
	Matrix rotate =
	{
		1, 0, 0, 0,
		0, c, s, 0,
		0,-s, c, 0,
		0, 0, 0, 1
	};
	return rotate;
}

Matrix CreateRotateY(float angle)
{
	float c = (float)cos(angle);
	float s = (float)sin(angle);
	Matrix rotate =
	{
		c, 0,-s, 0,
		0, 1, 0, 0,
		s, 0, c, 0,
		0, 0, 0, 1
	};
	return rotate;
}

Matrix CreateRotateZ(float angle)
{
	float c = (float)cos(angle);
	float s = (float)sin(angle);
	Matrix rotate =
	{
		c, s, 0, 0,
	   -s, c, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};
	return rotate;
}

Matrix CreateScaling(Vector3 scale)
{
	float sx = scale.x;
	float sy = scale.y;
	float sz = scale.z;
	Matrix scaling =
	{
		sx, 0, 0, 0,
		 0,sy, 0, 0,
		 0, 0,sz, 0,
		 0, 0, 0, 1
	};
	return scaling;
}

Matrix CreateViewToLH(Vector3 position, Vector3 direction, Vector3 up)
{
	//Calculate d
	Vector3 d = v3normalize(direction);
	//Calculate r
	Vector3 r = v3cross(up, d);
	r = v3normalize(r);
	//Calculate u
	Vector3 u = v3cross(r, d);
	u = v3normalize(u);
	//Fill the view matrix
	float pr = -v3dot(position, r);
	float pu = -v3dot(position, u);
	float pd = -v3dot(position, d);
	Matrix view =
	{
		r.x, u.x, d.x, 0,
		r.y, u.y, d.y, 0,
		r.z, u.z, d.z, 0,
		pr,  pu,  pd,  1
	};
	return view;
}

Matrix CreateViewAtLH(Vector3 position, Vector3 target, Vector3 up)
{
	return CreateViewToLH(position, v3sub(target, position), up);
}

Matrix CreatePerspectiveLH(float fieldofView, float aspectRadio, float zNear, float zFar)
{
	float ys = 1.0f / (float)tan(fieldofView / 2.0f);
	float xs = ys / aspectRadio;
	float za = zFar / (zFar - zNear);
	float zb = (zNear * zFar) / (zNear - zFar);
	Matrix perspective =
	{
		xs, 0 , 0 , 0 ,
		0 , ys, 0 , 0 ,
		0 , 0 , za, 1 ,
		0 , 0 , zb, 0
	};
	return perspective;
}

Object CreateCube(Vector3 position, Vector3 eulerAngle, float side)
{
	Object cube;
	cube.totalVertex = 24;
	cube.mesh = new Vertex[cube.totalVertex];
	cube.mesh[0] = { Vector4{ -0.5f, -0.5f, -0.5f, 1 }, Vector3{ 0, 0, -1 }, Vector2{ 0, 1 } };
	cube.mesh[1] = { Vector4{ -0.5f, 0.5f, -0.5f, 1 }, Vector3{ 0, 0, -1 }, Vector2{ 0, 0 } };
	cube.mesh[2] = { Vector4{ 0.5f, 0.5f, -0.5f, 1 }, Vector3{ 0, 0, -1 }, Vector2{ 1, 0 } };
	cube.mesh[3] = { Vector4{ 0.5f, -0.5f, -0.5f, 1 }, Vector3{ 0, 0, -1 }, Vector2{ 1, 1 } };
	cube.mesh[4] = { Vector4{ -0.5f, -0.5f, 0.5f, 1 }, Vector3{ -1, 0, 0 }, Vector2{ 0, 1 } };
	cube.mesh[5] = { Vector4{ -0.5f, 0.5f, 0.5f, 1 }, Vector3{ -1, 0, 0 }, Vector2{ 0, 0 } };
	cube.mesh[6] = { Vector4{ -0.5f, 0.5f, -0.5f, 1 }, Vector3{ -1, 0, 0 }, Vector2{ 1, 0 } };
	cube.mesh[7] = { Vector4{ -0.5f, -0.5f, -0.5f, 1 }, Vector3{ -1, 0, 0 }, Vector2{ 1, 1 } };
	cube.mesh[8] = { Vector4{ 0.5f, -0.5f, 0.5f, 1 }, Vector3{ 0, 0, 1 }, Vector2{ 0, 1 } };
	cube.mesh[9] = { Vector4{ 0.5f, 0.5f, 0.5f, 1 }, Vector3{ 0, 0, 1 }, Vector2{ 0, 0 } };
	cube.mesh[10] = { Vector4{ -0.5f, 0.5f, 0.5f, 1 }, Vector3{ 0, 0, 1 }, Vector2{ 1, 0 } };
	cube.mesh[11] = { Vector4{ -0.5f, -0.5f, 0.5f, 1 }, Vector3{ 0, 0, 1 }, Vector2{ 1, 1 } };
	cube.mesh[12] = { Vector4{ 0.5f, -0.5f, -0.5f, 1 }, Vector3{ 1, 0, 0 }, Vector2{ 0, 1 } };
	cube.mesh[13] = { Vector4{ 0.5f, 0.5f, -0.5f, 1 }, Vector3{ 1, 0, 0 }, Vector2{ 0, 0 } };
	cube.mesh[14] = { Vector4{ 0.5f, 0.5f, 0.5f, 1 }, Vector3{ 1, 0, 0 }, Vector2{ 1, 0 } };
	cube.mesh[15] = { Vector4{ 0.5f, -0.5f, 0.5f, 1 }, Vector3{ 1, 0, 0 }, Vector2{ 1, 1 } };
	cube.mesh[16] = { Vector4{ -0.5f, 0.5f, -0.5f, 1 }, Vector3{ 0, 1, 0 }, Vector2{ 0, 1 } };
	cube.mesh[17] = { Vector4{ -0.5f, 0.5f, 0.5f, 1 }, Vector3{ 0, 1, 0 }, Vector2{ 0, 0 } };
	cube.mesh[18] = { Vector4{ 0.5f, 0.5f, 0.5f, 1 }, Vector3{ 0, 1, 0 }, Vector2{ 1, 0 } };
	cube.mesh[19] = { Vector4{ 0.5f, 0.5f, -0.5f, 1 }, Vector3{ 0, 1, 0 }, Vector2{ 1, 1 } };
	cube.mesh[20] = { Vector4{ -0.5f, -0.5f, 0.5f, 1 }, Vector3{ 0, -1, 0 }, Vector2{ 0, 1 } };
	cube.mesh[21] = { Vector4{ -0.5f, -0.5f, -0.5f, 1 }, Vector3{ 0, -1, 0 }, Vector2{ 0, 0 } };
	cube.mesh[22] = { Vector4{ 0.5f, -0.5f, -0.5f, 1 }, Vector3{ 0, -1, 0 }, Vector2{ 1, 0 } };
	cube.mesh[23] = { Vector4{ 0.5f, -0.5f, 0.5f, 1 }, Vector3{ 0, -1, 0 }, Vector2{ 1, 1 } };
	cube.totalIndices = 36;
	cube.indices = new int[cube.totalIndices]
	{
		0, 1, 2, 0, 2, 3,
		4, 5, 6, 4, 6, 7,
		8, 9, 10, 8, 10, 11,
		12, 13, 14, 12, 14, 15,
		16, 17, 18, 16, 18, 19,
		20, 21, 22, 20, 22, 23
	};
	cube.position = position;
	cube.eulerAngle = eulerAngle;
	cube.scale = Vector3{ side, side, side };
	return cube;
}

#pragma pack(1)
Texture	LoadBitmap24(const char* path)
{
	Texture tex;
	tex.data = NULL;
	FILE* file;
	fopen_s(&file, path, "rb");
	assert(file);
	struct BITMAPFILEHEADER
	{
		WORD bfType;
		DWORD bfSize;
		WORD bfReserved1;
		WORD bfReserved2;
		DWORD bfOffBits;
	} fileHeader;
	fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1, file);
	struct BITMAPINFOHEADER {
		DWORD biSize;
		LONG biWidth;
		LONG biHeight;
		WORD biPlanes;
		WORD biBitCount;
		DWORD biCompression;
		DWORD biSizeImage;
		LONG biXPelsPerMeter;
		LONG biYPelsPerMeter;
		DWORD biClrUsed;
		DWORD biClrImportant;
	} infoHeader;
	fread(&infoHeader, sizeof(BITMAPINFOHEADER), 1, file);
	tex.w = infoHeader.biWidth;
	tex.h = infoHeader.biHeight;
	tex.data = new Pixel[tex.w * tex.h];
	for (int i = 0; i < tex.w * tex.h; ++i)
	{
		fread(&tex.data[i], sizeof(Pixel), 1, file);
		byte temp = tex.data[i].b;
		tex.data[i].b = tex.data[i].r;
		tex.data[i].r = temp;
	}
	fclose(file);
	return tex;
}
#pragma pack()

bool IsInvisible(Vector3* points)
{
	int outofZ = 0;
	for (int i = 0; i < 3; ++i)
	{
		if (points[i].z <= 0 || points[i].z >= 1)
		{
			outofZ++;
		}
	}
	if (outofZ == 3)
	{
		return true;
	}
	for (int i = 0; i < 3; ++i)
	{
		if (points[i].x > -1 && points[i].x < 1 &&
			points[i].y > -1 && points[i].y < 1)
		{
			return false;
		}
		float u = points[(i + 1) % 3].x - points[i].x;
		float v = points[(i + 1) % 3].y - points[i].y;
		//Top edge
		float p = (-1 - points[i].y) * u / v + points[i].x;
		if (p > -1 && p < 1)
		{
			return false;
		}
		//Bottom edge
		p = (1 - points[i].y) * u / v + points[i].x;
		if (p > -1 && p < 1)
		{
			return false;
		}
		//Left edge
		p = (-1 - points[i].x) * v / u + points[i].y;
		if (p > -1 && p < 1)
		{
			return false;
		}
		//Right edge
		p = (1 - points[i].x) * v / u + points[i].y;
		if (p > -1 && p < 1)
		{
			return false;
		}
	}
	return true;
}

bool IsBackface(Vector2* points)
{
	//Use z value of cross product between first and second vector
	Vector2 first = { points[1].x - points[0].x, points[1].y - points[0].y };
	Vector2 second = { points[2].x - points[1].x, points[2].y - points[1].y };
	return first.x * second.y < first.y* second.x;
}

bool IsBackface(Vector3* points)
{
	//Use z value of cross product between first and second vector
	Vector2 first = { points[1].x - points[0].x, points[1].y - points[0].y };
	Vector2 second = { points[2].x - points[1].x, points[2].y - points[1].y };
	return first.x * second.y < first.y* second.x;
}