#ifndef _STRUCTS_H_
#define _STRUCTS_H_

#include <DirectXMath.h>
#include "DebugLogger.h"
#include <vector>
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
	int vertexCount = -1;
};

struct TransformCache
{
	TransformCache()
	{
		DirectX::XMStoreFloat4x4(&world, DirectX::XMMatrixIdentity());
	}
	~TransformCache() {}
	DirectX::XMFLOAT4X4 world;
	DirectX::XMFLOAT4X4 translation;
	DirectX::XMFLOAT4X4 rotation;
	DirectX::XMFLOAT4X4 scale;
};

struct ChildTransform
{
	bool inheritTranslation;
	bool inheritRotation;
	bool inheritScale;
	unsigned id;
	ChildTransform(unsigned id, bool trans, bool rot, bool scale)
	{
		this->id = id;
		inheritTranslation = trans;
		inheritRotation = rot;
		inheritScale = scale;
	}
};

struct Transform
{
	DirectX::XMFLOAT3 translation;
	DirectX::XMFLOAT3 rotation;
	DirectX::XMFLOAT3 scale;
	std::vector<ChildTransform> children;
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

struct PerObjectBuffer
{
	DirectX::XMFLOAT4X4 WVP;
	DirectX::XMFLOAT4X4 WorldViewInvTrp;
	DirectX::XMFLOAT4X4 World;
	DirectX::XMFLOAT4X4 WorldView;
	DirectX::XMFLOAT4X4 WorldInvTrp;
};

enum Components
{
	TRANSFORM,
	MESH,
	MATERIAL,
	COMPONENT_COUNT
};

class GameObject
{
public:
	int id;
	int components[Components::COMPONENT_COUNT] = { -1 };
	GameObject()
	{
		id = GameObject::GenerateID();
	}
	GameObject(const GameObject& other)
	{
		id = GameObject::GenerateID();
		for (int i = 0; i < Components::COMPONENT_COUNT; ++i)
		{
			components[i] = other.components[i];
		}
	}
	const GameObject& operator=(const GameObject& rhs)
	{
		//id should remain unchanged
		for (int i = 0; i < Components::COMPONENT_COUNT; ++i)
		{
			components[i] = rhs.components[i];
		}
		return *this;
	}
private:
	static int GenerateID()
	{
		static unsigned oid = 0;
		return oid++;
	}
};



#endif

