#include "Core.h"
#include "DebugLogger.h"
#include <SDL_events.h>

int main(int argc, char** argv)
{
	Core::CreateInstance();
	Core::GetInstance()->Init(800, 600, false);

	Mesh m = Core::GetMeshManager()->LoadMesh("cube.obj");
	SDL_Event ev;
	
	while(true)
	{
		SDL_PollEvent(&ev);
		if (ev.type == SDL_KEYDOWN)
			break;
		Core::GetDirect3D11()->Draw();
	}
	Core::GetInstance()->ShutDown();
	DebugLogger::Dump();

	return 0;
}