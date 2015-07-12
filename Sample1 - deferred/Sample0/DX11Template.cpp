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

	SecureZeroMemory(&vertexBufferDesc, sizeof(D3D11_BUFFER_DESC));

	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.ByteWidth = sizeof(VertexPosUV) * _countof(g_Scene);
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;

	SecureZeroMemory(&resourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	resourceData.pSysMem = g_Scene;

	g_hr = g_d3dDevice->CreateBuffer(&vertexBufferDesc, &resourceData, &g_d3dSceneBuffer);
	if (FAILED(g_hr)) return false;

	g_hr = CreateDDSTextureFromFileEx(
		g_d3dDevice, L"../../Resources/brick_albedo.dds",
		0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, 0, false, 
		&g_diffuseTex, &g_diffuseView);

	if (FAILED(g_hr)) return false;

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceView;
	SecureZeroMemory(&shaderResourceView, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));

	shaderResourceView.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	shaderResourceView.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceView.Texture2D.MipLevels = 1;
	shaderResourceView.Texture2D.MostDetailedMip = 0;
	g_hr = g_d3dDevice->CreateShaderResourceView(g_d3dDepthStencilBuffer, &shaderResourceView, &g_d3dDepthSRV);
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

	SecureZeroMemory(&constantBufferDesc, sizeof(D3D11_BUFFER_DESC));

	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBufferDesc.ByteWidth = sizeof(PointLight);
	constantBufferDesc.CPUAccessFlags = 0;
	constantBufferDesc.Usage = D3D11_USAGE_DEFAULT;

	g_hr = g_d3dDevice->CreateBuffer(&constantBufferDesc, nullptr, &g_d3dLightBuffer);
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
		L"C:/Users/Konrad/Documents/DX11Samples/Sample1 - deferred/Sample0/color.hlsl", NULL, NULL, "VS", "vs_5_0",
		shaderFlags, 0, &vertexShaderBlob, NULL);
	if (FAILED(g_hr)) return false;

	g_hr = g_d3dDevice->CreateVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), nullptr, &g_d3dVertexShader);
	if (FAILED(g_hr)) return false;

	D3D11_INPUT_ELEMENT_DESC vertexLayoutDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosColor, position), D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosColor, color), D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosColor, normal), D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(VertexPosColor, texCoord), D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	g_hr = g_d3dDevice->CreateInputLayout(vertexLayoutDesc, _countof(vertexLayoutDesc), vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), &g_d3dInputLayout);
	if (FAILED(g_hr)) return false;

	SafeRelease(vertexShaderBlob);

	ID3DBlob* pixelShaderBlob;

	g_hr = D3DCompileFromFile(
		L"C:/Users/Konrad/Documents/DX11Samples/Sample1 - deferred/Sample0/color.hlsl", NULL, NULL, "PS", "ps_5_0",
		shaderFlags, 0, &pixelShaderBlob, NULL);
	if (FAILED(g_hr)) return false;
	g_hr = g_d3dDevice->CreatePixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize(), nullptr, &g_d3dPixelShader);
	if (FAILED(g_hr)) return false;

	SafeRelease(pixelShaderBlob);

	return true;
}

bool LoadSceneShader()
{
	UINT shaderFlags = 0;

#if DEBUG || _DEBUG
	shaderFlags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	ID3DBlob* vertexShaderBlob;

	g_hr = D3DCompileFromFile(
		L"C:/Users/Konrad/Documents/DX11Samples/Sample1 - deferred/Sample0/SceneDraw.hlsl", NULL, NULL, "VS", "vs_5_0",
		shaderFlags, 0, &vertexShaderBlob, NULL);
	if (FAILED(g_hr)) return false;

	g_hr = g_d3dDevice->CreateVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), nullptr, &g_d3dSimpleShader);
	if (FAILED(g_hr)) return false;

	D3D11_INPUT_ELEMENT_DESC vertexLayoutDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosUV, position), D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(VertexPosUV, texCoord), D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	g_hr = g_d3dDevice->CreateInputLayout(vertexLayoutDesc, _countof(vertexLayoutDesc), vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), &g_d3dSceneInputLayout);
	if (FAILED(g_hr)) return false;

	SafeRelease(vertexShaderBlob);

	ID3DBlob* pixelShaderBlob;

	g_hr = D3DCompileFromFile(
		L"C:/Users/Konrad/Documents/DX11Samples/Sample1 - deferred/Sample0/SceneDraw.hlsl", NULL, NULL, "PS", "ps_5_0",
		shaderFlags, 0, &pixelShaderBlob, NULL);
	if (FAILED(g_hr)) return false;
	g_hr = g_d3dDevice->CreatePixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize(), nullptr, &g_d3dSceneShader);
	if (FAILED(g_hr)) return false;

	SafeRelease(pixelShaderBlob);

	return true;
}

