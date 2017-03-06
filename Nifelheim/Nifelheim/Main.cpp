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
	

	int terrain = core->CreateGameObject();
	core->GetMeshManager()->LoadTerrain(terrain, "mountains257.data", 0.05f, 1.0f);
	core->GetTransformManager()->CreateTransform(terrain, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
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
		
	}

	for (int i = 0; i < 400; ++i)
	{
		int someobject = core->CreateGameObject();
		core->GetMeshManager()->LoadMesh(someobject, "cube.obj");
		core->GetTransformManager()->CreateTransform(someobject, i % 20, 0.0f, i / 20, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f);
		core->GetTextureManager()->GiveTexture(someobject, "ft_stone01_c.png", TextureTypes::TT_DIFFUSE);
		core->GetTextureManager()->GiveTexture(someobject, "ft_stone01_n.png", TextureTypes::TT_NORMAL);
	}

	int soundEmitter = core->CreateGameObject();
	core->GetAudioManager()->GiveAudio(soundEmitter, "callofmagic.wav", AUDIO_ENABLE_LOOPING);
	core->GetAudioManager()->SetVolume(soundEmitter, 30);
	core->GetAudioManager()->Play(soundEmitter);
	

	int cave = core->CreateGameObject();
	core->GetMeshManager()->LoadMesh(cave, "cave.obj");
	core->GetTextureManager()->GiveTexture(cave, "concrete_c.png", TextureTypes::TT_DIFFUSE);
	core->GetTransformManager()->CreateTransform(cave, -15, 6, -5, 8, 8, 8,0,100,0);



	int thingamajig = core->CreateGameObject();
	core->GetAudioManager()->GiveAudio(thingamajig, "siren.wav", AUDIO_ENABLE_LOOPING | AUDIO_ENABLE_MAX_RANGE | AUDIO_ENABLE_STEREO_PANNING);
	core->GetAudioManager()->SetVolume(thingamajig, 127);
	core->GetAudioManager()->SetRange(thingamajig, 400);
	core->GetMeshManager()->LoadMesh(thingamajig, "speakers.obj");
	core->GetTextureManager()->GiveTexture(thingamajig, "speaker.png", TextureTypes::TT_DIFFUSE);
	core->GetTransformManager()->CreateTransform(thingamajig, 40, 5, 0,0.01f,0.01f,0.01f);
	core->GetAudioManager()->Play(thingamajig);

	int dadada = core->CreateGameObject();
	core->GetAudioManager()->GiveAudio(dadada, "guitar.wav", AUDIO_ENABLE_LOOPING | AUDIO_ENABLE_MAX_RANGE | AUDIO_ENABLE_STEREO_PANNING);
	core->GetAudioManager()->SetVolume(dadada, 127);
	core->GetAudioManager()->SetRange(dadada, 400);
	core->GetMeshManager()->LoadMesh(dadada, "cube.obj");
	core->GetTextureManager()->GiveTexture(dadada, "volume.png", TextureTypes::TT_DIFFUSE);
	core->GetTransformManager()->CreateTransform(dadada, -15,6,-5);
	core->GetAudioManager()->Play(dadada);

	InputManager* i = core->GetInputManager();
	CameraManager* c = core->GetCameraManager();
	AudioManager* audio = core->GetAudioManager();
	Timer* t = core->GetTimer();
	while(true)
	{
		if (i->WasKeyPressed(KEY_G))
			audio->Play(dadada);
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

		if (i->WasKeyPressed(KEY_V))
			audio->SetFilter(dadada, EchoFilter);
		if (i->WasKeyPressed(KEY_B))
			audio->ClearFilters(dadada);

		if (i->WasKeyPressed(KEY_F))
			audio->SetFilter(thingamajig, EchoFilter);
		if (i->WasKeyPressed(KEY_G))
			audio->ClearFilters(thingamajig);

		if (i->WasKeyPressed(KEY_K))
		{
			std::stringstream ss;
			ss << deltatime;
			DebugLogger::AddMsg("Frametime: " + ss.str());
		}

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