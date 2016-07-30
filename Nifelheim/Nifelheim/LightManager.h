#ifndef _LIGHT_MANAGER_H_
#define _LIGHT_MANAGER_H_

#include "Structs.h"
#include <vector>
#define MAX_POINTLIGHTS 512U
#define MAX_DIRLIGHTS 8U
class LightManager
{
public:
	LightManager();
	~LightManager();

	//Position is given and updated by transform-system
	void GivePointLight(unsigned gameObject, float colR, float colG, float colB, float range, float intensity);
	void GiveDirectionalLight(unsigned gameObject, float colR, float colG, float colB, float dirX, float dirY, float dirZ, float intensity);
	void UpdatePosition(int index, float x, float y, float z);
	const std::vector<PointLight>& GetPointLights();
	const std::vector<DirectionalLight>& GetDirLights() const;
	const LightComponent& GetLightComponent(unsigned index) const;
	const PointLight& GetPointLight(unsigned index) const;
	const DirectionalLight& GetDirectionalLight(unsigned index) const;

private:
	std::vector<LightComponent> _lights;
	std::vector<PointLight> _pointLights;
	std::vector<DirectionalLight> _directionalLights;
};

#endif

