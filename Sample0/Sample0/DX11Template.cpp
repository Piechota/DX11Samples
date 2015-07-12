#include "GlobalVariables.h"
#include "Resources.h"

#include "WinInit.h"
#include "D3DInit.h"

#include "Common.h"
#include "CommonRendering.h"
#include "CommonManagers.h"
#include "CommonMath.h"

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

	g_hr = CreateDDSTextureFromFileEx(
		g_d3dDevice, L"../../Resources/brick_albedo.dds",
		0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, 0, false, 
		&g_diffuseTex, &g_diffuseView);

	if (FAILED(g_hr)) return false;

	return true;
}
bool CreateBuffers()
{
	D3D11_BUFFER_DESC constantBufferDesc;
	SecureZeroMemory(&constantBufferDesc, sizeof(D3D11_BUFFER_DESC));

	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBufferDesc.ByteWidth = sizeof(XMMATRIX) * 2;
	constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;

	g_hr = g_d3dDevice->CreateBuffer(&constantBufferDesc, nullptr, &g_d3dConstantBuffer);
	if (FAILED(g_hr)) return false;

	return true;
}

bool CreateSampler()
{
	D3D11_SAMPLER_DESC samplerDesc;
	SecureZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));

	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	g_hr = g_d3dDevice->CreateSamplerState(&samplerDesc, &g_d3dSamplerState);
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
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(VertexPosColor, texCoord), D3D11_INPUT_PER_VERTEX_DATA, 0 }
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
	g_projectionMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.f), aWH, 0.1f, 10.f);

	g_d3dDeviceContext->VSSetConstantBuffers(0, 1, &g_d3dConstantBuffer);

	g_d3dDeviceContext->RSSetState(g_d3dRasterizerState);
	g_d3dDeviceContext->RSSetViewports(1, &g_Viewport);
	g_d3dDeviceContext->PSSetShaderResources(0, 1, &g_diffuseView);

	g_d3dDeviceContext->PSSetSamplers(0, 1, &g_d3dSamplerState);
}

void PreBoxDraw()
{
	const UINT vertexStride = sizeof(VertexPosColor);
	const UINT offset = 0;

	g_d3dDeviceContext->IASetVertexBuffers(0, 1, &g_d3dVertexBuffer, &vertexStride, &offset);
	g_d3dDeviceContext->IASetInputLayout(g_d3dInputLayout);
	g_d3dDeviceContext->IASetIndexBuffer(g_d3dIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	g_d3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	g_d3dDeviceContext->VSSetShader(g_d3dVertexShader, nullptr, 0);
	g_d3dDeviceContext->PSSetShader(g_d3dPixelShader, nullptr, 0);

	g_d3dDeviceContext->OMSetRenderTargets(1, &g_d3dRenderTargetView, g_d3dDepthStencilView);
	g_d3dDeviceContext->OMSetDepthStencilState(g_d3dDepthStencilState, 1);
}

void DrawBox()
{
	g_d3dDeviceContext->ClearRenderTargetView(g_d3dRenderTargetView, Colors::Blue);
	g_d3dDeviceContext->ClearDepthStencilView(g_d3dDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

	XMMATRIX matrices;
	matrices = g_worldMatrix * g_viewMatrix * g_projectionMatrix;

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	SecureZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	g_d3dDeviceContext->Map(g_d3dConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy(mappedResource.pData, &matrices, sizeof(matrices));
	g_d3dDeviceContext->Unmap(g_d3dConstantBuffer, 0);

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

	SafeRelease(g_d3dSamplerState);

	SafeRelease(g_diffuseTex);
	SafeRelease(g_diffuseView);

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
	PreBoxDraw();
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
			DrawBox();
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
		!CreateSampler() ||
		!LoadShaders())
		return 0;
	Timer::Setup();
	return Run();
}