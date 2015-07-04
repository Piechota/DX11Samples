#include "Common.h"
#include "CommonRendering.h"
#include "CommonManagers.h"
#include "Input.h"

HINSTANCE _hAppInst;
HWND _mainWin;
UINT _4xMsaaQuality;

ID3D11Device* _d3dDevice;
ID3D11DeviceContext* _d3dImmediateContext;
IDXGISwapChain* _SwapChain;
ID3D11Texture2D* _DepthStencilBuffer;
ID3D11RenderTargetView* _RenderTargetView;
ID3D11DepthStencilView* _DepthStencilView;
D3D11_VIEWPORT _ScreenViewport;

bool InitWindowsApp(HINSTANCE instanceHandle, int show)
{
	_hAppInst = instanceHandle;

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

	_mainWin = CreateWindow(
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

	if (_mainWin == 0)
	{
		MessageBox(0, L"CreateWindow FAILED", 0, 0);
		return false;
	}

	ShowWindow(_mainWin, show);
	return true;
}
bool InitDirectX()
{
	UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL featureLevel;
	HRESULT hr = D3D11CreateDevice(
		0,
		D3D_DRIVER_TYPE_HARDWARE,
		0,
		createDeviceFlags,
		0, 0,
		D3D11_SDK_VERSION,
		&_d3dDevice,
		&featureLevel,
		&_d3dImmediateContext
		);

	if (FAILED(hr))
	{
		MessageBox(0, L"Blad D3D11CreateDevice", 0, 0);
		return false;
	}
	if (featureLevel != D3D_FEATURE_LEVEL_11_0)
	{
		MessageBox(0, L"Poziom mozliwosci DX11 nie jest obslugiwany", 0, 0);
		return false;
	}

	_d3dDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &_4xMsaaQuality);
	assert(_4xMsaaQuality > 0);

	DXGI_SWAP_CHAIN_DESC scd;
	scd.BufferDesc.Width = screenWidth;
	scd.BufferDesc.Height = screenHeight;
	scd.BufferDesc.RefreshRate.Numerator = 60;
	scd.BufferDesc.RefreshRate.Denominator = 1;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	if (mEnable4xMsaa)
	{
		scd.SampleDesc.Count = 4;
		scd.SampleDesc.Quality = _4xMsaaQuality-1;
	}
	else
	{
		scd.SampleDesc.Count = 1;
		scd.SampleDesc.Quality = 0;
	}

	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.BufferCount = 1;
	scd.OutputWindow = _mainWin;
	scd.Windowed = true;
	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	scd.Flags = 0;

	IDXGIDevice* dxgiDevice = nullptr;
	_d3dDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice);

	IDXGIAdapter* dxgiAdapter = nullptr;
	dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter);

	IDXGIFactory* dxgiFactory = nullptr;
	dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory);

	hr = dxgiFactory->CreateSwapChain(_d3dDevice, &scd, &_SwapChain);

	dxgiDevice->Release();
	dxgiAdapter->Release();
	dxgiFactory->Release();

	ID3D11Texture2D* backBuffer;
	_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer));
	_d3dDevice->CreateRenderTargetView(backBuffer, 0, &_RenderTargetView);
	backBuffer->Release();

	D3D11_TEXTURE2D_DESC depthStencilDesc;
	depthStencilDesc.Width = screenWidth;
	depthStencilDesc.Height = screenHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	if (mEnable4xMsaa)
	{
		depthStencilDesc.SampleDesc.Count = 4;
		depthStencilDesc.SampleDesc.Quality = _4xMsaaQuality - 1;
	}
	else
	{
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
	}
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	_d3dDevice->CreateTexture2D(&depthStencilDesc, 0, &_DepthStencilBuffer);
	_d3dDevice->CreateDepthStencilView(_DepthStencilBuffer, 0, &_DepthStencilView);

	_d3dImmediateContext->OMSetRenderTargets(1, &_RenderTargetView, _DepthStencilView);

	_ScreenViewport.TopLeftX = 0.f;
	_ScreenViewport.TopLeftY = 0.f;
	_ScreenViewport.Width = static_cast<float>(screenWidth);
	_ScreenViewport.Height = static_cast<float>(screenHeight);
	_ScreenViewport.MinDepth = 0.f;
	_ScreenViewport.MaxDepth = 1.f;

	_d3dImmediateContext->RSSetViewports(1, &_ScreenViewport);

	return true;
}

void Clear()
{
	_d3dDevice->Release();
	_d3dImmediateContext->Release();
	_SwapChain->Release();
	_DepthStencilBuffer->Release();
	_RenderTargetView->Release();
	_DepthStencilView->Release();
}

int Run()
{
	MSG msg = { 0 };
	unsigned int frameCount = 0;
	float time = Timer::GetTimeFromBeginning();
	while (msg.message != WM_QUIT)
	{
		Timer::Tick();
		++frameCount;
		if (Timer::GetTimeFromBeginning() - time >= 1.f)
		{
			time = Timer::GetTimeFromBeginning();
			SetWindowText(_mainWin, std::to_wstring(frameCount).c_str());
			frameCount = 0;
		}

		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		{
			//GAME LOOP
		}

		_d3dImmediateContext->ClearRenderTargetView(_RenderTargetView, Colors::Blue);
		_d3dImmediateContext->ClearDepthStencilView(_DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
		_SwapChain->Present(0, 0);
	}

	Clear();
	return (int)msg.wParam;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine, int nShowCmd)
{
	if (!InitWindowsApp(hInstance, nShowCmd))
		return 0;
	if (!InitDirectX())
		return 0;
	Timer::Setup();
	return Run();
}