bool InitGBuffer()
{
	D3D11_TEXTURE2D_DESC t2d_desc;
	SecureZeroMemory(&t2d_desc, sizeof(D3D11_TEXTURE2D_DESC));

	t2d_desc.ArraySize = 1;
	t2d_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	t2d_desc.CPUAccessFlags = 0;
	t2d_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	t2d_desc.Height = 1024;
	t2d_desc.Width = 1024;
	t2d_desc.MipLevels = 1;
	t2d_desc.MiscFlags = 0;
	t2d_desc.SampleDesc.Count = 1;
	t2d_desc.SampleDesc.Quality = 0;
	t2d_desc.Usage = D3D11_USAGE_DEFAULT;
	g_hr = g_d3dDevice->CreateTexture2D(&t2d_desc, nullptr, &g_d3dgBuffersTex[0]);
	if (FAILED(g_hr)) return false;
	g_hr = g_d3dDevice->CreateTexture2D(&t2d_desc, nullptr, &g_d3dgBuffersTex[1]);
	if (FAILED(g_hr)) return false;


	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	SecureZeroMemory(&rtvDesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;
	g_hr = g_d3dDevice->CreateRenderTargetView(g_d3dgBuffersTex[0], &rtvDesc, &g_d3dgBufferRTV[0]);
	if (FAILED(g_hr)) return false;
	g_hr = g_d3dDevice->CreateRenderTargetView(g_d3dgBuffersTex[1], &rtvDesc, &g_d3dgBufferRTV[1]);
	if (FAILED(g_hr)) return false;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	SecureZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	g_hr = g_d3dDevice->CreateShaderResourceView(g_d3dgBuffersTex[0], &srvDesc, &g_d3dgBufferSRV[0]);
	if (FAILED(g_hr)) return false;
	g_hr = g_d3dDevice->CreateShaderResourceView(g_d3dgBuffersTex[1], &srvDesc, &g_d3dgBufferSRV[1]);
	if (FAILED(g_hr)) return false;

	return true;
}

void PreRendering()
{
	static const float aWH = static_cast<float>(screenWidth) / static_cast<float>(screenHeight);


	g_worldMatrix = XMMatrixRotationY(XMConvertToRadians(20.f));
	g_projectionMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.f), aWH, 0.1f, 10.f);

	g_d3dDeviceContext->VSSetConstantBuffers(0, 1, &g_d3dConstantBuffer);
	g_d3dDeviceContext->PSSetConstantBuffers(1, 1, &g_d3dLightBuffer);

	g_d3dDeviceContext->RSSetState(g_d3dRasterizerState);
	g_d3dDeviceContext->RSSetViewports(1, &g_Viewport);
	g_d3dDeviceContext->PSSetShaderResources(0, 1, &g_diffuseView);

	PointLight p0;
	p0.position = XMFLOAT3(0.f, 0.f, -3.f);
	p0.color = XMFLOAT3(1.f, 1.f, 1.f);
	p0.linearAtt = 1.f;

	g_d3dDeviceContext->UpdateSubresource(g_d3dLightBuffer, 0, nullptr, &p0, 0, 0);
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

	static ID3D11ShaderResourceView* pNullSRV = NULL;
	g_d3dDeviceContext->PSSetShaderResources(1, 1, &pNullSRV);

	g_d3dDeviceContext->OMSetRenderTargets(2, g_d3dgBufferRTV, g_d3dDepthStencilView);
	g_d3dDeviceContext->OMSetDepthStencilState(g_d3dDepthStencilState, 1);
}

