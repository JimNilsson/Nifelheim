#include "Core.h"
#include "DebugLogger.h"
#include <SDL_events.h>

int main(int argc, char** argv)
{
	Core::CreateInstance();
	Core* core = Core::GetInstance();
	core->Init(800, 600, false);

	int m = core->GetMeshManager()->LoadMesh("cube.obj");
	SDL_Event ev;
	
	Core::CreateGameObject();
	Core::CreateGameObject();
	

	while(true)
	{
		SDL_PollEvent(&ev);
		if (ev.type == SDL_KEYDOWN)
			break;
		core->GetDirect3D11()->Draw();
	}
	Core::ShutDown();
	DebugLogger::Dump();

	return 0;
}