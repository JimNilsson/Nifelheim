#include "GameObject.h"

GameObject::GameObject()
{
	id = GameObject::GenerateID();
	memset(components, -1, sizeof(int) * Components::COMPONENT_COUNT);

}

GameObject::GameObject(const GameObject & other)
{
	id = GameObject::GenerateID();
	for (int i = 0; i < Components::COMPONENT_COUNT; ++i)
	{
		components[i] = other.components[i];
	}
}

const GameObject& GameObject::operator=(const GameObject& rhs)
{
	//id = GameObject::GenerateID();
	for (int i = 0; i < Components::COMPONENT_COUNT; ++i)
	{
		components[i] = rhs.components[i];
	}
}
