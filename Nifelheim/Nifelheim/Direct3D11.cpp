#include "Direct3D11.h"
#include "Core.h"
#include "DebugLogger.h"
#include <exception>





Direct3D11::Direct3D11()
{
	DXGI_SWAP_CHAIN_DESC scd;
	ZeroMemory(&scd, sizeof(scd));
	const Core const* core = Core::GetInstance();
	scd.BufferCount = 1;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.OutputWindow = core->GetWindow()->GetHandle();
	scd.SampleDesc.Count = 1; //We will be using deferred lighting
	scd.Windowed = TRUE; //Why BOOL is an int is probably related to working in C
	
	HRESULT hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_DEBUG, NULL, NULL, D3D11_SDK_VERSION, &scd, &_swapChain, &_device, NULL, &_deviceContext);
	if (FAILED(hr))
		throw std::exception("Failed to create device and swapchain");

	////Set up GBuffers for deferred shading
	D3D11_TEXTURE2D_DESC td;
	ZeroMemory(&td, sizeof(td));
	td.Width = core->GetWindow()->GetWidth();
	td.Height = core->GetWindow()->GetHeight();
	td.MipLevels = 1;
	td.ArraySize = 1;
	td.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	td.SampleDesc.Count = 1;
	td.Usage = D3D11_USAGE_DEFAULT;
	td.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	for (int i = 0; i < RenderTargets::RT_COUNT; ++i)
	{
		_device->CreateTexture2D(&td, nullptr, &_renderTargetTextures[i]);
	}

	D3D11_RENDER_TARGET_VIEW_DESC rtvd;
	ZeroMemory(&rtvd, sizeof(rtvd));
	rtvd.Format = td.Format;
	rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvd.Texture2D.MipSlice = 0;

	for (int i = 0; i < RenderTargets::RT_COUNT; ++i)
	{
		_device->CreateRenderTargetView(_renderTargetTextures[i], &rtvd, &_renderTargetViews[i]);
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	ZeroMemory(&srvd, sizeof(srvd));
	srvd.Format = td.Format;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvd.Texture2D.MostDetailedMip = 0;
	srvd.Texture2D.MipLevels = 1;

	for (int i = 0; i < RenderTargets::RT_COUNT; ++i)
	{
		_device->CreateShaderResourceView(_renderTargetTextures[i], &srvd, &_shaderResourceViews[i]);
	}

	ID3D11Buffer* backbuffer;
	rtvd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backbuffer);
	_device->CreateRenderTargetView(backbuffer, &rtvd, &_backbufferRTV);
	SAFE_RELEASE(backbuffer);

	_CreateShadersAndInputLayouts();
	_CreateDepthBuffer();
	_CreateSamplerState();
	_CreateViewPort();
	_CreateRasterizerState();
}

Direct3D11::~Direct3D11()
{

	for (auto &i : _vertexShaders)
	{
		SAFE_RELEASE(i);
	}
	//for (auto &i : _geometryShaders)
	//{
	//	SAFE_RELEASE(i);
	//}
	for (auto &i : _pixelShaders)
	{
		SAFE_RELEASE(i);
	}
	for (auto &i : _computeShaders)
	{
		SAFE_RELEASE(i);
	}
	for (auto &i : _renderTargetViews)
	{
		SAFE_RELEASE(i);
	}
	for (auto &i : _renderTargetTextures)
	{
		SAFE_RELEASE(i);
	}
	for (auto &i : _shaderResourceViews)
	{
		SAFE_RELEASE(i);
	}
	for (auto &i : _vertexBuffers)
	{
		SAFE_RELEASE(i);
	}
	for (auto &i : _indexBuffers)
	{
		SAFE_RELEASE(i);
	}
	for (auto &i : _inputLayouts)
	{
		SAFE_RELEASE(i);
	}
	for (auto &i : _samplerStates)
	{
		SAFE_RELEASE(i);
	}
	for (auto &i : _constantBuffers)
	{
		SAFE_RELEASE(i);
	}
	for (auto &i : _rasterizerStates)
	{
		SAFE_RELEASE(i);
	}

	SAFE_RELEASE(_depth.DSB);
	SAFE_RELEASE(_depth.DSV);
	SAFE_RELEASE(_depth.DSVReadOnly);
	SAFE_RELEASE(_depth.SRV);
	SAFE_RELEASE(_depth.Texture);

	SAFE_RELEASE(_backbufferRTV);
	SAFE_RELEASE(_swapChain);
	SAFE_RELEASE(_deviceContext);
	if (_device)
	{
		uint32_t refCount = _device->Release();
		if (refCount > 0)
		{
			DebugLogger::AddMsg("Unreleased COM-objects detected. :(");
		}
	}
}

