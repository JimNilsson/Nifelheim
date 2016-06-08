#ifndef _CORE_H_
#define _CORE_H_
#include <SDL.h>
#include <vector>
#include "Window.h"
#include "Direct3D11.h"
#include "MeshManager.h"
#include "CameraManager.h"
#include "TransformManager.h"

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

	std::vector<GameObject> _gameObjects;

	unsigned FindObjectIndex(ObjectID id);
public:
	static void CreateInstance();
	static Core* GetInstance();
	static void ShutDown();
	void Init(uint32_t width, uint32_t height, bool fullscreen);

	Window* GetWindow() const;
	Direct3D11* GetDirect3D11() const;
	MeshManager* GetMeshManager() const;
	CameraManager* GetCameraManager() const;
	TransformManager* GetTransformManager() const;

	const ObjectID& CreateGameObject();
	void GiveMesh(ObjectID gameObject, const std::string& filename);
	void GiveTransform(ObjectID gameObject,
		float posX, float posY, float posZ,
		float rotX, float rotY, float rotZ,
		float scaleX, float scaleY, float scaleZ);




	
};


#endif
