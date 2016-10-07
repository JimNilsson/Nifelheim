#ifndef _CORE_H_
#define _CORE_H_
#include <SDL.h>
#include <vector>
#include "Window.h"
#include "Direct3D11.h"
#include "MeshManager.h"
#include "CameraManager.h"
#include "TransformManager.h"
#include "TextureManager.h"
#include "InputManager.h"
#include "LightManager.h"
#include "Timer.h"

typedef int ObjectID;

//Singleton
class Core
{
private:
	Core();
	~Core();
	static Core* _instance;
	Window* _window;
	Direct3D11* _d3d11;
	MeshManager* _meshManager;
	CameraManager* _cameraManager;
	TransformManager* _transformManager;
	TextureManager* _textureManager;
	InputManager* _inputManager;
	LightManager* _lightManager;

	Timer* _timer;
	std::vector<GameObject> _gameObjects;

	unsigned FindObjectIndex(ObjectID id);
public:
	static void CreateInstance();
	static Core* GetInstance();
	static void ShutDown();
	void Init(uint32_t width, uint32_t height, bool fullscreen);
	void Update();

	Window* GetWindow() const;
	Direct3D11* GetDirect3D11() const;
	MeshManager* GetMeshManager() const;
	CameraManager* GetCameraManager() const;
	TransformManager* GetTransformManager() const;
	TextureManager* GetTextureManager() const;
	InputManager* GetInputManager() const;
	LightManager* GetLightManager() const;
	Timer* GetTimer() const;

	const int CreateGameObject();
	const GameObject& GetGameObject(int id) const;

	const std::vector<GameObject>& GetGameObjects() const;

	void GetRenderBatches(std::vector<Batch>& meshbatches) const;
};


#endif
