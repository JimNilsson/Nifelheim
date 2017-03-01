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


	int thirdCube = core->CreateGameObject();
	core->GetMeshManager()->LoadMesh(thirdCube, "ringbox.obj");
	core->GetTransformManager()->CreateTransform(thirdCube, 0.0f, 7.0f, 4.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f);
	core->GetTextureManager()->GiveTexture(thirdCube, "concrete_c.png", TextureTypes::TT_DIFFUSE);
	core->GetTextureManager()->GiveTexture(thirdCube, "ft_stone01_n.png", TextureTypes::TT_NORMAL);

	int cube = core->CreateGameObject();
	core->GetMeshManager()->LoadMesh(cube, "cube.obj");
	core->GetTransformManager()->CreateTransform(cube, 6.0f, 7.0f, 4.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f);
	core->GetTextureManager()->GiveTexture(cube, "wall.png", TextureTypes::TT_DIFFUSE);
	core->GetTextureManager()->GiveTexture(cube, "wall_n.png", TextureTypes::TT_NORMAL);


	int anchor = core->CreateGameObject();
	core->GetTransformManager()->CreateTransform(anchor, 0.0f, 7.0f, 4.5f);

	int spinner = core->CreateGameObject();
	core->GetLightManager()->GivePointLight(spinner, 0.0f, 0.2f, 1.0f, 40.0f, 1.2f);
	core->GetTransformManager()->CreateTransform(spinner, 3.0f, 7.0f, 4.5f,0.1f, 0.1f, 0.1f);
	core->GetTransformManager()->BindChild(anchor, spinner);
	core->GetMeshManager()->LoadMesh(spinner, "ringbox.obj");
	core->GetTextureManager()->GiveTexture(spinner, "megumin.png", TextureTypes::TT_DIFFUSE);
	

	int uglyfuck = core->CreateGameObject();
	core->GetMeshManager()->LoadMesh(uglyfuck, "uglyfuck.obj");
	core->GetTransformManager()->CreateTransform(uglyfuck, 5.0f, 0.5f, 5.0f,0.25f,0.25f,0.25f);
	core->GetTextureManager()->GiveTexture(uglyfuck, "ft_stone01_c.png", TextureTypes::TT_DIFFUSE);
	core->GetTextureManager()->GiveTexture(uglyfuck, "default_n.png", TextureTypes::TT_NORMAL);

	int terrain = core->CreateGameObject();
	core->GetMeshManager()->LoadTerrain(terrain, "mountains257.data", 0.05f, 1.0f);
	core->GetTransformManager()->CreateTransform(terrain, 0.0f, 0.0f, 0.0f, 0.25f, 1.0f, 0.25f);
	core->GetTextureManager()->GiveTexture(terrain, "grass.png", TextureTypes::TT_DIFFUSE);
	core->GetTextureManager()->GiveTexture(terrain, "default_n.png", TextureTypes::TT_NORMAL);

	int dirLight = core->CreateGameObject();
	core->GetLightManager()->GiveDirectionalLight(dirLight, 0.8f, 0.5f, 0.1f, 2.0f, -4.0f, -6.0f, 2.0f);

	for (int i = 0; i < 80; ++i)
	{
		int light = core->CreateGameObject();
		
		core->GetLightManager()->GivePointLight(light, 1.0f, 1.0f, 1.0f, 20.0f, 1.0f);
		

		float xpos =static_cast<float>( rand() % 64 - 32);
		float ypos = 2.0f + 4.0f*((float)rand() / RAND_MAX);
		float zpos = static_cast<float>(rand() % 64 - 32);
		core->GetTransformManager()->CreateTransform(light, xpos, ypos, zpos);
		
	/*	int ssomeobject = core->CreateGameObject();
		core->GetMeshManager()->LoadMesh(ssomeobject, "cube.obj");
		core->GetTransformManager()->CreateTransform(ssomeobject, xpos, ypos, zpos, 0.1f, 0.1f, 0.1f, 0.0f, 0.0f, 0.0f);
		core->GetTextureManager()->GiveTexture(ssomeobject, "yukieat.png", TextureTypes::TT_DIFFUSE);
		core->GetTextureManager()->GiveTexture(ssomeobject, "yukieat.png", TextureTypes::TT_NORMAL);
*/
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

	int soundEmitter = core->CreateGameObject();
	core->GetAudioManager()->GiveAudio(soundEmitter, "voice.wav");
	core->GetAudioManager()->Play(soundEmitter);
	
	int dadada = core->CreateGameObject();
	core->GetAudioManager()->GiveAudio(dadada, "guitar.wav", AUDIO_ENABLE_LOOPING | AUDIO_ENABLE_MAX_RANGE | AUDIO_ENABLE_STEREO_PANNING);
	core->GetMeshManager()->LoadMesh(dadada, "cube.obj");
	core->GetTextureManager()->GiveTexture(dadada, "megumin.png", TextureTypes::TT_DIFFUSE);
	core->GetTransformManager()->CreateTransform(dadada, 0, 0, 0);
	core->GetAudioManager()->Play(dadada);
	InputManager* i = core->GetInputManager();
	CameraManager* c = core->GetCameraManager();
	AudioManager* audio = core->GetAudioManager();
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

		if (i->IsKeyDown(KEY_Y))
			core->GetTransformManager()->Rotate(cube, 0.02 * deltatime, 0.0f, 0.0f);
		if (i->IsKeyDown(KEY_U))
			core->GetTransformManager()->Rotate(cube, 0.0f, 0.02 * deltatime, 0.0f);
		if (i->IsKeyDown(KEY_I))
			core->GetTransformManager()->Rotate(cube, 0.0f, 0.0f, 0.02 * deltatime);

		if (i->IsKeyDown(KEY_NUMPAD_4))
			core->GetTransformManager()->Translate(dadada, deltatime*4.0f, 0.0f, 0.0f);
		if (i->IsKeyDown(KEY_NUMPAD_6))
			core->GetTransformManager()->Translate(dadada, -deltatime*4.0f, 0.0f, 0.0f);
		if (i->IsKeyDown(KEY_NUMPAD_8))
			core->GetTransformManager()->Translate(dadada, 0.0f, 0.0f, deltatime*4.0f);
		if (i->IsKeyDown(KEY_NUMPAD_2))
			core->GetTransformManager()->Translate(dadada, 0.0f, 0.0f, -deltatime*4.0f);
		if (i->IsKeyDown(KEY_NUMPAD_7))
			core->GetTransformManager()->Translate(dadada, 0.0f, deltatime*4.0f, 0.0f);
		if (i->IsKeyDown(KEY_NUMPAD_9))
			core->GetTransformManager()->Translate(dadada, 0.0f, -deltatime*4.0f, 0.0f);

		if (i->IsKeyDown(KEY_ESCAPE))
			break;
		
		//core->GetTransformManager()->Rotate(thirdCube, 0.0f, 0.00003f, 0.0f);
		if(i->IsKeyDown(KEY_B))
			core->GetTransformManager()->Rotate(anchor, 0.0f, 0.02f * deltatime, 0.0f);
		if (i->IsKeyDown(KEY_N))
			core->GetTransformManager()->Rotate(anchor, 0.0f, 0.0f, 0.02f * deltatime);
		if (i->IsKeyDown(KEY_M))
			core->GetTransformManager()->Rotate(anchor, 0.02f * deltatime, 0.0f, 0.0f);
		core->Update();
	}
	Core::ShutDown();
	DebugLogger::Dump();

	return 0;
}