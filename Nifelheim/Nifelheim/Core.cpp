#include "Core.h"
#include "Macros.h"
#include <exception>

Core* Core::_instance = nullptr;

Core::Core()
{
	_window = nullptr;
	_d3d11 = nullptr;
	_meshManager = nullptr;
	_cameraManager = nullptr;
}
Core::~Core()
{

}
unsigned Core::FindObjectIndex(ObjectID id)
{
	unsigned size = _gameObjects.size();
	unsigned lower = 0;
	unsigned upper = size - 1;
	unsigned index = size / 2;
	while (_gameObjects[index].id != id && lower != upper)
	{
		if (_gameObjects[index].id > id)
		{
			upper = index;
			index = (lower + upper) / 2;
		}
		else if (_gameObjects[index].id < id)
		{
			lower = index;
			index = (lower + upper) / 2;
		}
	}
	return _gameObjects[index].id == id ? index : -1;
}
void Core::CreateInstance()
{
	if (!_instance)
		_instance = new Core();
}
Core* Core::GetInstance()
{
	return _instance;
}

void Core::ShutDown()
{
	SAFE_DELETE(Core::GetInstance()->_window);
	SAFE_DELETE(Core::GetInstance()->_d3d11);
	SAFE_DELETE(Core::GetInstance()->_meshManager);
	SAFE_DELETE(Core::GetInstance()->_cameraManager);
	SAFE_DELETE(Core::GetInstance()->_transformManager);
	delete _instance;
	_instance = nullptr;
}

void Core::Init(uint32_t width, uint32_t height, bool fullscreen)
{
	_window = new Window();
	_d3d11 = new Direct3D11();
	_meshManager = new MeshManager();
	_cameraManager = new CameraManager();
	_transformManager = new TransformManager();

}

Window * Core::GetWindow() const
{
	return _window;
}

Direct3D11 * Core::GetDirect3D11() const
{
	return _d3d11;
}

MeshManager * Core::GetMeshManager() const
{
	return _meshManager;
}

CameraManager * Core::GetCameraManager() const
{
	return _cameraManager;
}

TransformManager * Core::GetTransformManager() const
{
	return _transformManager;
}

const ObjectID& Core::CreateGameObject()
{
	_gameObjects.push_back(GameObject());
	return _gameObjects.back().id;
}

void Core::GiveMesh(ObjectID gameObject, const std::string & filename)
{
	int index = FindObjectIndex(gameObject);
	if (index >= 0)
	{
		_gameObjects[index].components[Components::MESH] = _meshManager->LoadMesh(filename);
	}
	else
	{
		DebugLogger::AddMsg("Couldn't find gameobject to give mesh to...");
	}
}

void Core::GiveTransform(ObjectID gameObject, float posX, float posY, float posZ, float rotX, float rotY, float rotZ, float scaleX, float scaleY, float scaleZ)
{
	int index = FindObjectIndex(gameObject);
	if (index >= 0)
	{
		_gameObjects[index].components[Components::TRANSFORM] = _transformManager->CreateTransform(posX, posY, posZ, scaleX, scaleY, scaleZ, rotX, rotY, rotZ);
	}
	else
	{
		DebugLogger::AddMsg("Couldn't find gameobject to give transform to...");
	}
}

const std::vector<GameObject>& Core::GetGameObjects() const
{
	return _gameObjects;
}
