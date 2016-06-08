#ifndef _CORE_H_
#define _CORE_H_
#include <SDL.h>
#include <vector>
#include "Window.h"
#include "Direct3D11.h"
#include "MeshManager.h"
#include "CameraManager.h"

typedef unsigned ObjectID;

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

	std::vector<GameObject> _gameObjects;
public:
	static void CreateInstance();
	static Core* GetInstance();
	static void ShutDown();
	void Init(uint32_t width, uint32_t height, bool fullscreen);

	Window* GetWindow() const;
	Direct3D11* GetDirect3D11() const;
	MeshManager* GetMeshManager() const;
	CameraManager* GetCameraManager() const;

	static ObjectID CreateGameObject();




	
};


#endif
