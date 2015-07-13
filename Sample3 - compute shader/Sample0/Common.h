#pragma once

#include <Windows.h>
#include <windowsx.h>
#include <assert.h>
#include <string>
#include <ctime>

static const int screenWidth = 1280;
static const int screenHeight = 720;
static const bool mEnable4xMsaa = true;

extern HWND g_mainWin;


template<typename T>
inline void SafeRelease(T& ptr)
{
	if (ptr != NULL)
	{
		ptr->Release();
		ptr = NULL;
	}
}