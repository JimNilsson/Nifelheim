#include "InputManager.h"
#include <unordered_map>
#include <SDL_events.h>

InputManager::InputManager()
{

}

InputManager::~InputManager()
{
}

bool InputManager::WasKeyPressed(unsigned keyCode) const
{
	auto got = _pressedKeys.find(keyCode);
	if (got == _pressedKeys.end())
		return false;

	return got->second;
}

bool InputManager::IsKeyDown(unsigned keyCode) const
{
	auto got = _downKeys.find(keyCode);
	if (got == _downKeys.end())
		return false;

	return got->second;
}


void InputManager::Update()
{
	_pressedKeys.clear();
	SDL_Event ev;
	SDL_PollEvent(&ev);
	switch (ev.type)
	{
	case SDL_KEYDOWN:
	{
		_pressedKeys[ev.key.keysym.sym] = true;
		_downKeys[ev.key.keysym.sym] = true;
		break;
	}
	case SDL_KEYUP:
	{
		_downKeys[ev.key.keysym.sym] = false;
		_pressedKeys[ev.key.keysym.sym] = false;
	}
	default:
		break;
	}
}