int Direct3D11::CreateVertexBuffer(Vertex * vertexData, unsigned vertexCount)
{
	D3D11_BUFFER_DESC bd;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.ByteWidth = sizeof(Vertex) * vertexCount;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	bd.StructureByteStride = 0;
	bd.Usage = D3D11_USAGE_DEFAULT;

	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(data));
	data.pSysMem = &vertexData[0];
	ID3D11Buffer* buffer = nullptr;
	_vertexBuffers.push_back(buffer);
	HRESULT hr = _device->CreateBuffer(&bd, &data, &_vertexBuffers.back());
	if (FAILED(hr))
		DebugLogger::AddMsg("Failed to create vertex buffer");
	return _vertexBuffers.size() - 1;
}

int Direct3D11::CreateIndexBuffer(unsigned * indexData, unsigned indexCount)
{
	D3D11_BUFFER_DESC bd;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.ByteWidth = sizeof(unsigned) * indexCount;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	bd.StructureByteStride = 0;
	bd.Usage = D3D11_USAGE_DEFAULT;

	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(data));
	data.pSysMem = &indexData[0];
	ID3D11Buffer* buffer = nullptr;
	_indexBuffers.push_back(buffer);
	HRESULT hr = _device->CreateBuffer(&bd, &data, &_indexBuffers.back());
	if (FAILED(hr))
		DebugLogger::AddMsg("Failed to create index buffer");
	return _vertexBuffers.size() - 1;

	return 0;
}

