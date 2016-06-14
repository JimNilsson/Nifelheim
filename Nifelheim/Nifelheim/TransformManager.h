#ifndef _TRANSFORM_MANAGER_H_
#define _TRANSFORM_MANAGER_H_

#include "Structs.h"
#include <vector>

class TransformManager
{
public:
	TransformManager();
	~TransformManager();
	const unsigned CreateTransform(const int gameObject,
		float posX, float posY, float posZ,
		float scaleX = 1.0f, float scaleY = 1.0f, float scaleZ = 1.0f,
		float rotX = 0.0f, float rotY = 0.0f, float rotZ = 0.0f);
	
	void Rotate(const int gameObject, float degX, float degY, float degZ);
	void SetRotation(const int gameObject, float degX, float degY, float degZ);

	void Scale(const int gameObject, float x, float y, float z);
	void SetScale(const int gameObject, float x, float y, float z);

	void Translate(const int gameObject, float x, float y, float z);
	void SetTranslation(const int gameObject, float x, float y, float z);

	void BindChild(const int parent, const int child, bool bindTranslation = true, bool bindRotation = true, bool bindScale = false);

	DirectX::XMFLOAT4X4 GetWorld(unsigned id) const;
	DirectX::XMFLOAT3 GetPosition(unsigned id) const;

	const TransformCache& GetTransformBuffer(unsigned id) const;

private:
	void _Rotate(unsigned id, float degX, float degY, float degZ);
	void _SetRotation(unsigned id, float degX, float degY, float degZ);

	void _Scale(unsigned id, float x, float y, float z);
	void _SetScale(unsigned id, float x, float y, float z);

	void _Translate(unsigned id, float x, float y, float z);
	void _SetTranslation(unsigned id, float x, float y, float z);

	void _BindChild(unsigned parent, unsigned child, bool bindTranslation = true, bool bindRotation = true, bool bindScale = false);

	void _UpdateCache(unsigned id);
	std::vector<Transform> _transforms;
	std::vector<TransformCache> _transformCache;
};


#endif

