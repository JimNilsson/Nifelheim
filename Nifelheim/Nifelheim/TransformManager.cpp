#include "TransformManager.h"
#include "Core.h"

using namespace DirectX;

TransformManager::TransformManager()
{
}

TransformManager::~TransformManager()
{
}

const unsigned TransformManager::CreateTransform(const int gameObject, float posX, float posY, float posZ, float scaleX, float scaleY, float scaleZ, float rotX, float rotY, float rotZ)
{
	Transform trans;
	rotX *= 180.0f / XM_PI;
	rotY *= 180.0f / XM_PI;
	rotZ *= 180.0f / XM_PI;
	trans.translation = XMFLOAT3(posX, posY, posZ);
	trans.scale = XMFLOAT3(scaleX, scaleY, scaleZ);
	trans.rotation = XMFLOAT3(rotX, rotY, rotZ);
	_transforms.push_back(trans);
	
	
	XMMATRIX t = XMMatrixTranslation(posX, posY, posZ);
	XMMATRIX r = XMMatrixRotationRollPitchYaw(rotZ, rotX, rotY);
	XMMATRIX s = XMMatrixScaling(scaleX, scaleY, scaleZ);
	XMMATRIX world = s*r*t;

	TransformCache tc;
	XMStoreFloat4x4(&tc.world, world);
	XMStoreFloat4x4(&tc.scale, s);
	XMStoreFloat4x4(&tc.translation, t);
	XMStoreFloat4x4(&tc.rotation, r);
	_transformCache.push_back(tc);
	const GameObject& go = Core::GetInstance()->GetGameObject(gameObject);
	const_cast<GameObject&>(go).components[Components::TRANSFORM] = _transformCache.size() - 1;

	return _transformCache.size() - 1;
}

void TransformManager::Rotate(const int gameObject, float degX, float degY, float degZ)
{
	int id = Core::GetInstance()->GetGameObject(gameObject).components[Components::TRANSFORM];
	if( id >= 0)
		_Rotate(id, degX, degY, degZ);
}

void TransformManager::SetRotation(const int gameObject, float degX, float degY, float degZ)
{
	int id = Core::GetInstance()->GetGameObject(gameObject).components[Components::TRANSFORM];
	if (id >= 0)
		_SetRotation(id, degX, degY, degZ);
}

void TransformManager::Scale(const int gameObject, float x, float y, float z)
{
	int id = Core::GetInstance()->GetGameObject(gameObject).components[Components::TRANSFORM];
	if (id >= 0)
		_Scale(id, x, y, z);
}

void TransformManager::SetScale(const int gameObject, float x, float y, float z)
{
	int id = Core::GetInstance()->GetGameObject(gameObject).components[Components::TRANSFORM];
	if (id >= 0)
		_SetScale(id, x, y, z);
}

void TransformManager::Translate(const int gameObject, float x, float y, float z)
{
	int id = Core::GetInstance()->GetGameObject(gameObject).components[Components::TRANSFORM];
	if (id >= 0)
		_Translate(id, x, y, z);
}

void TransformManager::SetTranslation(const int gameObject, float x, float y, float z)
{
	int id = Core::GetInstance()->GetGameObject(gameObject).components[Components::TRANSFORM];
	if (id >= 0)
		_SetTranslation(id, x, y, z);
}

void TransformManager::BindChild(const int parent, const int child, bool bindTranslation, bool bindRotation, bool bindScale)
{
	int parentid = Core::GetInstance()->GetGameObject(parent).components[Components::TRANSFORM];
	int childid = Core::GetInstance()->GetGameObject(child).components[Components::TRANSFORM];
	if (parentid >= 0 && childid >= 0)
		_BindChild(parentid, childid, bindTranslation, bindRotation, bindScale);
}

void TransformManager::_Rotate(unsigned id, float degX, float degY, float degZ)
{
	float radX = degX * 180.0f / XM_PI;
	float radY = degY * 180.0f / XM_PI;
	float radZ = degZ * 180.0f / XM_PI;
	_transforms[id].rotation.x = _transforms[id].rotation.x + radX;
	_transforms[id].rotation.y = _transforms[id].rotation.y + radY;
	_transforms[id].rotation.z = _transforms[id].rotation.z + radZ;

	if (_transforms[id].rotation.x > XM_2PI)
		_transforms[id].rotation.x -= XM_2PI;
	if (_transforms[id].rotation.y > XM_2PI)
		_transforms[id].rotation.y -= XM_2PI;
	if (_transforms[id].rotation.z > XM_2PI)
		_transforms[id].rotation.z -= XM_2PI;

	for (auto &i : _transforms[id].children)
	{
		if (i.inheritRotation)
		{
			float diffX = _transforms[i.id].translation.x - _transforms[id].translation.x;
			float diffY = _transforms[i.id].translation.y - _transforms[id].translation.y;
			float diffZ = _transforms[i.id].translation.z - _transforms[id].translation.z;
			XMMATRIX t = XMMatrixTranslation(diffX, diffY, diffZ);
			XMMATRIX r = XMMatrixRotationRollPitchYaw(radX, radY, radZ);
			XMMATRIX t2 = XMLoadFloat4x4(&_transformCache[id].translation);
			XMVECTOR oldPos = XMLoadFloat3(&_transforms[i.id].translation);
			XMVectorSetW(oldPos, 1.0f);
			XMVECTOR newPos = XMVectorSet(0, 0, 0, 1.0f);
			newPos = XMVector3Transform(newPos, t * r * t2);
			XMVECTOR diff = newPos - oldPos;
			XMFLOAT3 childTranslation;
			XMStoreFloat3(&childTranslation, diff);
			_Translate(i.id, childTranslation.x, childTranslation.y, childTranslation.z);
			_Rotate(i.id, degX, degY, degZ);
			_UpdateCache(i.id);
		}
	}

	_UpdateCache(id);
}

