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
	Particle	l_particleData[particleCount];
	float	l_particlePositionData[particleCount * 3];
	SecureZeroMemory(l_particleData, sizeof(Particle) * particleCount);
	SecureZeroMemory(l_particlePositionData, sizeof(float) * 3 * particleCount);

	for (int i = 0; i < particleCount; ++i)
	{
		int x = i - particleCount / 2;
		srand(x);
		l_particlePositionData[i * 3 + 0] = 0.f;
		l_particlePositionData[i * 3 + 1] = 0.f;
		l_particlePositionData[i * 3 + 2] = 0.f;
		l_particleData[i].velocity = XMFLOAT3(((float)(rand() % 100) - 50) / 100.f, (float)(rand() % 100) / 100.f, ((float)(rand() % 100) - 50) / 100.f);
		XMVECTOR tmp = XMVectorSet(l_particleData[i].velocity.x, l_particleData[i].velocity.y, l_particleData[i].velocity.z, 0.f);
		tmp = XMVector3Normalize(tmp);
		l_particleData[i].velocity.x = XMVectorGetX(tmp) * 2.f;
		l_particleData[i].velocity.y = XMVectorGetY(tmp) * 2.f;
		l_particleData[i].velocity.z = XMVectorGetZ(tmp) * 2.f;
		l_particleData[i].accelerate = XMFLOAT3(0.f, -1.f, 0.f);
	}

	D3D11_BUFFER_DESC particleDesc;
	SecureZeroMemory(&particleDesc, sizeof(D3D11_BUFFER_DESC));

	particleDesc.Usage = D3D11_USAGE_DEFAULT;
	particleDesc.ByteWidth = sizeof(Particle) * particleCount;
	particleDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	particleDesc.CPUAccessFlags = 0;
	particleDesc.StructureByteStride = sizeof(Particle);
	particleDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	D3D11_SUBRESOURCE_DATA particleData;
	SecureZeroMemory(&particleData, sizeof(D3D11_SUBRESOURCE_DATA));

	particleData.pSysMem = l_particleData;

	g_hr = g_d3dDevice->CreateBuffer(&particleDesc, &particleData, &g_d3dParticleBuffer);
	if (FAILED(g_hr)) return false;

	SecureZeroMemory(&particleDesc, sizeof(D3D11_BUFFER_DESC));

	particleDesc.Usage = D3D11_USAGE_DEFAULT;
	particleDesc.ByteWidth = sizeof(float) * 3 * particleCount;
	particleDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_VERTEX_BUFFER;
	particleDesc.CPUAccessFlags = 0;

	SecureZeroMemory(&particleData, sizeof(D3D11_SUBRESOURCE_DATA));

	particleData.pSysMem = l_particlePositionData;

	g_hr = g_d3dDevice->CreateBuffer(&particleDesc, &particleData, &g_d3dVertexBuffer);
	if (FAILED(g_hr)) return false;

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	SecureZeroMemory(&uavDesc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));

	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.Flags = 0;
	uavDesc.Buffer.NumElements = particleCount;

	g_hr = g_d3dDevice->CreateUnorderedAccessView(g_d3dParticleBuffer, &uavDesc, &g_d3dParticleView);
	if (FAILED(g_hr)) return false;

	SecureZeroMemory(&uavDesc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));

	uavDesc.Format = DXGI_FORMAT_R32_FLOAT;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.Flags = 0;
	uavDesc.Buffer.NumElements = particleCount * 3;

	g_hr = g_d3dDevice->CreateUnorderedAccessView(g_d3dVertexBuffer, &uavDesc, &g_d3dVertexView);
	if (FAILED(g_hr)) return false;

	return true;
}
bool CreateBuffers()
{
	D3D11_BUFFER_DESC constantBufferDesc;
	SecureZeroMemory(&constantBufferDesc, sizeof(D3D11_BUFFER_DESC));

	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBufferDesc.ByteWidth = sizeof(XMMATRIX) + 16;
	constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;

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

	ID3DBlob* errorBlob;
	ID3DBlob* computeShaderBlob;

	g_hr = D3DCompileFromFile(
		L"C:/Users/Konrad/Documents/DX11Samples/Sample3 - compute shader/Sample0/color.hlsl", NULL, NULL, "CS", "cs_5_0",
		shaderFlags, 0, &computeShaderBlob, &errorBlob);
	if (FAILED(g_hr))
	{
		D3DWriteBlobToFile(errorBlob, L"shit.txt", true);
		SafeRelease(errorBlob);
		return false;
	}
	g_hr = g_d3dDevice->CreateComputeShader(computeShaderBlob->GetBufferPointer(), computeShaderBlob->GetBufferSize(), nullptr, &g_d3dComputeShader);
	if (FAILED(g_hr)) return false;

	SafeRelease(computeShaderBlob);

	ID3DBlob* vertexShaderBlob;

	g_hr = D3DCompileFromFile(
		L"C:/Users/Konrad/Documents/DX11Samples/Sample3 - compute shader/Sample0/color.hlsl", NULL, NULL, "VS", "vs_5_0",
		shaderFlags, 0, &vertexShaderBlob, &errorBlob);
	if (FAILED(g_hr))
	{
		D3DWriteBlobToFile(errorBlob, L"shit.txt", true);
		SafeRelease(errorBlob);
		return false;
	}

	g_hr = g_d3dDevice->CreateVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), nullptr, &g_d3dVertexShader);
	if (FAILED(g_hr)) return false;

	D3D11_INPUT_ELEMENT_DESC vertexLayoutDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	g_hr = g_d3dDevice->CreateInputLayout(vertexLayoutDesc, _countof(vertexLayoutDesc), vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), &g_d3dInputLayout);
	if (FAILED(g_hr)) return false;

	SafeRelease(vertexShaderBlob);

	ID3DBlob* geometryShaderBlob;

	g_hr = D3DCompileFromFile(
		L"C:/Users/Konrad/Documents/DX11Samples/Sample3 - compute shader/Sample0/color.hlsl", NULL, NULL, "GS", "gs_5_0",
		shaderFlags, 0, &geometryShaderBlob, &errorBlob);
	if (FAILED(g_hr))
	{
		D3DWriteBlobToFile(errorBlob, L"shit.txt", true);
		SafeRelease(errorBlob);
		return false;
	}
	g_hr = g_d3dDevice->CreateGeometryShader(geometryShaderBlob->GetBufferPointer(), geometryShaderBlob->GetBufferSize(), nullptr, &g_d3dGeometryShader);
	if (FAILED(g_hr)) return false;

	SafeRelease(geometryShaderBlob);

	ID3DBlob* pixelShaderBlob;

	g_hr = D3DCompileFromFile(
		L"C:/Users/Konrad/Documents/DX11Samples/Sample3 - compute shader/Sample0/color.hlsl", NULL, NULL, "PS", "ps_5_0",
		shaderFlags, 0, &pixelShaderBlob, &errorBlob);
	if (FAILED(g_hr))
	{
		D3DWriteBlobToFile(errorBlob, L"shit.txt", true);
		SafeRelease(errorBlob);
		return false;
	}
	g_hr = g_d3dDevice->CreatePixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize(), nullptr, &g_d3dPixelShader);
	if (FAILED(g_hr)) return false;

	
	SafeRelease(pixelShaderBlob);

	return true;
}

