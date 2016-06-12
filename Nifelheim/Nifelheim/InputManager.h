#ifndef _INPUT_MANAGER_H_
#define _INPUT_MANAGER_H_

#include "Structs.h"
#include <unordered_map>
#include <map>
class InputManager
{
public:
	InputManager();
	~InputManager();
	void Update();
	bool WasKeyPressed(unsigned keyCode) const;
	bool IsKeyDown(unsigned keyCode) const;
private:
	std::map<unsigned, bool> _pressedKeys;
	std::map<unsigned, bool> _downKeys;
};



#endif

