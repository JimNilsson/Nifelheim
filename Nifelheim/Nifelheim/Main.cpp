#include "Core.h"
#include "DebugLogger.h"
#include <SDL_events.h>

int main(int argc, char** argv)
{
	Core::CreateInstance();
	Core* core = Core::GetInstance();
	core->Init(800, 600, false);

	SDL_Event ev;
	
	ObjectID cube = core->CreateGameObject();
	core->GiveMesh(cube, "cube.obj");
	core->GiveTransform(cube, 0, 0, 2.0f, 0.0f, 0.0f, 0.0f, 2.0f, 2.0f, 2.0f);
	int p = 5;
	const int& k = p;
	p = 4;

	while(true)
	{
		SDL_PollEvent(&ev);
		if (ev.type == SDL_KEYDOWN)
			break;
		core->GetDirect3D11()->Draw();
		core->GetTransformManager()->Rotate(0, 0.0f, 0.00003f, 0.0f);
	}
	Core::ShutDown();
	DebugLogger::Dump();

	return 0;
}