void PreSceneDraw()
{
	const UINT vertexStride = sizeof(VertexPosUV);
	const UINT offset = 0;

	g_d3dDeviceContext->IASetVertexBuffers(0, 1, &g_d3dSceneBuffer, &vertexStride, &offset);
	g_d3dDeviceContext->IASetInputLayout(g_d3dSceneInputLayout);
	g_d3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	g_d3dDeviceContext->VSSetShader(g_d3dSimpleShader, nullptr, 0);
	g_d3dDeviceContext->PSSetShader(g_d3dSceneShader, nullptr, 0);

	g_d3dDeviceContext->OMSetRenderTargets(1, &g_d3dRenderTargetView, nullptr);
	g_d3dDeviceContext->OMSetDepthStencilState(g_d3dSceneDepthStencilState, 1);
	g_d3dDeviceContext->PSSetShaderResources(1, 1, &g_d3dgBufferSRV[0]);
	g_d3dDeviceContext->PSSetShaderResources(2, 1, &g_d3dgBufferSRV[1]);
	g_d3dDeviceContext->PSSetShaderResources(3, 1, &g_d3dDepthSRV);
}
void DrawBox()
{
	g_d3dDeviceContext->ClearRenderTargetView(g_d3dgBufferRTV[0], Colors::Blue);
	g_d3dDeviceContext->ClearRenderTargetView(g_d3dgBufferRTV[1], Colors::Blue);
	g_d3dDeviceContext->ClearDepthStencilView(g_d3dDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

	XMMATRIX matrices[2];
	matrices[0] = g_worldMatrix * g_viewMatrix * g_projectionMatrix;
	matrices[1] = g_worldMatrix * g_viewMatrix;

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	SecureZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	g_d3dDeviceContext->Map(g_d3dConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy(mappedResource.pData, matrices, sizeof(matrices));
	g_d3dDeviceContext->Unmap(g_d3dConstantBuffer, 0);

	g_d3dDeviceContext->DrawIndexed(_countof(g_Indicies), 0, 0);
}
void DrawScene()
{
	g_d3dDeviceContext->Draw(4, 0);
	g_d3dSwapChain->Present(0, 0);
}
void PostRendering()
{
	SafeRelease(g_d3dConstantBuffer);
	SafeRelease(g_d3dLightBuffer);
	SafeRelease(g_d3dIndexBuffer);
	SafeRelease(g_d3dSceneBuffer);
	SafeRelease(g_d3dVertexBuffer);
	SafeRelease(g_d3dInputLayout);
	SafeRelease(g_d3dSceneInputLayout);
	SafeRelease(g_d3dVertexShader);
	SafeRelease(g_d3dPixelShader);
	SafeRelease(g_d3dSceneShader);

	SafeRelease(g_d3dSamplerState);

	SafeRelease(g_diffuseTex);
	SafeRelease(g_diffuseView);

	SafeRelease(g_d3dDepthSRV);

	SafeRelease(g_d3dgBufferRTV[0]);
	SafeRelease(g_d3dgBufferRTV[1]);
	SafeRelease(g_d3dgBufferSRV[0]);
	SafeRelease(g_d3dgBufferSRV[1]);
	SafeRelease(g_d3dgBuffersTex[0]);
	SafeRelease(g_d3dgBuffersTex[1]);

	SafeRelease(g_d3dDepthStencilView);
	SafeRelease(g_d3dRenderTargetView);
	SafeRelease(g_d3dDepthStencilBuffer);
	SafeRelease(g_d3dDepthStencilState);
	SafeRelease(g_d3dSceneDepthStencilState);
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
			PreBoxDraw();
			DrawBox();
			PreSceneDraw();
			DrawScene();
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
		!LoadShaders()	 ||
		!LoadSceneShader() ||
		!InitGBuffer())
		return 0;
	Timer::Setup();
	return Run();
}