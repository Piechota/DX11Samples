#include "CommonWindows.h"
#include "Input.h"

HWND mainWin;

bool InitWindowsApp(HINSTANCE instanceHandle, int show)
{
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

	mainWin = CreateWindow( 
		L"DX1Samples",
		L"DX1Samples",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		0,
		0,
		instanceHandle,
		0
		);

	if (mainWin == 0)
	{
		MessageBox(0, L"CreateWindow FAILED", 0, 0);
		return false;
	}

	ShowWindow(mainWin, show);
	UpdateWindow(mainWin);

	return true;
}

int Run()
{
	MSG msg = { 0 };

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		{
			//GAME LOOP
		}
	}

	return (int)msg.wParam;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine, int nShowCmd)
{
	if (!InitWindowsApp(hInstance, nShowCmd))
		return 0;

	return Run();
}