void TransformManager::_SetRotation(unsigned id, float degX, float degY, float degZ)
{
	float radX = degX * 180.0f / XM_PI;
	float radY = degX * 180.0f / XM_PI;
	float radZ = degX * 180.0f / XM_PI;
	_transforms[id].rotation.x = radX; 
	_transforms[id].rotation.y = radY;
	_transforms[id].rotation.z = radZ;
	for (auto &i : _transforms[id].children)
	{
		if (i.inheritRotation)
		{
			float diffX = _transforms[i.id].translation.x - _transforms[id].translation.x;
			float diffY = _transforms[i.id].translation.y - _transforms[id].translation.y;
			float diffZ = _transforms[i.id].translation.z - _transforms[id].translation.z;
			XMMATRIX t = XMMatrixTranslation(diffX, diffY, diffZ);
			XMMATRIX r = XMMatrixRotationRollPitchYaw(radX, radY, radZ);
			XMMATRIX t2 = XMLoadFloat4x4(&_transformCache[id].translation);
			XMVECTOR oldPos = XMLoadFloat3(&_transforms[i.id].translation);
			XMVectorSetW(oldPos, 1.0f);
			XMVECTOR newPos = XMVectorSet(0, 0, 0, 1.0f);
			newPos = XMVector3Transform(newPos, t * r * t2);
			XMVECTOR diff = newPos - oldPos;
			XMFLOAT3 childTranslation;
			XMStoreFloat3(&childTranslation, diff);
			_Translate(i.id, childTranslation.x, childTranslation.y, childTranslation.z);
			
			float rotDiffX = (radX - _transforms[i.id].rotation.x) * XM_PI / 180.0f;
			float rotDiffY = (radY - _transforms[i.id].rotation.y) * XM_PI / 180.0f;
			float rotDiffZ = (radZ - _transforms[i.id].rotation.z) * XM_PI / 180.0f;
			_Rotate(i.id, rotDiffX, rotDiffY, rotDiffZ);

			_UpdateCache(i.id);
		}
	}
	_UpdateCache(id);
}

void TransformManager::_Scale(unsigned id, float x, float y, float z)
{
	_transforms[id].scale.x *= x;
	_transforms[id].scale.y *= y;
	_transforms[id].scale.z *= z;
	for (auto &i : _transforms[id].children)
	{
		if (i.inheritScale)
		{
			_Scale(i.id, x, y, z);
			_UpdateCache(i.id);
		}
	}
	_UpdateCache(id);
}

void TransformManager::_SetScale(unsigned id, float x, float y, float z)
{
	_transforms[id].scale.x = x;
	_transforms[id].scale.y = y;
	_transforms[id].scale.z = z;
	for (auto &i : _transforms[id].children)
	{
		if (i.inheritScale)
		{
			_SetScale(i.id, x, y, z);
			_UpdateCache(i.id);
		}
	}
	_UpdateCache(id);
}

void TransformManager::_Translate(unsigned id, float x, float y, float z)
{
	_transforms[id].translation.x += x;
	_transforms[id].translation.y += y;
	_transforms[id].translation.z += z;
	for (auto &i : _transforms[id].children)
	{
		if (i.inheritTranslation)
		{
			_Translate(i.id, x, y, z);
			_UpdateCache(i.id);
		}
	}
	_UpdateCache(id);
}

void TransformManager::_SetTranslation(unsigned id, float x, float y, float z)
{
	XMFLOAT3 previous = _transforms[id].translation;
	_transforms[id].translation.x = x;
	_transforms[id].translation.y = y;
	_transforms[id].translation.z = z;
	float diffX = x - previous.x;
	float diffY = y - previous.y;
	float diffZ = z - previous.z;
	for (auto &i : _transforms[id].children)
	{
		if (i.inheritTranslation)
		{
			_Translate(i.id, diffX, diffY, diffZ);
			_UpdateCache(i.id);
		}
	}
	_UpdateCache(id);
}

void TransformManager::_BindChild(unsigned parent, unsigned child, bool bindTranslation, bool bindRotation, bool bindScale)
{
	_transforms[parent].children.push_back(ChildTransform(child, bindTranslation, bindRotation, bindScale));
}

const TransformCache & TransformManager::GetTransformBuffer(unsigned id) const
{
	if (id < _transformCache.size())
		return _transformCache[id];
	else
	{
		DebugLogger::AddMsg("Nonexistant transform referenced.");
		return TransformCache();
	}
}

void TransformManager::_UpdateCache(unsigned id)
{
	XMMATRIX t = XMMatrixTranslation(_transforms[id].translation.x,
		_transforms[id].translation.y,
		_transforms[id].translation.z);
	XMMATRIX r = XMMatrixRotationRollPitchYaw(_transforms[id].rotation.x,
		_transforms[id].rotation.y,
		_transforms[id].rotation.z);
	XMMATRIX s = XMMatrixScaling(_transforms[id].scale.x,
		_transforms[id].scale.y,
		_transforms[id].scale.z);
	XMMATRIX world = s * r * t;
	XMStoreFloat4x4(&_transformCache[id].world, world);
	XMStoreFloat4x4(&_transformCache[id].scale, s);
	XMStoreFloat4x4(&_transformCache[id].rotation, r);
	XMStoreFloat4x4(&_transformCache[id].translation, t);
}
