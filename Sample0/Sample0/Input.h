#pragma once

#include "CommonWindows.h"

class Input
{
public:
	static LRESULT CALLBACK _WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};