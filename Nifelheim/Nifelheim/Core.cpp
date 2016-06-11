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
	_transformManager = nullptr;
	_textureManager = nullptr;
	_inputManager = nullptr;
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
	SAFE_DELETE(Core::GetInstance()->_textureManager);
	SAFE_DELETE(Core::GetInstance()->_inputManager);
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
	_textureManager = new TextureManager();
	_inputManager = new InputManager();

}

void Core::Update()
{
	_inputManager->Update();
	_d3d11->Draw();
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

TextureManager * Core::GetTextureManager() const
{
	return _textureManager;
}

InputManager * Core::GetInputManager() const
{
	return _inputManager;
}

const int Core::CreateGameObject()
{
	_gameObjects.push_back(GameObject());
	return _gameObjects.back().id;
}

const GameObject & Core::GetGameObject(int id) const
{
	int index = Core::GetInstance()->FindObjectIndex(id);
	if (index >= 0)
	{
		return _gameObjects[index];
	}
	else
	{
		throw std::exception("Nonexistant gameobject referenced.");
	}

}


const std::vector<GameObject>& Core::GetGameObjects() const
{
	return _gameObjects;
}
