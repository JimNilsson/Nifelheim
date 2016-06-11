#include "Core.h"
#include "DebugLogger.h"
#include <SDL_events.h>

int main(int argc, char** argv)
{
	Core::CreateInstance();
	Core* core = Core::GetInstance();
	core->Init(800, 600, false);
	
	SDL_Event ev;
	//
	int cube = core->CreateGameObject();
	core->GetMeshManager()->LoadMesh(cube, "cube.obj");
	core->GetTransformManager()->CreateTransform(cube, -5.0f, 0.0f, 4.5f, 6.0f, 6.0f, 6.0f, 0.0f, 0.0f, 0.0f);
	core->GetTextureManager()->GiveTexture(cube, "yukieat.png", TextureTypes::TT_DIFFUSE);

	int otherCube = core->CreateGameObject();
	core->GetMeshManager()->LoadMesh(otherCube, "cube.obj");
	core->GetTransformManager()->CreateTransform(otherCube, 5.0f, 0.0f, 4.5f, 6.0f, 6.0f, 6.0f, 0.0f, 0.0f, 0.0f);
	core->GetTextureManager()->GiveTexture(otherCube, "yukieat.png", TextureTypes::TT_DIFFUSE);
	core->GetTransformManager()->BindChild(cube, otherCube);
	int p = 5;
	const int& k = p;
	p = 4;

	while(true)
	{
		InputManager* i = core->GetInputManager();
		CameraManager* c = core->GetCameraManager();

		if (i->IsKeyDown(KEY_ARROW_LEFT))
			c->RotateActiveCamera(0.0f, -0.00001f, 0.0f);
		if (i->IsKeyDown(KEY_ARROW_RIGHT))
			c->RotateActiveCamera(0.0f, 0.00001f, 0.0f);
		if (i->IsKeyDown(KEY_W))
			c->MoveForward(0.001f);
		if (i->IsKeyDown(KEY_S))
			c->MoveForward(-0.001f);
		if (i->IsKeyDown(KEY_A))
			c->MoveRight(-0.001f);
		if (i->IsKeyDown(KEY_D))
			c->MoveRight(0.001f);
			
		if (i->IsKeyDown(KEY_ESCAPE))
			break;
	//	core->GetTransformManager()->Rotate(cube, 0.0000f, 0.00001f, 0.0000f);
		core->Update();
	}
	Core::ShutDown();
	DebugLogger::Dump();

	return 0;
}