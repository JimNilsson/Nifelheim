#ifndef _INPUT_MANAGER_H_
#define _INPUT_MANAGER_H_

#include "Structs.h"
#include <unordered_map>

class InputManager
{
public:
	InputManager();
	~InputManager();
	void Update();
	bool WasKeyPressed(unsigned keyCode) const;
	bool IsKeyDown(unsigned keyCode) const;
private:
	std::unordered_map<unsigned, KeyPress> _keyInfo;
};



#endif

