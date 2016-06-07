#ifndef _STRUCTS_H_
#define _STRUCTS_H_

#include <DirectXMath.h>
#include "DebugLogger.h"

struct Vertex
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT4 tangent; //w is used to compute bitan
	DirectX::XMFLOAT2 texcoord;
};

struct RawMeshData
{
	Vertex* vertices = nullptr;
	unsigned* indices = nullptr;
	unsigned vertexCount = 0;
	unsigned indexCount = 0;
};

struct Mesh
{
	int vertexBuffer = -1;
	int indexBuffer = -1;
	int indexCount = -1;
};

struct Transform
{
	Transform()
	{
		DirectX::XMStoreFloat4x4(&world, DirectX::XMMatrixIdentity());
	}
	~Transform() {}
	DirectX::XMFLOAT4X4 world;
};

struct Material
{
	float roughness = 1.0f;
	float metallic = 1.0f;
};

struct Textures
{
	int diffuse = -1;
	int normal = -1;
	int roughness = -1;
	int metallic = -1;
};

struct Camera
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 forward;
	DirectX::XMFLOAT3 up;
	float fov;
	float aspectRatio;
	float nearPlane;
	float farPlane;
};

struct PerFrameBuffer
{
	DirectX::XMFLOAT4X4 View;
	DirectX::XMFLOAT4X4 Proj;
	DirectX::XMFLOAT4X4 ViewProj;
	DirectX::XMFLOAT4X4 InvView;
	DirectX::XMFLOAT4X4 InvViewProj;
	DirectX::XMFLOAT4 CamPos;
};

struct Renderable
{
	Mesh mesh;
	Transform transform;
	Material material;
	Textures textures;
};



#endif

