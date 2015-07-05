#include "Common.h"
#include "CommonRendering.h"
#include "CommonManagers.h"
#include "CommonMath.h"
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

/////////////////////////////////////////////////////////////////////////////////////

struct Vertex
{
	XMFLOAT3 pos;
	XMFLOAT4 color;
};

ID3D11Buffer* _boxVB;
ID3D11Buffer* _boxIB;
ID3DX11Effect* _FX;

ID3DX11EffectTechnique* _tech;
ID3DX11EffectMatrixVariable* _mvpMatrixFX;

ID3D11InputLayout* _inputLayout;

XMFLOAT4X4 _world;
XMFLOAT4X4 _view;
XMFLOAT4X4 _projection;

void PreDrawInstructions()
{
	XMStoreFloat4x4(&_world, XMMatrixIdentity());
	XMStoreFloat4x4(&_view, XMMatrixIdentity());
	XMStoreFloat4x4(&_projection, XMMatrixIdentity());

	Vertex vertices[]=
	{
		{ XMFLOAT3(-1.f, -1.f, -1.f), XMFLOAT4(Colors::White) },
		{ XMFLOAT3(-1.f, +1.f, -1.f), XMFLOAT4(Colors::Red) },
		{ XMFLOAT3(+1.f, +1.f, -1.f), XMFLOAT4(Colors::Blue) },
		{ XMFLOAT3(+1.f, -1.f, -1.f), XMFLOAT4(Colors::Green) },
		{ XMFLOAT3(-1.f, -1.f, +1.f), XMFLOAT4(Colors::Yellow) },
		{ XMFLOAT3(-1.f, +1.f, +1.f), XMFLOAT4(Colors::Green) },
		{ XMFLOAT3(+1.f, +1.f, +1.f), XMFLOAT4(Colors::Black) },
		{ XMFLOAT3(+1.f, -1.f, +1.f), XMFLOAT4(Colors::Red) }
	};

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex) * 8;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = vertices;
	_d3dDevice->CreateBuffer(&vbd, &vinitData, &_boxVB);

	UINT indices[]=
	{
		0, 1, 2,
		0, 2, 3,

		4, 6, 5,
		4, 7, 6,

		4, 5, 1,
		4, 1, 0,

		3, 2, 6,
		3, 6, 7,

		1, 5, 6,
		1, 6, 2,

		4, 0, 3,
		4, 3, 7
	};

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * 36;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = indices;
	_d3dDevice->CreateBuffer(&ibd, &iinitData, &_boxIB);

	DWORD shaderFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	shaderFlags |= D3D10_SHADER_DEBUG;
	shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif

	ID3D10Blob* compiledShader = nullptr;
	ID3D10Blob* compilationMsgs = nullptr;
	HRESULT hr = D3DX11CompileFromFile(L"C:/Users/Konrad/Documents/DX11Samples/Sample0/Sample0/FX/color.fx", 0, 0, 0, "fx_5_0", shaderFlags, 0, 0, &compiledShader, &compilationMsgs, 0);
	if (compilationMsgs != 0)
	{
		MessageBox(0, (LPCWSTR)compilationMsgs->GetBufferPointer(), 0, 0);
		compilationMsgs->Release();
	}
	if (FAILED(hr))
		DXTrace(__FILE__, (DWORD)__LINE__, hr, L"D3DX11CompileFromFIle", true);

	D3DX11CreateEffectFromMemory(
		compiledShader->GetBufferPointer(),
		compiledShader->GetBufferSize(),
		0, _d3dDevice, &_FX);

	compiledShader->Release();

	_tech = _FX->GetTechniqueByName("Default");
	_mvpMatrixFX = _FX->GetVariableByName("gWorldViewProj")->AsMatrix();


	D3D11_INPUT_ELEMENT_DESC vertexDesc[]=
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	D3DX11_PASS_DESC passDesc;
	_tech->GetPassByIndex(0)->GetDesc(&passDesc);
	_d3dDevice->CreateInputLayout(vertexDesc, 2, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &_inputLayout);
}

void DrawInstructions()
{
	_d3dImmediateContext->ClearRenderTargetView(_RenderTargetView, Colors::Black);
	_d3dImmediateContext->ClearDepthStencilView(_DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

	_d3dImmediateContext->IASetInputLayout(_inputLayout);
	_d3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	_d3dImmediateContext->IASetVertexBuffers(0, 1, &_boxVB, &stride, &offset);
	_d3dImmediateContext->IASetIndexBuffer(_boxIB, DXGI_FORMAT_R32_UINT, 0);

	XMMATRIX mvpMatrix = XMMatrixTranslation(0.f, 0.f, -5.f) * XMMatrixPerspectiveFovRH(60.f, 1280.f/720.f, 0.001f, 100.f);

	_mvpMatrixFX->SetMatrix(reinterpret_cast<float*>(&mvpMatrix));

	_tech->GetPassByIndex(0)->Apply(0, _d3dImmediateContext);
	_d3dImmediateContext->DrawIndexed(36, 0, 0);

	_SwapChain->Present(0, 0);
}
void PostDrawInstructions()
{
	_boxVB->Release();
	_boxIB->Release();
	_FX->Release();
	_inputLayout->Release();
}
/////////////////////////////////////////////////////////////////////////////////////

int Run()
{
	MSG msg = { 0 };
	unsigned int frameCount = 0;
	float time = Timer::GetTimeFromBeginning();
	PreDrawInstructions();
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
		DrawInstructions();
	}
	PostDrawInstructions();
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