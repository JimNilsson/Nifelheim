#include "Core.h"
#include "DebugLogger.h"
#include <SDL_events.h>

int main(int argc, char** argv)
{
	Core::CreateInstance();
	Core* core = Core::GetInstance();
	core->Init(800, 600, false);

	SDL_Event ev;
	
	int cube = core->CreateGameObject();
	core->GetMeshManager()->LoadMesh(cube, "cube.obj");
	core->GetTransformManager()->CreateTransform(cube, 0.0f, 0.0f, 4.5f, 8.0f, 8.0f, 8.0f, 0.0f, 0.0f, 0.0f);
	core->GetTextureManager()->GiveTexture(cube, "yukieat.png", TextureTypes::TT_DIFFUSE);

	//int otherCube = core->CreateGameObject();
	//core->GetMeshManager()->LoadMesh(otherCube, "cube.obj");
	//core->GetTransformManager()->CreateTransform(otherCube, 4.0f, 0.0f, 1.0f, 0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f);
	//core->GetTransformManager()->BindChild(cube, otherCube);
	int p = 5;
	const int& k = p;
	p = 4;

	while(true)
	{
		SDL_PollEvent(&ev);
		if (ev.type == SDL_KEYDOWN)
			break;
		core->GetDirect3D11()->Draw();
		core->GetTransformManager()->Rotate(cube, 0.00001f, 0.0000f, 0.0000f);
	}
	Core::ShutDown();
	DebugLogger::Dump();

	return 0;
}