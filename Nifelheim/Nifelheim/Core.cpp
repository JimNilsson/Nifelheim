#include "Core.h"
#include "Macros.h"
#include <exception>
#include <map>

Core* Core::_instance = nullptr;

Core::Core()
{
	if (SDL_Init(0) < 0)
	{
		throw(std::runtime_error("COuld not init SDL"));
	}
	_window = nullptr;
	_d3d11 = nullptr;
	_meshManager = nullptr;
	_cameraManager = nullptr;
	_transformManager = nullptr;
	_textureManager = nullptr;
	_inputManager = nullptr;
	_timer = nullptr;
	_audioManager = nullptr;
}
Core::~Core()
{
	SDL_Quit();
}
unsigned Core::FindObjectIndex(ObjectID id)
{
	unsigned size = _gameObjects.size();
	unsigned lower = 0;
	unsigned upper = size - 1;
	unsigned index;
	while (lower <= upper)
	{
		index = (lower + upper) / 2;
		if (_gameObjects[index].id == id)
		{
			return index;
		}
		else if (_gameObjects[index].id > id)
		{
			upper = index - 1;
		}
		else if (_gameObjects[index].id < id)
		{
			lower = index + 1;
		}
	}
	return -1;
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
	SAFE_DELETE(Core::GetInstance()->_lightManager);
	SAFE_DELETE(Core::GetInstance()->_timer);
	SAFE_DELETE(Core::GetInstance()->_audioManager);
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
	_lightManager = new LightManager();
	_timer = new Timer();
	_audioManager = new AudioManager();

}

void Core::Update()
{
	_timer->Update();
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

LightManager * Core::GetLightManager() const
{
	return _lightManager;
}

AudioManager * Core::GetAudioManager() const
{
	return _audioManager;
}

Timer * Core::GetTimer() const
{
	return _timer;
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


void Core::GetRenderBatches(std::vector<Batch>& meshbatches) const
{

	int size = _gameObjects.size();
	int remaining = size;
	int* indices = new int[size];
	for (int i = 0; i < size; ++i)
	{
		if (_gameObjects[i].components[Components::MESH] >= 0)
			indices[i] = i;
		else
			indices[i] = -1;
	}

	for (int i = 0; i < size; ++i)
	{	
		if (indices[i] >= 0)
		{
			std::vector<DirectX::XMFLOAT4X4> transforms;
			transforms.reserve(remaining);

			meshbatches.push_back(Batch());
			Batch& b = meshbatches.back();
			b.jobCount = 0;
			Job job;

			job.mesh = _meshManager->GetMesh(_gameObjects[indices[i]].components[Components::MESH]);
			job.textures = _textureManager->GetTextures(_gameObjects[indices[i]].components[Components::TEXTURES]);
			for (int j = i; j < size && b.jobCount < MAX_INSTANCES; ++j)
			{
				if (indices[j] >= 0)
				{
					Job compJob;
					compJob.mesh = _meshManager->GetMesh(_gameObjects[indices[j]].components[Components::MESH]);
					compJob.textures = _textureManager->GetTextures(_gameObjects[indices[j]].components[Components::TEXTURES]);
					if (job == compJob)
					{
						++b.jobCount;
						transforms.push_back(_transformManager->GetWorld(_gameObjects[indices[j]].components[Components::TRANSFORM]));
						indices[j] = -1;
					}
				}
				
			}
			remaining -= b.jobCount;
			b.job = job;
			//caller's responsibility to clean up.
			b.transforms = new DirectX::XMFLOAT4X4[b.jobCount];
			memcpy(b.transforms, &transforms[0], sizeof(DirectX::XMFLOAT4X4) * b.jobCount);
		}
	}
	delete[] indices;

	for (const auto& go : _gameObjects)
	{
		if (go.components[Components::TERRAIN] >= 0)
		{
			std::vector<Mesh> terrainMeshes;
			DirectX::XMFLOAT4X4 worldf =_transformManager->GetWorld(go.components[Components::TRANSFORM]);
			DirectX::XMMATRIX world = XMLoadFloat4x4(&worldf);
			_meshManager->GetTerrain(go.components[Components::TERRAIN], terrainMeshes,world);
			for (const auto& m : terrainMeshes)
			{
				Batch b;
				b.job.mesh = m;
				b.job.textures = _textureManager->GetTextures(go.components[Components::TEXTURES]);
				b.transforms = new DirectX::XMFLOAT4X4();
				*b.transforms = _transformManager->GetWorld(go.components[Components::TRANSFORM]);
				b.jobCount = 1;
				meshbatches.push_back(b);
			}
		}
	}
}