void Direct3D11::Draw()
{
	const Core const * core = Core::GetInstance();
	float clearColor[] = { 0.0f,0.0f,0.0f,0.0f };

	for (auto &rtv : _renderTargetViews)
	{
		_deviceContext->ClearRenderTargetView(rtv, clearColor);
	}
	_deviceContext->ClearRenderTargetView(_backbufferRTV, clearColor);
	_deviceContext->ClearDepthStencilView(_depth.DSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
//	_deviceContext->ClearDepthStencilView(_depth.DSVReadOnly, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	_deviceContext->IASetInputLayout(_inputLayouts[InputLayouts::IL_STATIC_MESHES]);
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	_deviceContext->IASetVertexBuffers(0, 1, &_vertexBuffers[0], &stride, &offset);
	_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	_deviceContext->VSSetShader(_vertexShaders[VertexShaders::VS_STATIC_MESHES], nullptr, 0);
	_deviceContext->PSSetShader(_pixelShaders[PixelShaders::PS_STATIC_MESHES], nullptr, 0);
	_deviceContext->OMSetRenderTargets(RenderTargets::RT_COUNT, &_renderTargetViews[0], _depth.DSV);
	
	PerFrameBuffer pfb;
	core->GetCameraManager()->FillPerFrameBuffer(pfb);
	
	_deviceContext->Draw(36, 0);

	_deviceContext->IASetInputLayout(nullptr);
	_deviceContext->VSSetShader(_vertexShaders[VertexShaders::VS_FULLSCREEN], nullptr, 0);
	_deviceContext->PSSetShader(_pixelShaders[PixelShaders::PS_FINAL], nullptr, 0);
	_deviceContext->OMSetRenderTargets(1, &_backbufferRTV, nullptr);
	_deviceContext->PSSetShaderResources(0, 1, &_shaderResourceViews[RenderTargets::DIFFUSE]);
	_deviceContext->Draw(3, 0);
	

	_swapChain->Present(0, 0);
}

void Direct3D11::_CreateShadersAndInputLayouts()
{
	ID3DBlob* pVS;
	D3DCompileFromFile(L"StaticMeshVS.hlsl",nullptr,nullptr,"main","vs_4_0",
		NULL, NULL, &pVS, nullptr);
	_device->CreateVertexShader(pVS->GetBufferPointer(), pVS->GetBufferSize(), nullptr, &_vertexShaders[VertexShaders::VS_STATIC_MESHES]);

	D3D11_INPUT_ELEMENT_DESC id[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TANGENT",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	_device->CreateInputLayout(id, ARRAYSIZE(id), pVS->GetBufferPointer(), pVS->GetBufferSize(), &_inputLayouts[InputLayouts::IL_STATIC_MESHES]);
	SAFE_RELEASE(pVS);

	D3DCompileFromFile(L"FinalVS.hlsl", nullptr, nullptr, "main", "vs_4_0",
		NULL, NULL, &pVS, nullptr);
	_device->CreateVertexShader(pVS->GetBufferPointer(), pVS->GetBufferSize(), nullptr, &_vertexShaders[VertexShaders::VS_FULLSCREEN]);

	D3DCompileFromFile(L"StaticMeshPS.hlsl", nullptr, nullptr, "main", "ps_4_0",
		NULL, NULL, &pVS, nullptr);
	_device->CreatePixelShader(pVS->GetBufferPointer(), pVS->GetBufferSize(), nullptr, &_pixelShaders[PixelShaders::PS_STATIC_MESHES]);
	SAFE_RELEASE(pVS);

	D3DCompileFromFile(L"FinalPS.hlsl", nullptr, nullptr, "main", "ps_4_0",
		NULL, NULL, &pVS, nullptr);
	_device->CreatePixelShader(pVS->GetBufferPointer(), pVS->GetBufferSize(), nullptr, &_pixelShaders[PixelShaders::PS_FINAL]);
	SAFE_RELEASE(pVS);


}

void Direct3D11::_CreateDepthBuffer()
{
	const Core const* core = Core::GetInstance();
	D3D11_TEXTURE2D_DESC dsd;
	ZeroMemory(&dsd, sizeof(dsd));
	dsd.Width = core->GetWindow()->GetWidth();
	dsd.Height = core->GetWindow()->GetHeight();
	dsd.MipLevels = 1;
	dsd.ArraySize = 1;
	dsd.Format = DXGI_FORMAT_R24G8_TYPELESS;
	dsd.SampleDesc.Count = 1;
	dsd.SampleDesc.Quality = 0;
	dsd.Usage = D3D11_USAGE_DEFAULT;
	dsd.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	dsd.CPUAccessFlags = 0;
	dsd.MiscFlags = 0;

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
	ZeroMemory(&dsvd, sizeof(dsvd));
	dsvd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvd.Texture2D.MipSlice = 0;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	ZeroMemory(&srvd, sizeof(srvd));
	srvd.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvd.Texture2D.MostDetailedMip = 0;
	srvd.Texture2D.MipLevels = -1;

	
	_device->CreateTexture2D(&dsd, 0, &_depth.DSB);
	_device->CreateDepthStencilView(_depth.DSB, &dsvd, &_depth.DSV);
	_device->CreateShaderResourceView(_depth.DSB, &srvd, &_depth.SRV);

}

void Direct3D11::_CreateSamplerState()
{
	D3D11_SAMPLER_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sd.MaxAnisotropy = 16;
	sd.Filter = D3D11_FILTER_ANISOTROPIC;
	sd.MinLOD = -FLT_MAX;
	sd.MaxLOD = FLT_MAX;
	sd.MipLODBias = 0.0f;
	_device->CreateSamplerState(&sd, &_samplerStates[Samplers::ANISO]);
	_deviceContext->PSSetSamplers(0, 1, &_samplerStates[Samplers::ANISO]);
}

void Direct3D11::_CreateViewPort()
{
	const Core const* core = Core::GetInstance();
	D3D11_VIEWPORT vp;
	vp.Width = core->GetWindow()->GetWidth();
	vp.Height = core->GetWindow()->GetHeight();
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	_deviceContext->RSSetViewports(1, &vp);
}

void Direct3D11::_CreateRasterizerState()
{
	D3D11_RASTERIZER_DESC rd;
	rd.AntialiasedLineEnable = false;
	rd.CullMode = D3D11_CULL_BACK;
	rd.FrontCounterClockwise = true;
	rd.DepthBias = false;
	rd.SlopeScaledDepthBias = 0.0f;
	rd.DepthClipEnable = true;
	rd.DepthBiasClamp = 0.0f;
	rd.FillMode = D3D11_FILL_SOLID;
	rd.MultisampleEnable = false;
	rd.ScissorEnable = false;

	_device->CreateRasterizerState(&rd, &_rasterizerStates[RasterizerStates::RS_CULL_BACK]);

	rd.CullMode = D3D11_CULL_NONE;

	_device->CreateRasterizerState(&rd, &_rasterizerStates[RasterizerStates::RS_CULL_NONE]);

	_deviceContext->RSSetState(_rasterizerStates[RasterizerStates::RS_CULL_NONE]);
}
