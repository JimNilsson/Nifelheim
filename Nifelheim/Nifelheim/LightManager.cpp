#include "LightManager.h"
#include "Core.h"

using namespace DirectX;

LightManager::LightManager()
{
}

LightManager::~LightManager()
{
}

void LightManager::GivePointLight(unsigned gameObject, float colR, float colG, float colB, float range, float intensity)
{
	const Core* c = Core::GetInstance();
	const GameObject& go = c->GetGameObject(gameObject);

	PointLight p;
	p.color = XMFLOAT3(colR, colG, colB);
	p.intensity = intensity;
	p.range = range;

	LightComponent lc;

	//If the game object has a transform component (which it should) get the position
	//The reason for storing the position in two places (in the pointlight as well as the transform component)
	//is to get less overhead when the renderer fetches the pointlights
	if (go.components[Components::TRANSFORM] >= 0)
		p.position = c->GetTransformManager()->GetPosition(go.components[Components::TRANSFORM]);

	if (go.components[Components::LIGHTSOURCE] >= 0)
	{
		if (_lights[go.components[Components::LIGHTSOURCE]].pointLight >= 0)
		{
			_pointLights[_lights[go.components[Components::LIGHTSOURCE]].pointLight] = p;
		}
		else
		{
			_pointLights.push_back(p);
			_lights[go.components[Components::LIGHTSOURCE]].pointLight = _pointLights.size() - 1;
		}
	}
	else
	{
		_pointLights.push_back(p);
		lc.pointLight = _pointLights.size() - 1;
		_lights.push_back(lc);
		const_cast<GameObject&>(go).components[Components::LIGHTSOURCE] = _lights.size() - 1;
	}

	
	
}

void LightManager::GiveDirectionalLight(unsigned gameObject, float colR, float colG, float colB, float dirX, float dirY, float dirZ, float intensity)
{
	const Core* c = Core::GetInstance();
	const GameObject& go = c->GetGameObject(gameObject);
	DirectionalLight d;
	XMVECTOR dir = XMVector3Normalize(XMVectorSet(dirX, dirY, dirZ, 0.0f));
	XMVECTOR col = XMVector3Normalize(XMVectorSet(colR, colG, colB, 0.0f));
	XMStoreFloat3(&d.color, col);
	XMStoreFloat3(&d.direction, dir);
	d.intensity = intensity;

	if (go.components[Components::LIGHTSOURCE] >= 0)
	{
		if (_lights[go.components[Components::LIGHTSOURCE]].dirLight >= 0)
		{
			_directionalLights[_lights[go.components[Components::LIGHTSOURCE]].dirLight] = d;
		}
		else
		{
			_directionalLights.push_back(d);
			_lights[go.components[Components::LIGHTSOURCE]].dirLight = _directionalLights.size() - 1;
		}
	}
	else
	{
		LightComponent lc;
		_directionalLights.push_back(d);
		lc.dirLight = _directionalLights.size() - 1;
		_lights.push_back(lc);
		const_cast<GameObject&>(go).components[Components::LIGHTSOURCE] = _lights.size() - 1;

	}
}

void LightManager::UpdatePosition(int index, float x, float y, float z)
{
	if (index >= 0)
		_pointLights[_lights[index].pointLight].position = XMFLOAT3(x, y, z);
}

const std::vector<PointLight>& LightManager::GetPointLights()
{
	const Core* core = Core::GetInstance();
	const std::vector<GameObject>& gameobjects = core->GetGameObjects();
	for (const auto& go : gameobjects)
	{
		if (go.components[Components::TRANSFORM] >= 0 && go.components[Components::LIGHTSOURCE] >= 0)
		{
			if (_lights[go.components[Components::LIGHTSOURCE]].pointLight >= 0)
			{
				_pointLights[_lights[go.components[Components::LIGHTSOURCE]].pointLight].position = core->GetTransformManager()->GetPosition(go.components[Components::TRANSFORM]);
			}
		}
	}
	return _pointLights;
}

const std::vector<DirectionalLight>& LightManager::GetDirLights() const
{
	return _directionalLights;
}

const LightComponent & LightManager::GetLightComponent(unsigned index) const
{
	if (index < _lights.size())
		return _lights[index];
	else
		throw std::exception("Index out of bounds in lightmanager");

}

const PointLight & LightManager::GetPointLight(unsigned index) const
{
	if (index < _pointLights.size())
		return _pointLights[index];
	else
		throw std::exception("Index out of bounds in lightmanager");
}
