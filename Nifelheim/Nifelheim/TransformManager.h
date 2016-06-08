#ifndef _TRANSFORM_MANAGER_H_
#define _TRANSFORM_MANAGER_H_

#include "Structs.h"
#include <vector>

class TransformManager
{
public:
	TransformManager();
	~TransformManager();
private:
	std::vector<Transform> _transforms;
	std::vector<TransformCache> _transformCache;
};


#endif

