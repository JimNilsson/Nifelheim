#ifndef _TRANSFORM_MANAGER_H_
#define _TRANSFORM_MANAGER_H_

#include "Structs.h"
#include <vector>

class TransformManager
{
public:
	TransformManager();
	~TransformManager();
	const unsigned CreateTransform(float posX, float posY, float posZ,
		float scaleX, float scaleY, float scaleZ,
		float rotX, float rotY, float rotZ);
	
	void Rotate(unsigned id, float degX, float degY, float degZ);
	void SetRotation(unsigned id, float degX, float degY, float degZ);

	void Scale(unsigned id, float x, float y, float z);
	void SetScale(unsigned id, float x, float y, float z);

	void Translate(unsigned id, float x, float y, float z);
	void SetTranslation(unsigned id, float x, float y, float z);

	void BindChild(unsigned parent, unsigned child, bool bindTranslation = true, bool bindRotation = true, bool bindScale = false);

	const TransformCache& GetTransformBuffer(unsigned id) const;

private:
	void _UpdateCache(unsigned id);
	std::vector<Transform> _transforms;
	std::vector<TransformCache> _transformCache;
};


#endif

