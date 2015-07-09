#include "Common.h"
#include "CommonRendering.h"
#include "CommonManagers.h"
#include "CommonMath.h"

HWND g_mainWin;
ID3D11DeviceContext*		g_d3dDeviceContext		= nullptr;
ID3D11Device*				g_d3dDevice				= nullptr;
IDXGISwapChain*				g_d3dSwapChain			= nullptr;
ID3D11RenderTargetView*		g_d3dRenderTargetView	= nullptr;
ID3D11Texture2D*			g_d3dDepthStencilBuffer = nullptr;
ID3D11DepthStencilView*		g_d3dDepthStencilView	= nullptr;
ID3D11DepthStencilState*	g_d3dDepthStencilState	= nullptr;
ID3D11RasterizerState*		g_d3dRasterizerState	= nullptr;
D3D11_VIEWPORT				g_Viewport;

HRESULT g_hr;

ID3D11Buffer*			g_d3dVertexBuffer		= nullptr;
ID3D11Buffer*			g_d3dIndexBuffer		= nullptr;
ID3D11Buffer*			g_d3dConstantBuffer		= nullptr;

ID3D11VertexShader*		g_d3dVertexShader		= nullptr;
ID3D11PixelShader*		g_d3dPixelShader		= nullptr;

ID3D11InputLayout*		g_d3dInputLayout		= nullptr;

XMMATRIX g_worldMatrix;
XMMATRIX g_viewMatrix;
XMMATRIX g_projectionMatrix;

struct VertexPosColor
{
	XMFLOAT3 position;
	XMFLOAT3 color;
};

VertexPosColor g_Vertices[8] =
{
	{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f) }, // 0
	{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) }, // 1
	{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(1.0f, 1.0f, 0.0f) }, // 2
	{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) }, // 3
	{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) }, // 4
	{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 1.0f) }, // 5
	{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, 1.0f, 1.0f) }, // 6
	{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 1.0f) }  // 7
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

bool InitDeviceAndSwapChain()
{
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	SecureZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = g_mainWin;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Windowed = TRUE;

	UINT createDeviceFlags = 0;
#if DEBUG || _DEBUG
	createDeviceFlags = D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL featureLevels[]
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0
	};
	D3D_FEATURE_LEVEL featureLevel;

	g_hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevels, _countof(featureLevels),
		D3D11_SDK_VERSION, &swapChainDesc, &g_d3dSwapChain, &g_d3dDevice, &featureLevel, &g_d3dDeviceContext);

	if (FAILED(g_hr))
	{
		g_hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, &featureLevels[1], _countof(featureLevels) - 1,
			D3D11_SDK_VERSION, &swapChainDesc, &g_d3dSwapChain, &g_d3dDevice, &featureLevel, &g_d3dDeviceContext);
	}

	if (FAILED(g_hr))
		return false;

	return true;
}
bool InitSwapChainBuffer()
{
	ID3D11Texture2D* backBuffer;
	g_hr = g_d3dSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
	if (FAILED(g_hr)) return false;

	g_hr = g_d3dDevice->CreateRenderTargetView(backBuffer, nullptr, &g_d3dRenderTargetView);
	if (FAILED(g_hr)) return false;

	SafeRelease(backBuffer);

	D3D11_TEXTURE2D_DESC depthStencilBufferDesc;
	SecureZeroMemory(&depthStencilBufferDesc, sizeof(D3D11_TEXTURE2D_DESC));

	depthStencilBufferDesc.ArraySize = 1;
	depthStencilBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilBufferDesc.CPUAccessFlags = 0;
	depthStencilBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilBufferDesc.Width = screenWidth;
	depthStencilBufferDesc.Height = screenHeight;
	depthStencilBufferDesc.MipLevels = 1;
	depthStencilBufferDesc.SampleDesc.Count = 1;
	depthStencilBufferDesc.SampleDesc.Quality = 0;
	depthStencilBufferDesc.Usage = D3D11_USAGE_DEFAULT;

	g_hr = g_d3dDevice->CreateTexture2D(&depthStencilBufferDesc, nullptr, &g_d3dDepthStencilBuffer);
	if (FAILED(g_hr)) return false;

	g_hr = g_d3dDevice->CreateDepthStencilView(g_d3dDepthStencilBuffer, nullptr, &g_d3dDepthStencilView);
	if (FAILED(g_hr)) return false;

	return true;
}
bool SetupDepthStencilState()
{
	D3D11_DEPTH_STENCIL_DESC depthStencilStateDesc;
	SecureZeroMemory(&depthStencilStateDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));

	depthStencilStateDesc.DepthEnable = TRUE;
	depthStencilStateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilStateDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilStateDesc.StencilEnable = FALSE;

	g_hr = g_d3dDevice->CreateDepthStencilState(&depthStencilStateDesc, &g_d3dDepthStencilState);
	if (FAILED(g_hr)) return false;

	D3D11_RASTERIZER_DESC rasterizerDesc;
	SecureZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));

	rasterizerDesc.AntialiasedLineEnable = FALSE;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0.f;
	rasterizerDesc.DepthClipEnable = TRUE;
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.FrontCounterClockwise = FALSE;
	rasterizerDesc.MultisampleEnable = FALSE;
	rasterizerDesc.ScissorEnable = FALSE;
	rasterizerDesc.SlopeScaledDepthBias = 0.f;

	g_hr = g_d3dDevice->CreateRasterizerState(&rasterizerDesc, &g_d3dRasterizerState);
	if (FAILED(g_hr)) return false;

	return true;
}
void SetupViewport()
{
	g_Viewport.Width = static_cast<float>(screenWidth);
	g_Viewport.Height = static_cast<float>(screenHeight);
	g_Viewport.TopLeftX = 0.f;
	g_Viewport.TopLeftY = 0.f;
	g_Viewport.MinDepth = 0.f;
	g_Viewport.MaxDepth = 1.f;
}
bool InitDirectX()
{
	if (!InitDeviceAndSwapChain() ||
		!InitSwapChainBuffer() ||
		!SetupDepthStencilState()) 
		return false;

	SetupViewport();

	return true;
}

