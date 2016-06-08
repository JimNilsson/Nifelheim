#include "CameraManager.h"
#include "Core.h"
#include "DebugLogger.h"

using namespace DirectX;

CameraManager::CameraManager()
{
	_activeCamera = 0;
	const Core* core = Core::GetInstance();
	float width = (float)core->GetWindow()->GetWidth();
	float height = (float)core->GetWindow()->GetHeight();
	float aspectRatio = height / width;
	float fov = 85.0f;
	Camera defaultCam;
	defaultCam.aspectRatio = aspectRatio;
	defaultCam.fov = fov;
	defaultCam.forward = XMFLOAT3(0.0f, 0.0f, 1.0f);
	defaultCam.up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	defaultCam.nearPlane = 1.0f;
	defaultCam.farPlane = 100.0f;
	defaultCam.position = XMFLOAT3(0.0f, 0.0f, -2.0f);
	_cameras.push_back(defaultCam);

}

CameraManager::~CameraManager()
{
}

unsigned CameraManager::AddCamera(float posX, float posY, float posZ, float dirX, float dirY, float dirZ, float fov, float aspectRatio, float upX, float upY, float upZ, float nearPlane, float farPlane)
{
	Camera cam;
	cam.position = XMFLOAT3(posX, posY, posZ);
	cam.forward = XMFLOAT3(dirX, dirY, dirZ);
	cam.up = XMFLOAT3(upX, upY, upZ);
	cam.fov = fov;
	cam.aspectRatio = aspectRatio;
	_cameras.push_back(cam);

	if (_activeCamera == -1)
		_activeCamera = 0;

	return _cameras.size() - 1;

}

unsigned CameraManager::CycleActiveCamera()
{

	_activeCamera = (_activeCamera + 1) % _cameras.size();
	return _activeCamera;
}

void CameraManager::SetActiveCamera(unsigned id)
{
	if (id >= _cameras.size())
	{
		DebugLogger::AddMsg("Tried to set nonexistant camera.");
		return;
	}
	_activeCamera = id;
}

void CameraManager::FillPerFrameBuffer(PerFrameBuffer& pfb, int cameraID)
{
	if (cameraID == -1)
		cameraID = _activeCamera;

	XMVECTOR pos = XMLoadFloat3(&_cameras[cameraID].position);
	XMVECTOR dir = XMLoadFloat3(&_cameras[cameraID].forward);
	XMVECTOR up = XMLoadFloat3(&_cameras[cameraID].up);
	XMMATRIX view = XMMatrixLookToLH(pos, dir, up);
	XMMATRIX proj = XMMatrixPerspectiveFovLH(_cameras[cameraID].fov, _cameras[cameraID].aspectRatio, _cameras[cameraID].nearPlane, _cameras[cameraID].farPlane);

	XMStoreFloat4x4(&pfb.View, view);
	XMStoreFloat4x4(&pfb.Proj, proj);
	XMStoreFloat4x4(&pfb.InvView, XMMatrixInverse(nullptr, view));
	XMStoreFloat4x4(&pfb.ViewProj, view * proj);
	XMStoreFloat4x4(&pfb.InvViewProj, XMMatrixInverse(nullptr, view * proj));
	XMStoreFloat4(&pfb.CamPos, pos);
}

void CameraManager::RotateActiveCamera(float degX, float degY, float degZ)
{
	float radX = degX * 180.0f / XM_PI;
	float radY = degY * 180.0f / XM_PI;
	float radZ = degZ * 180.0f / XM_PI;

	XMMATRIX rot = XMMatrixRotationRollPitchYaw(radX, radY, radZ);
	XMVECTOR dir = XMLoadFloat3(&_cameras[_activeCamera].forward);
	XMVECTOR up = XMLoadFloat3(&_cameras[_activeCamera].up);
	XMStoreFloat3(&_cameras[_activeCamera].forward, XMVector2Transform(dir,rot));
	XMStoreFloat3(&_cameras[_activeCamera].up, XMVector2Transform(up, rot));

}

void CameraManager::TranslateActiveCamera(float offsetX, float offsetY, float offsetZ)
{
	_cameras[_activeCamera].position.x += offsetX;
	_cameras[_activeCamera].position.y += offsetY;
	_cameras[_activeCamera].position.z += offsetZ;
}

void CameraManager::SetCameraPosition(float posX, float posY, float posZ)
{
	_cameras[_activeCamera].position = XMFLOAT3(posX, posY, posZ);
}
