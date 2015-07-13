#pragma once

#include "GlobalVariables.h"

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

	ID3D11Texture2D* backBuffer;
	g_hr = g_d3dSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
	if (FAILED(g_hr)) return false;

	g_hr = g_d3dDevice->CreateRenderTargetView(backBuffer, nullptr, &g_d3dRenderTargetView);
	if (FAILED(g_hr)) return false;

	SafeRelease(backBuffer);

	return true;
}

bool InitDepthStencilBufferView()
{
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
		!InitDepthStencilBufferView() ||
		!SetupDepthStencilState())
		return false;

	SetupViewport();

	return true;
}