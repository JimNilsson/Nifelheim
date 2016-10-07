#ifndef _GAME_OBJECT_H_
#define _GAME_OBJECT_H_

#include "Structs.h"

class GameObject
{
public:
	int id;
	int components[Components::COMPONENT_COUNT];
	GameObject();
	GameObject(const GameObject& other);
	const GameObject& operator=(const GameObject& rhs);

private:
	static int GenerateID()
	{
		static unsigned oid = 0;
		return oid++;
	}
};

#endif
