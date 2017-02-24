#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

#define GBUFFER_COUNT 4
#define SAFE_RELEASE(x) {if(x){ x->Release(); x = nullptr;}};
#define MAX_INSTANCES 128

#include <d3d11.h>
#include <d3dcompiler.h>
#include <vector>
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")


#include "Structs.h"

enum VertexShaders
{
	VS_STATIC_MESHES,
	VS_STATIC_MESHES_INSTANCED,
	VS_FULLSCREEN,
	VS_SKYBOX,
	VS_COUNT
};

enum GeometryShaders
{
	GS_SCALE_UV,
	GS_COUNT
};

enum PixelShaders
{
	PS_STATIC_MESHES,
	PS_FINAL,
	PS_SKYBOX,
	PS_COUNT
};

enum ComputeShaders
{
	CS_TILED_DEFERRED,
	CS_COUNT
};

enum InputLayouts
{
	IL_STATIC_MESHES,
	LAYOUT_COUNT
};

enum RenderTargets
{
	DIFFUSE,
	NORMAL,
	RT_COUNT
};

enum ConstantBuffers
{
	CB_PER_FRAME,
	CB_PER_OBJECT,
	CB_PER_INSTANCE,
	CB_LIGHTBUFFER,
	CB_SKYBOX,
	CB_COUNT
};

enum DSStates
{
	DS_LESS_EQUAL,
	DS_COUNT
};

enum Samplers
{
	ANISO,
	SAM_COUNT
};

enum RasterizerStates
{
	RS_CULL_NONE,
	RS_CULL_BACK,
	RS_WIREFRAME,
	RS_COUNT
};

struct LightBuffer
{
	PointLight pointLights[MAX_POINTLIGHTS];
	DirectionalLight dirLights[MAX_DIRLIGHTS];
	unsigned pointLightCount;
	unsigned dirLightCount;
	float pad1;
	float pad2;
};

struct SkyBuffer
{
	DirectX::XMFLOAT4 Campos;
	DirectX::XMFLOAT4X4 ViewProj;
};

struct DepthBuffer
{
	ID3D11DepthStencilView*   DSV         = nullptr;
	ID3D11DepthStencilView*   DSVReadOnly = nullptr;
	ID3D11ShaderResourceView* SRV         = nullptr;
	ID3D11Texture2D*          DSB         = nullptr;
	ID3D11Resource*           Texture     = nullptr;
};

class Direct3D11
{
private:
	ID3D11Device*                       _device = nullptr;
	ID3D11DeviceContext*                _deviceContext = nullptr;
	IDXGISwapChain*                     _swapChain = nullptr;
	ID3D11RenderTargetView*             _backbufferRTV = nullptr;

	DepthBuffer                         _depth;


	ID3D11VertexShader*     _vertexShaders[VertexShaders::VS_COUNT] = { nullptr };
	ID3D11GeometryShader*   _geometryShaders[GeometryShaders::GS_COUNT] = { nullptr };
	ID3D11PixelShader*      _pixelShaders[PixelShaders::PS_COUNT] = { nullptr };
	ID3D11ComputeShader*    _computeShaders[ComputeShaders::CS_COUNT] = { nullptr };
	
	ID3D11RenderTargetView*   _renderTargetViews[RenderTargets::RT_COUNT] = { nullptr };
	ID3D11ShaderResourceView* _shaderResourceViews[RenderTargets::RT_COUNT] = { nullptr }; //related to deferred
	ID3D11Texture2D*          _renderTargetTextures[RenderTargets::RT_COUNT] = { nullptr };

	ID3D11DepthStencilState* _depthStencilStates[DSStates::DS_COUNT] = { nullptr };
	
	std::vector<ID3D11ShaderResourceView*> _textures; //diffuse maps, normal maps, etc.
	std::vector<ID3D11Buffer*> _vertexBuffers;
	std::vector<ID3D11Buffer*> _indexBuffers;
	ID3D11Buffer* _constantBuffers[ConstantBuffers::CB_COUNT] = { nullptr };
	ID3D11InputLayout* _inputLayouts[InputLayouts::LAYOUT_COUNT] = { nullptr };
	ID3D11SamplerState* _samplerStates[Samplers::SAM_COUNT] = { nullptr };
	ID3D11RasterizerState* _rasterizerStates[RasterizerStates::RS_COUNT] = { nullptr };
	

	void _CreateShadersAndInputLayouts();
	void _CreateDepthBuffer();
	void _CreateDepthStencilState();
	void _CreateSamplerState();
	void _CreateViewPort();
	void _CreateRasterizerState();
	void _CreateConstantBuffers();

	int fuckballer;


public:
	Direct3D11();
	~Direct3D11();
	ID3D11Device* GetDevice() { return _device; }
	ID3D11DeviceContext* GetDeviceContext() { return _deviceContext; }

	int CreateVertexBuffer(Vertex* vertexData, unsigned vertexCount);
	int CreateIndexBuffer(unsigned* indexData, unsigned indexCount);
	int CreateTexture(const wchar_t* filename);
	int CreateTextureCube(const wchar_t* filename);

	void Draw();
	
};


#endif

