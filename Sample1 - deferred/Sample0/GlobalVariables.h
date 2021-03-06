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
ID3D11DepthStencilState*	g_d3dSceneDepthStencilState = nullptr;
ID3D11RasterizerState*		g_d3dRasterizerState = nullptr;
D3D11_VIEWPORT				g_Viewport;

HRESULT g_hr;

ID3D11Buffer*				g_d3dVertexBuffer = nullptr;
ID3D11Buffer*				g_d3dIndexBuffer = nullptr;
ID3D11Buffer*				g_d3dSceneBuffer = nullptr;
ID3D11Buffer*				g_d3dConstantBuffer = nullptr;

ID3D11Buffer*				g_d3dLightBuffer = nullptr;

ID3D11Resource*				g_diffuseTex = nullptr;
ID3D11ShaderResourceView*	g_diffuseView = nullptr;

ID3D11ShaderResourceView*	g_d3dDepthSRV = nullptr;

ID3D11SamplerState*			g_d3dSamplerState = nullptr;

ID3D11VertexShader*			g_d3dVertexShader = nullptr;
ID3D11PixelShader*			g_d3dPixelShader = nullptr;

ID3D11VertexShader*			g_d3dSimpleShader = nullptr;
ID3D11PixelShader*			g_d3dSceneShader = nullptr;

ID3D11InputLayout*			g_d3dInputLayout = nullptr;
ID3D11InputLayout*			g_d3dSceneInputLayout = nullptr;

ID3D11Texture2D*			g_d3dgBuffersTex[2];
ID3D11RenderTargetView*		g_d3dgBufferRTV[2];
ID3D11ShaderResourceView*	g_d3dgBufferSRV[2];

XMMATRIX g_worldMatrix;
XMMATRIX g_viewMatrix;
XMMATRIX g_projectionMatrix;