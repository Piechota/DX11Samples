#pragma once;
#include "Common.h"
#include "CommonRendering.h"
#include "CommonMath.h"

HWND g_mainWin;
ID3D11DeviceContext*		g_d3dDeviceContext = nullptr;
ID3D11Device*				g_d3dDevice = nullptr;
IDXGISwapChain*				g_d3dSwapChain = nullptr;
ID3D11RenderTargetView*		g_d3dRenderTargetView = nullptr;
ID3D11Texture2D*			g_d3dDepthStencilBuffer = nullptr;
ID3D11DepthStencilView*		g_d3dDepthStencilView = nullptr;
ID3D11DepthStencilState*	g_d3dDepthStencilState = nullptr;
ID3D11RasterizerState*		g_d3dRasterizerState = nullptr;
D3D11_VIEWPORT				g_Viewport;

HRESULT g_hr;

ID3D11Buffer*				g_d3dVertexBuffer = nullptr;
ID3D11Buffer*				g_d3dParticleBuffer = nullptr;
ID3D11Buffer*				g_d3dConstantBuffer = nullptr;

ID3D11UnorderedAccessView*	g_d3dVertexView = nullptr;
ID3D11UnorderedAccessView*	g_d3dParticleView = nullptr;

ID3D11VertexShader*			g_d3dVertexShader = nullptr;
ID3D11GeometryShader*		g_d3dGeometryShader = nullptr;
ID3D11PixelShader*			g_d3dPixelShader = nullptr;

ID3D11ComputeShader*		g_d3dComputeShader = nullptr;

ID3D11InputLayout*			g_d3dInputLayout = nullptr;

XMMATRIX g_worldMatrix;
XMMATRIX g_viewMatrix;
XMMATRIX g_projectionMatrix;