void PreRendering()
{
	static const float aWH = static_cast<float>(screenWidth) / static_cast<float>(screenHeight);

	g_worldMatrix = XMMatrixIdentity();
	g_projectionMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.f), aWH, 0.1f, 10.f);

	g_d3dDeviceContext->CSSetConstantBuffers(0, 1, &g_d3dConstantBuffer);
	g_d3dDeviceContext->GSSetConstantBuffers(0, 1, &g_d3dConstantBuffer);

	g_d3dDeviceContext->RSSetState(g_d3dRasterizerState);
	g_d3dDeviceContext->RSSetViewports(1, &g_Viewport);

	g_d3dDeviceContext->CSSetUnorderedAccessViews(0, 1, &g_d3dParticleView, nullptr);
}

void PreBoxDraw()
{
	g_d3dDeviceContext->IASetInputLayout(g_d3dInputLayout);
	g_d3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	g_d3dDeviceContext->CSSetShader(g_d3dComputeShader, nullptr, 0);
	g_d3dDeviceContext->VSSetShader(g_d3dVertexShader, nullptr, 0);
	g_d3dDeviceContext->GSSetShader(g_d3dGeometryShader, nullptr, 0);
	g_d3dDeviceContext->PSSetShader(g_d3dPixelShader, nullptr, 0);

	g_d3dDeviceContext->OMSetRenderTargets(1, &g_d3dRenderTargetView, g_d3dDepthStencilView);
	g_d3dDeviceContext->OMSetDepthStencilState(g_d3dDepthStencilState, 1);
}

void DrawBox()
{
	g_d3dDeviceContext->ClearRenderTargetView(g_d3dRenderTargetView, Colors::Blue);
	g_d3dDeviceContext->ClearDepthStencilView(g_d3dDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

	ConstBuffer cb;
	cb.matrix = g_worldMatrix * g_viewMatrix * g_projectionMatrix;
	cb.time = Timer::GetDeltaTime();

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	SecureZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	g_d3dDeviceContext->Map(g_d3dConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy(mappedResource.pData, &cb, sizeof(cb));
	g_d3dDeviceContext->Unmap(g_d3dConstantBuffer, 0);

	const UINT vertexStride = sizeof(XMFLOAT3);
	const UINT offset = 0;
	ID3D11Buffer* l_nullBuffer = NULL;
	g_d3dDeviceContext->IASetVertexBuffers(0, 1, &l_nullBuffer, &vertexStride, &offset);
	g_d3dDeviceContext->CSSetUnorderedAccessViews(1, 1, &g_d3dVertexView, nullptr);
	g_d3dDeviceContext->Dispatch(1, 1, 1);

	ID3D11UnorderedAccessView* l_nullUAV = NULL;
	g_d3dDeviceContext->CSSetUnorderedAccessViews(1, 1, &l_nullUAV, nullptr);
	
	g_d3dDeviceContext->IASetVertexBuffers(0, 1, &g_d3dVertexBuffer, &vertexStride, &offset);

	g_d3dDeviceContext->Draw(particleCount, 0);
	g_d3dSwapChain->Present(0, 0);
}
void PostRendering()
{
	SafeRelease(g_d3dConstantBuffer);
	SafeRelease(g_d3dInputLayout);

	SafeRelease(g_d3dVertexBuffer);
	SafeRelease(g_d3dParticleView);
	SafeRelease(g_d3dVertexView);
	SafeRelease(g_d3dParticleBuffer);

	SafeRelease(g_d3dVertexShader);
	SafeRelease(g_d3dGeometryShader);
	SafeRelease(g_d3dPixelShader);
	SafeRelease(g_d3dComputeShader);

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
		!LoadShaders())
		return 0;
	Timer::Setup();
	return Run();
}