/*				CODE PER PROJECT			*/
bool LoadResources()
{
	D3D11_BUFFER_DESC vertexBufferDesc;
	SecureZeroMemory(&vertexBufferDesc, sizeof(D3D11_BUFFER_DESC));

	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.ByteWidth = sizeof(VertexPosColor) * _countof(g_Vertices);
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;

	D3D11_SUBRESOURCE_DATA resourceData;
	SecureZeroMemory(&resourceData, sizeof(D3D11_SUBRESOURCE_DATA));

	resourceData.pSysMem = g_Vertices;

	g_hr = g_d3dDevice->CreateBuffer(&vertexBufferDesc, &resourceData, &g_d3dVertexBuffer);
	if (FAILED(g_hr)) return false;

	D3D11_BUFFER_DESC indexBufferDesc;
	SecureZeroMemory(&indexBufferDesc, sizeof(D3D11_BUFFER_DESC));

	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.ByteWidth = sizeof(WORD) * _countof(g_Indicies);
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	resourceData.pSysMem = g_Indicies;

	g_hr = g_d3dDevice->CreateBuffer(&indexBufferDesc, &resourceData, &g_d3dIndexBuffer);
	if (FAILED(g_hr)) return false;

	return true;
}
bool CreateBuffers()
{
	D3D11_BUFFER_DESC constantBufferDesc;
	SecureZeroMemory(&constantBufferDesc, sizeof(D3D11_BUFFER_DESC));

	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBufferDesc.ByteWidth = sizeof(XMMATRIX);
	constantBufferDesc.CPUAccessFlags = 0;
	constantBufferDesc.Usage = D3D11_USAGE_DEFAULT;	///>>>>>>>>>>>>>>>>>>>????????????????????????

	g_hr = g_d3dDevice->CreateBuffer(&constantBufferDesc, nullptr, &g_d3dConstantBuffer);
	if (FAILED(g_hr)) return false;

	return true;
}
bool LoadShaders()
{
	UINT shaderFlags = 0;

#if DEBUG || _DEBUG
	shaderFlags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	ID3DBlob* vertexShaderBlob;

	g_hr = D3DCompileFromFile(
		L"C:/Users/Konrad/Documents/DX11Samples/Sample0/Sample0/color.hlsl", NULL, NULL, "VS", "vs_5_0",
		shaderFlags, 0, &vertexShaderBlob, NULL);
	if (FAILED(g_hr)) return false;

	g_hr = g_d3dDevice->CreateVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), nullptr, &g_d3dVertexShader);
	if (FAILED(g_hr)) return false;

	D3D11_INPUT_ELEMENT_DESC vertexLayoutDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosColor, position), D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosColor, color), D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	g_hr = g_d3dDevice->CreateInputLayout(vertexLayoutDesc, _countof(vertexLayoutDesc), vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), &g_d3dInputLayout);
	if (FAILED(g_hr)) return false;

	SafeRelease(vertexShaderBlob);

	ID3DBlob* pixelShaderBlob;

	g_hr = D3DCompileFromFile(
		L"C:/Users/Konrad/Documents/DX11Samples/Sample0/Sample0/color.hlsl", NULL, NULL, "PS", "ps_5_0",
		shaderFlags, 0, &pixelShaderBlob, NULL);
	if (FAILED(g_hr)) return false;
	g_hr = g_d3dDevice->CreatePixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize(), nullptr, &g_d3dPixelShader);
	if (FAILED(g_hr)) return false;

	SafeRelease(pixelShaderBlob);

	return true;
}

