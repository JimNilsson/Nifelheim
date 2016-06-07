#ifndef _CORE_H_
#define _CORE_H_
#include <SDL.h>
#include "Window.h"
#include "Direct3D11.h"
#include "MeshManager.h"
#include "CameraManager.h"

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
public:
	static void CreateInstance();
	static Core* GetInstance();
	static void ShutDown();
	void Init(uint32_t width, uint32_t height, bool fullscreen);

	static Window* GetWindow();
	static Direct3D11* GetDirect3D11();
	static MeshManager* GetMeshManager();
	static CameraManager* GetCameraManager();



	
};


#endif
