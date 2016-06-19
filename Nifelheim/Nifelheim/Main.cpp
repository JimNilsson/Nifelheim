#include "Core.h"
#include "DebugLogger.h"
#include <SDL_events.h>
#include <sstream>
#include <string>


int main(int argc, char** argv)
{
	Core::CreateInstance();
	Core* core = Core::GetInstance();
	core->Init(800, 600, false);
	srand(1232323);
	//


	//
	int cube = core->CreateGameObject();
	core->GetMeshManager()->LoadMesh(cube, "cube.obj");
	core->GetTransformManager()->CreateTransform(cube, -5.0f, 0.0f, 4.5f, 3.0f, 3.0f, 3.0f, 0.0f, 0.0f, 0.0f);
	core->GetTextureManager()->GiveTexture(cube, "yukieat.png", TextureTypes::TT_DIFFUSE);

	int otherCube = core->CreateGameObject();
	core->GetMeshManager()->LoadMesh(otherCube, "cube.obj");
	core->GetTransformManager()->CreateTransform(otherCube, 5.0f, 0.0f, 4.5f, 3.0f, 3.0f, 3.0f, 0.0f, 0.0f, 0.0f);
	core->GetTextureManager()->GiveTexture(otherCube, "megumin.png", TextureTypes::TT_DIFFUSE);
	core->GetTransformManager()->BindChild(cube, otherCube);

	int thirdCube = core->CreateGameObject();
	core->GetMeshManager()->LoadMesh(thirdCube, "gudsnes.obj");
	core->GetTransformManager()->CreateTransform(thirdCube, 0.0f, 5.0f, 4.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f);
	core->GetTextureManager()->GiveTexture(thirdCube, "goodsnes.png", TextureTypes::TT_DIFFUSE);
	//core->GetTransformManager()->BindChild(otherCube, thirdCube);

	int uglyfuck = core->CreateGameObject();
	core->GetMeshManager()->LoadMesh(uglyfuck, "uglyfuck.obj");
	core->GetTransformManager()->CreateTransform(uglyfuck, 5.0f, 0.5f, 5.0f,0.25f,0.25f,0.25f);
	core->GetTextureManager()->GiveTexture(uglyfuck, "ft_stone01_c.png", TextureTypes::TT_DIFFUSE);
	core->GetTextureManager()->GiveTexture(uglyfuck, "default_n.png", TextureTypes::TT_NORMAL);

	int terrain = core->CreateGameObject();
	core->GetMeshManager()->LoadTerrain(terrain, "island257.data", 0.005f, 1.0f);
	core->GetTransformManager()->CreateTransform(terrain, 0.0f, 0.0f, 0.0f, 0.25f, 1.0f, 0.25f);
	core->GetTextureManager()->GiveTexture(terrain, "ft_stone01_c.png", TextureTypes::TT_DIFFUSE);
	core->GetTextureManager()->GiveTexture(terrain, "ft_stone01_n.png", TextureTypes::TT_NORMAL);

	for (int i = 0; i < 4; ++i)
	{
		int light = core->CreateGameObject();
		
		core->GetLightManager()->GivePointLight(light, 1.0f, 1.0f, 1.0f, 20.0f, 1.0f);
		

		float xpos = rand() % 20;
		float ypos = 1.0f + 2.0f*((float)rand() / RAND_MAX);
		float zpos = rand() % 20;
		core->GetTransformManager()->CreateTransform(light, xpos, ypos, zpos);
		
		int ssomeobject = core->CreateGameObject();
		core->GetMeshManager()->LoadMesh(ssomeobject, "cube.obj");
		core->GetTransformManager()->CreateTransform(ssomeobject, xpos, ypos, zpos, 0.1f, 0.1f, 0.1f, 0.0f, 0.0f, 0.0f);
		core->GetTextureManager()->GiveTexture(ssomeobject, "yukieat.png", TextureTypes::TT_DIFFUSE);
		core->GetTextureManager()->GiveTexture(ssomeobject, "yukieat.png", TextureTypes::TT_NORMAL);

	}
	int poslight = core->CreateGameObject();
	core->GetMeshManager()->LoadMesh(poslight, "cube.obj");
	core->GetTransformManager()->CreateTransform(poslight, 0.0f, 0.0f, 0.0f);
	core->GetTextureManager()->GiveTexture(poslight, "megumin.png", TextureTypes::TT_DIFFUSE);

	for (int i = 0; i < 400; ++i)
	{
		int someobject = core->CreateGameObject();
		core->GetMeshManager()->LoadMesh(someobject, "cube.obj");
		core->GetTransformManager()->CreateTransform(someobject, i % 20, 0.0f, i / 20, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f);
		core->GetTextureManager()->GiveTexture(someobject, "ft_stone01_c.png", TextureTypes::TT_DIFFUSE);
		core->GetTextureManager()->GiveTexture(someobject, "ft_stone01_n.png", TextureTypes::TT_NORMAL);

		//someobject = core->CreateGameObject();
		//core->GetMeshManager()->LoadMesh(someobject, "ringbox.obj");
		//core->GetTransformManager()->CreateTransform(someobject, rand() % 20, rand() % 20, rand() % 20, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f);
		//core->GetTextureManager()->GiveTexture(someobject, "megumin.png", TextureTypes::TT_DIFFUSE);
	}

	

	InputManager* i = core->GetInputManager();
	CameraManager* c = core->GetCameraManager();
	Timer* t = core->GetTimer();
	while(true)
	{
		float deltatime = t->GetDeltaTime();
		if (i->IsKeyDown(KEY_ARROW_LEFT))
			c->RotateActiveCamera(0.0f, deltatime * -0.05f, 0.0f);
		if (i->IsKeyDown(KEY_ARROW_RIGHT))
			c->RotateActiveCamera(0.0f, deltatime * 0.05f, 0.0f);
		if (i->IsKeyDown(KEY_ARROW_UP))
			c->RotatePitch(deltatime * -0.05f);
		if (i->IsKeyDown(KEY_ARROW_DOWN))
			c->RotatePitch(deltatime * 0.05f);
		if (i->IsKeyDown(KEY_W))
			c->MoveForward(deltatime * 4.0f);
		if (i->IsKeyDown(KEY_S))
			c->MoveForward(deltatime * -4.0f);
		if (i->IsKeyDown(KEY_A))
			c->MoveRight(deltatime * -4.0f);
		if (i->IsKeyDown(KEY_D))
			c->MoveRight(deltatime * 4.0f);
		if (i->IsKeyDown(KEY_LSHIFT))
			c->MoveUp(deltatime * 4.0f);
		if (i->IsKeyDown(KEY_LCTRL))
			c->MoveUp(deltatime*-4.0f);
		if (i->WasKeyPressed(KEY_K))
		{
			std::stringstream ss;
			ss << deltatime;
			DebugLogger::AddMsg("Frametime: " + ss.str());
		}


		if (i->IsKeyDown(KEY_ESCAPE))
			break;
		core->GetTransformManager()->Rotate(cube, 0.0000f, 0.00001f, 0.0000f);
		core->GetTransformManager()->Rotate(otherCube, 0.0f, 0.00002f, 0.0f);
		//core->GetTransformManager()->Rotate(thirdCube, 0.0f, 0.00003f, 0.0f);
		core->Update();
	}
	Core::ShutDown();
	DebugLogger::Dump();

	return 0;
}