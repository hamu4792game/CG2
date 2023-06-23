#pragma once
#include "Vector2.h"

struct Vector4
{
	float x, y, z, w;
};

struct VertexData {
	Vector4 position;
	Vector2 texcoord;
};