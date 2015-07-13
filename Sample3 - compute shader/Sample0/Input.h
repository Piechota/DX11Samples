#pragma once

#include "Common.h"
#include "CommonMath.h"

class Input
{
private:
	static const float moveSpeed;
	static const float rotateSpeed;
	static XMFLOAT3 camPosition;
	static XMVECTOR camRotation;

	static bool 
		wKey,
		sKey,
		aKey,
		dKey,
		qKey,
		eKey;
public:
	static XMMATRIX* viewMatrix;
	static LRESULT CALLBACK _WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static void Update();
};