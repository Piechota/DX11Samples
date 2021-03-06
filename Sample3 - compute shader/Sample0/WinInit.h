#pragma once

#include "GlobalVariables.h"
#include "CommonManagers.h"

bool InitWindowsApp(HINSTANCE instanceHandle, int show)
{
	g_viewMatrix = XMMatrixIdentity();
	Input::viewMatrix = &g_viewMatrix;
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = instanceHandle;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = L"DX1Samples";
	wc.lpfnWndProc = Input::_WndProc;

	if (!RegisterClass(&wc))
	{
		MessageBox(0, L"RegusterClass FAILD", 0, 0);
		return false;
	}

	g_mainWin = CreateWindow(
		L"DX1Samples",
		L"DX1Samples",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		screenWidth,
		screenHeight,
		0,
		0,
		instanceHandle,
		0
		);

	if (g_mainWin == 0)
	{
		MessageBox(0, L"CreateWindow FAILED", 0, 0);
		return false;
	}

	ShowWindow(g_mainWin, show);
	UpdateWindow(g_mainWin);

	return true;
}