#include "InputManager.h"
#include <unordered_map>
#include <SDL_events.h>

InputManager::InputManager()
{
	_keyInfo[5] = KeyPress();
}

InputManager::~InputManager()
{
}

bool InputManager::WasKeyPressed(unsigned keyCode) const
{
	auto got = _keyInfo.find(keyCode);
	if (got == _keyInfo.end())
		return false;

	return got->second.wasPressed;
}

bool InputManager::IsKeyDown(unsigned keyCode) const
{
	auto got = _keyInfo.find(keyCode);
	if (got == _keyInfo.end())
		return false;

	return got->second.isDown;
}


void InputManager::Update()
{
	SDL_Event ev;
	SDL_PollEvent(&ev);
	switch (ev.type)
	{
	case SDL_KEYDOWN:
	{
		_keyInfo[ev.key.keysym.sym].wasPressed = !_keyInfo[ev.key.keysym.sym].isDown;
		_keyInfo[ev.key.keysym.sym].isDown = true;
		break;
	}
	case SDL_KEYUP:
	{
		_keyInfo[ev.key.keysym.sym].wasPressed = false;
		_keyInfo[ev.key.keysym.sym].isDown = false;
	}
	default:
		break;
	}
}



