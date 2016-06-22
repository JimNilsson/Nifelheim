#ifndef _STRUCTS_H_
#define _STRUCTS_H_

#include <DirectXMath.h>
#include "DebugLogger.h"
#include <vector>
#include <unordered_map>
#include <SDL_keycode.h>
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
	bool operator==(const Mesh& rhs)
	{
		return vertexBuffer == rhs.vertexBuffer;
	}
};

struct TransformCache
{
	TransformCache()
	{
		DirectX::XMStoreFloat4x4(&world, DirectX::XMMatrixIdentity());
		DirectX::XMStoreFloat4x4(&translation, DirectX::XMMatrixIdentity());
		DirectX::XMStoreFloat4x4(&scale, DirectX::XMMatrixIdentity());
		DirectX::XMStoreFloat4x4(&rotation, DirectX::XMMatrixIdentity());
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

enum TextureTypes
{
	TT_DIFFUSE,
	TT_NORMAL,
	TT_ROUGHNESS,
	TT_METALLIC,
	TT_COUNT
};

struct Textures
{
	int textures[TextureTypes::TT_COUNT] = { -1 };
	bool operator==(const Textures& rhs)
	{
		for (int i = 0; i < TextureTypes::TT_COUNT; ++i)
		{
			if (textures[i] != rhs.textures[i])
				return false;
		}
		return true;
	}
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
	DirectX::XMFLOAT4X4 InvProj;
	DirectX::XMFLOAT4 CamPos;
};

struct PerObjectBuffer
{
	DirectX::XMFLOAT4X4 WVP;
	DirectX::XMFLOAT4X4 WorldViewInvTrp;
	DirectX::XMFLOAT4X4 World;
	DirectX::XMFLOAT4X4 WorldView;
};

struct RenderJob
{
	Mesh mesh;
	DirectX::XMFLOAT4X4 transform;
	Textures textures;
};

struct Job
{
	Mesh mesh;
	Textures textures;
	bool operator==(const Job& rhs)
	{
		return mesh == rhs.mesh && textures == rhs.textures;
	}
};

struct Batch
{
	Job job;
	DirectX::XMFLOAT4X4* transforms = nullptr;
	unsigned jobCount = 0;
};

enum Components
{
	TRANSFORM,
	MESH,
	TERRAIN,
	MATERIAL,
	TEXTURES,
	LIGHTSOURCE,
	COMPONENT_COUNT
};

class GameObject
{
public:
	int id;
	int components[Components::COMPONENT_COUNT];
	GameObject()
	{
		id = GameObject::GenerateID();
		memset(components, -1, sizeof(int) * Components::COMPONENT_COUNT);

	}
	GameObject(const GameObject& other)
	{
		id = other.id;
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

enum KeyCodes
{
	KEY_0 = SDLK_0,
	KEY_1 = SDLK_1,
	KEY_2 = SDLK_2,
	KEY_3 = SDLK_3,
	KEY_4 = SDLK_4,
	KEY_5 = SDLK_5,
	KEY_6 = SDLK_6,
	KEY_7 = SDLK_7,
	KEY_8 = SDLK_8,
	KEY_9 = SDLK_9,
	KEY_A = SDLK_a,
	KEY_B = SDLK_b,
	KEY_C = SDLK_c,
	KEY_D = SDLK_d,
	KEY_E = SDLK_e,
	KEY_F = SDLK_f,
	KEY_G = SDLK_g,
	KEY_H = SDLK_h,
	KEY_I = SDLK_i,
	KEY_J = SDLK_j,
	KEY_K = SDLK_k,
	KEY_L = SDLK_l,
	KEY_M = SDLK_m,
	KEY_N = SDLK_n,
	KEY_O = SDLK_o,
	KEY_P = SDLK_p,
	KEY_Q = SDLK_q,
	KEY_R = SDLK_r,
	KEY_S = SDLK_s,
	KEY_T = SDLK_t,
	KEY_U = SDLK_u,
	KEY_V = SDLK_v,
	KEY_W = SDLK_w,
	KEY_X = SDLK_x,
	KEY_Y = SDLK_y,
	KEY_Z = SDLK_z,
	KEY_NUMPAD_0 = SDLK_KP_0,
	KEY_NUMPAD_1 = SDLK_KP_1,
	KEY_NUMPAD_2 = SDLK_KP_2,
	KEY_NUMPAD_3 = SDLK_KP_3,
	KEY_NUMPAD_4 = SDLK_KP_4,
	KEY_NUMPAD_5 = SDLK_KP_5,
	KEY_NUMPAD_6 = SDLK_KP_6,
	KEY_NUMPAD_7 = SDLK_KP_7,
	KEY_NUMPAD_8 = SDLK_KP_8,
	KEY_NUMPAD_9 = SDLK_KP_9,
	KEY_NUMPAD_COMMA = SDLK_KP_COMMA,
	KEY_NUMPAD_ENTER = SDLK_KP_ENTER,
	KEY_NUMPAD_PLUS = SDLK_KP_PLUS,
	KEY_NUMPAD_MINUS = SDLK_KP_MINUS,
	KEY_NUMPAD_ASTERISK = SDLK_KP_MULTIPLY,
	KEY_NUMPAD_SLASH = SDLK_KP_DIVIDE,
	KEY_TAB = SDLK_TAB,
	KEY_LSHIFT = SDLK_LSHIFT,
	KEY_LCTRL = SDLK_LCTRL,
	KEY_LALT = SDLK_LALT,
	KEY_SPACE = SDLK_SPACE,
	KEY_ENTER = SDLK_RETURN,
	KEY_ARROW_UP = SDLK_UP,
	KEY_ARROW_DOWN = SDLK_DOWN,
	KEY_ARROW_LEFT = SDLK_LEFT,
	KEY_ARROW_RIGHT = SDLK_RIGHT,
	KEY_DELETE = SDLK_DELETE,
	KEY_INSERT = SDLK_INSERT,
	KEY_HOME = SDLK_HOME,
	KEY_END = SDLK_END,
	KEY_PGUP = SDLK_PAGEUP,
	KEY_PGDOWN = SDLK_PAGEDOWN,
	KEY_BACKSPACE = SDLK_BACKSPACE,
	KEY_ESCAPE = SDLK_ESCAPE
};

struct PointLight
{
	DirectX::XMFLOAT3 position = DirectX::XMFLOAT3(0.0f,0.0f,0.0f);
	float range = 0;	
	DirectX::XMFLOAT3 color = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	float intensity = 0;
};


struct LightComponent
{
	int pointLight = -1;
};

#endif

