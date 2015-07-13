#pragma once
#include "CommonMath.h"

struct Particle
{
	XMFLOAT3 velocity;
	XMFLOAT3 accelerate;
};

struct ConstBuffer
{
	XMMATRIX matrix;
	float time;
};

const int particleCount = 16 * 16;
