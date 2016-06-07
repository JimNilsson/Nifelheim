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
}

void Core::Init(uint32_t width, uint32_t height, bool fullscreen)
{
	_window = new Window();
	_d3d11 = new Direct3D11();
	_meshManager = new MeshManager();
	_cameraManager = new CameraManager();

}

Window * Core::GetWindow()
{
	return Core::GetInstance()->_window;
}

Direct3D11 * Core::GetDirect3D11()
{
	return Core::GetInstance()->_d3d11;
}

MeshManager * Core::GetMeshManager()
{
	return Core::GetInstance()->_meshManager;
}

CameraManager * Core::GetCameraManager()
{
	return Core::GetInstance()->_cameraManager;
}