void PreRendering()
{
	static const float aWH = static_cast<float>(screenWidth) / static_cast<float>(screenHeight);

	g_worldMatrix = XMMatrixRotationY(XMConvertToRadians(20.f));
	g_projectionMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.f), aWH, 0.1f, 100.f);


	const UINT vertexStride = sizeof(VertexPosColor);
	const UINT offset = 0;

	g_d3dDeviceContext->IASetVertexBuffers(0, 1, &g_d3dVertexBuffer, &vertexStride, &offset);
	g_d3dDeviceContext->IASetInputLayout(g_d3dInputLayout);
	g_d3dDeviceContext->IASetIndexBuffer(g_d3dIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	g_d3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	g_d3dDeviceContext->VSSetShader(g_d3dVertexShader, nullptr, 0);
	g_d3dDeviceContext->VSSetConstantBuffers(0, 1, &g_d3dConstantBuffer);

	g_d3dDeviceContext->RSSetState(g_d3dRasterizerState);
	g_d3dDeviceContext->RSSetViewports(1, &g_Viewport);

	g_d3dDeviceContext->PSSetShader(g_d3dPixelShader, nullptr, 0);

	g_d3dDeviceContext->OMSetRenderTargets(1, &g_d3dRenderTargetView, g_d3dDepthStencilView);
	g_d3dDeviceContext->OMSetDepthStencilState(g_d3dDepthStencilState, 1);
}
void Rendering()
{
	g_d3dDeviceContext->ClearRenderTargetView(g_d3dRenderTargetView, Colors::Blue);
	g_d3dDeviceContext->ClearDepthStencilView(g_d3dDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

	XMMATRIX wvp = g_worldMatrix * g_viewMatrix * g_projectionMatrix;
	g_d3dDeviceContext->UpdateSubresource(g_d3dConstantBuffer, 0, nullptr, &wvp, 0, 0);

	g_d3dDeviceContext->DrawIndexed(_countof(g_Indicies), 0, 0);

	g_d3dSwapChain->Present(0, 0);
}
void PostRendering()
{
	SafeRelease(g_d3dConstantBuffer);
	SafeRelease(g_d3dIndexBuffer);
	SafeRelease(g_d3dVertexBuffer);
	SafeRelease(g_d3dInputLayout);
	SafeRelease(g_d3dVertexShader);
	SafeRelease(g_d3dPixelShader);

	SafeRelease(g_d3dDepthStencilView);
	SafeRelease(g_d3dRenderTargetView);
	SafeRelease(g_d3dDepthStencilBuffer);
	SafeRelease(g_d3dDepthStencilState);
	SafeRelease(g_d3dRasterizerState);
	SafeRelease(g_d3dSwapChain);
	SafeRelease(g_d3dDeviceContext);
	SafeRelease(g_d3dDevice);
}
/*				CODE PER PROJECT			*/

int Run()
{
	MSG msg = { 0 };
	unsigned int frameCount = 0;
	float time = Timer::GetTimeFromBeginning();
	PreRendering();
	while (msg.message != WM_QUIT)
	{
		Timer::Tick();
		++frameCount;
		if (Timer::GetTimeFromBeginning() - time >= 1.f)
		{
			time = Timer::GetTimeFromBeginning();
			SetWindowText(g_mainWin, std::to_wstring(frameCount).c_str());
			frameCount = 0;
		}
		Input::Update();
		
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			Rendering();
		}
	}
	PostRendering();
	return (int)msg.wParam;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine, int nShowCmd)
{
	if (!InitWindowsApp(hInstance, nShowCmd) ||
		!InitDirectX())
		return 0;

	if (!LoadResources() ||
		!CreateBuffers() ||
		!LoadShaders())
		return 0;
	Timer::Setup();
	return Run();
}