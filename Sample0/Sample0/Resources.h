#pragma once
#include "CommonMath.h"

struct VertexPosColor
{
	XMFLOAT3 position;
	XMFLOAT2 texCoord;
};

VertexPosColor g_Vertices[8] =
{
	{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(0.f, 1.f) }, // 0
	{ XMFLOAT3(-1.0f, +1.0f, -1.0f), XMFLOAT2(0.f, 0.f) }, // 1
	{ XMFLOAT3(+1.0f, +1.0f, -1.0f), XMFLOAT2(1.f, 0.f) }, // 2
	{ XMFLOAT3(+1.0f, -1.0f, -1.0f), XMFLOAT2(1.f, 1.f) }, // 3
	{ XMFLOAT3(-1.0f, -1.0f, +1.0f), XMFLOAT2(1.f, 1.f) }, // 4
	{ XMFLOAT3(-1.0f, +1.0f, +1.0f), XMFLOAT2(1.f, 0.f) }, // 5
	{ XMFLOAT3(+1.0f, +1.0f, +1.0f), XMFLOAT2(0.f, 0.f) }, // 6
	{ XMFLOAT3(+1.0f, -1.0f, +1.0f), XMFLOAT2(0.f, 1.f) }  // 7
};

WORD g_Indicies[36] =
{
	0, 1, 2, 0, 2, 3,
	4, 6, 5, 4, 7, 6,
	4, 5, 1, 4, 1, 0,
	3, 2, 6, 3, 6, 7,
	1, 5, 6, 1, 6, 2,
	4, 0, 3, 4, 3, 7
};
