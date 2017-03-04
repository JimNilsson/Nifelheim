#include "AudioManager.h"
#include "Core.h"
#include <SDL.h>
#include <SDL_audio.h>
#include <SDL_mixer.h>

#undef max
#undef min
#include <algorithm>


AudioManager::AudioManager()
{
	_audioHandles.reserve(100);
	if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0)
		throw std::runtime_error("COuld not initialize sdl audio");
	//if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) == -1)
	//	throw std::runtime_error("Could not initilaize SDL_mixer");

	error = Pa_Initialize();
	if (error != paNoError)
	{
		throw std::runtime_error("PortAudio broken");
	}

	//error = Pa_OpenDefaultStream(&paStream, 0, 2, paInt16, 44100.0, AUDIO_CHUNK_SIZE, paCallback, nullptr);
	//if (error != paNoError)
	//	throw std::runtime_error("Portaudio broken 2");

	
}

AudioManager::~AudioManager()
{
	for (auto& i : _paStreams)
		Pa_StopStream(i);
	Pa_Sleep(1000);
	Pa_Terminate();
	//for (auto& i : _chunksToDelete)
	//	Mix_FreeChunk(i);
	//Mix_CloseAudio();
	for (auto& i : _audioData)
		delete[] i.second.data;
}

void AudioManager::GiveAudio(int gameObject, const std::string & filename, int32_t flags)
{
	AudioData* audioData = nullptr;
	auto exists = _audioData.find(filename);
	if (exists == _audioData.end())
	{
		SNDFILE* sf = nullptr;
		SF_INFO info = {};
		sf = sf_open(filename.c_str(), SFM_READ, &info);
		if (sf == NULL)
			throw std::runtime_error(std::string("Could not open file: ").append(filename));
		if (!(info.format & SF_FORMAT_WAV))
		{
			sf_close(sf);
			throw std::runtime_error("Sorry we only support wav because just doing an assignment");
		}
		int8_t* rawData = new int8_t[info.frames * info.channels * sizeof(int)];
		sf_read_short(sf, (short*)rawData, info.frames * info.channels);
		sf_close(sf);

		AudioData ad;
		ad.channels = info.channels;
		ad.data = (int16_t*)rawData;
		ad.sampleCount = info.frames * info.channels;
		ad.sampleRate = info.samplerate;
		_audioData[filename] = ad;
		audioData = &_audioData[filename];
	}
	else
	{
		audioData = &(exists->second);
	}

	AudioHandle handle;
	handle.data = audioData;
	handle.offset = 0;
	handle.flags = flags;
	handle.volume = 128;
	handle.gameObject = gameObject;
	int index = (int)_audioHandles.size();
	_audioHandles.push_back(handle);

	_objectToStream[gameObject];
	Pa_OpenDefaultStream(&_objectToStream[gameObject], 0, 2, paInt16, 88200.0, (unsigned long)FRAMES_PER_BUFFER, paCallback, (void*)&_audioHandles[index]);
	
	const GameObject& go = Core::GetInstance()->GetGameObject(gameObject);
	const_cast<GameObject&>(go).components[Components::AUDIO] = index;
	_audioToGameObject[index] = go.id;
}

void AudioManager::Play(int gameObject)
{
	const GameObject& go = Core::GetInstance()->GetGameObject(gameObject);
	_audioHandles[go.components[Components::AUDIO]].offset = 0;
	//_currentlyPlaying.insert(go.components[Components::AUDIO]);
	Pa_StopStream(_objectToStream[gameObject]);
	Pa_StartStream(_objectToStream[gameObject]);
}

void AudioManager::Stop(int gameObject)
{
	const GameObject& go = Core::GetInstance()->GetGameObject(gameObject);
	_audioHandles[go.components[Components::AUDIO]].offset = 0;
	//_currentlyPlaying.erase(go.components[Components::AUDIO]);
	Pa_StopStream(_objectToStream[gameObject]);
}

void AudioManager::Pause(int gameObject)
{
	const GameObject& go = Core::GetInstance()->GetGameObject(gameObject);
//	auto g = _currentlyPlaying.find(go.components[Components::AUDIO]);
//	_currentlyPlaying.erase(go.components[Components::AUDIO]);
}

void AudioManager::SetFlags(int gameObject, AudioSourceFlags flags)
{
	const GameObject& go = Core::GetInstance()->GetGameObject(gameObject);
	_audioHandles[go.components[Components::AUDIO]].flags = flags;
}

void AudioManager::SetVolume(int gameObject, uint8_t volume)
{
	const GameObject& go = Core::GetInstance()->GetGameObject(gameObject);
	_audioHandles[go.components[Components::AUDIO]].volume = volume;
}

void AudioManager::SetRange(int gameObject, float range)
{
	const GameObject& go = Core::GetInstance()->GetGameObject(gameObject);
	_audioHandles[go.components[Components::AUDIO]].range = range;
}

void AudioManager::Update(float dt)
{
	////Play only a small chunk at a time, while one chunk is playing, load and process the next chunk
	//if (_load)
	//{
	//	//Sound volume and panning dependent on active camera
	//	CameraManager* cam = Core::GetInstance()->GetCameraManager();
	//	DirectX::XMFLOAT3 pos = cam->GetPosition();
	//	DirectX::XMFLOAT3 forward = cam->GetForward();
	//	DirectX::XMFLOAT3 up = cam->GetUp();

	//	DirectX::XMVECTOR playerPos = DirectX::XMLoadFloat3(&pos);

	//	//Zero out the buffer since we will be adding to it rather than replacing it
	//	memset(_audioLoadBuffer, 0, AUDIO_CHUNK_SIZE * sizeof(int16_t));

	//	//Set of sounds that are still active after playing a small chunk.
	//	std::set<uint32_t> swapWithCurrent;

	//	for (auto& h : _currentlyPlaying)
	//	{
	//		float scalingFactor = 1.0f;
	//		float leftScale = 1.0f;
	//		float rightScale = 1.0f;
	//		//Check if the sound source has a position
	//		int gameObjectID = _audioToGameObject[h];
	//		const GameObject& go = Core::GetInstance()->GetGameObject(gameObjectID);
	//		if (_audioHandles[h].flags & AUDIO_ENABLE_MAX_RANGE && go.components[Components::TRANSFORM] >= 0)
	//		{
	//			TransformManager* tran = Core::GetInstance()->GetTransformManager();
	//			DirectX::XMFLOAT3 soundOrigin = tran->GetPosition(go.components[Components::TRANSFORM]);
	//			DirectX::XMVECTOR soundPos = DirectX::XMLoadFloat3(&soundOrigin);

	//			DirectX::XMVECTOR soundToPlayer = DirectX::XMVectorSubtract(playerPos, soundPos);
	//			float distance = DirectX::XMVectorGetX(DirectX::XMVector3Length(soundToPlayer));
	//			scalingFactor = _audioHandles[h].range / (_audioHandles[h].range + 4.0f * DirectX::XM_PI * distance * distance);

	//			DirectX::XMVECTOR xzPlane = DirectX::XMVectorSet(0, 1, 0, 0);
	//			DirectX::XMVECTOR playerForward = DirectX::XMLoadFloat3(&forward);
	//			DirectX::XMVECTOR playerToSound = DirectX::XMVectorScale(soundToPlayer, -1.0f);
	//			//Project both into xz-plane to calculate angle between player direction and direction to sound

	//			DirectX::XMVECTOR alongNormal = DirectX::XMVectorScale(xzPlane, DirectX::XMVectorGetX(DirectX::XMVector3Dot(xzPlane, playerForward)));
	//			playerForward = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(playerForward, alongNormal));

	//			alongNormal = DirectX::XMVectorScale(xzPlane, DirectX::XMVectorGetX(DirectX::XMVector3Dot(xzPlane, playerToSound)));
	//			playerToSound = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(playerToSound, alongNormal));

	//			float cosOfAngle = DirectX::XMVectorGetX(DirectX::XMVector3Dot(playerForward, playerToSound));

	//			//If the cross product points "up", the sound is to the right
	//			DirectX::XMVECTOR leftOrRight = DirectX::XMVector3Cross(playerForward, playerToSound);
	//			if (DirectX::XMVectorGetY(leftOrRight) > 0)
	//			{
	//				rightScale = cosOfAngle;
	//				leftScale = sqrt(1 - cosOfAngle * cosOfAngle);
	//			}
	//			else
	//			{
	//				leftScale = cosOfAngle;
	//				rightScale = sqrt(1 - cosOfAngle * cosOfAngle);
	//			}

	//		}


	//		int16_t* curSound = (int16_t*)((uint8_t*)_audioHandles[h].data->data + _audioHandles[h].offset);
	//		
	//		int upper = (int)std::min((long long)AUDIO_CHUNK_SIZE, _audioHandles[h].data->sampleCount - _audioHandles[h].offset);
	//		for (int i = 0; i < upper; i++)
	//		{
	//			_audioLoadBuffer[i] += (_audioHandles[h].volume / 128.0f) * scalingFactor * curSound[i];
	//		}
	//		if (_audioHandles[h].flags & AUDIO_ENABLE_LOOPING)
	//		{
	//			curSound = (int16_t*)_audioHandles[h].data->data;
	//			for (int i = upper, j = 0; i < AUDIO_CHUNK_SIZE; i++, j++)
	//			{
	//				_audioLoadBuffer[i] += (_audioHandles[h].volume / 128.0f) * scalingFactor * curSound[j];
	//			}
	//		}
	//		_audioHandles[h].offset = (_audioHandles[h].offset + AUDIO_CHUNK_SIZE*sizeof(int16_t)) % _audioHandles[h].data->sampleCount;

	//		if (_audioHandles[h].offset >= AUDIO_CHUNK_SIZE * sizeof(int16_t) || _audioHandles[h].flags & AUDIO_ENABLE_LOOPING)
	//			swapWithCurrent.insert(h);
	//	}
	//	std::swap(_currentlyPlaying, swapWithCurrent);
	//	

	//	_load = false;
	//}

	//if (Mix_Playing(-1) == 0)
	//{
	//	_load = true;
	//	std::swap(_audioPlayBuffer, _audioLoadBuffer);
	//	Mix_Chunk chunk;
	//	chunk.abuf = (Uint8*)_audioPlayBuffer;

	//	chunk.alen = AUDIO_CHUNK_SIZE * sizeof(int16_t);
	//	chunk.allocated = 0;
	//	chunk.volume = 128;
	//	
	//	Mix_PlayChannel(-1, &chunk, 0);		
	//}


}

int AudioManager::paCallback(const void * input, void * output, unsigned long frameCount, const PaStreamCallbackTimeInfo * timeInfo, PaStreamCallbackFlags statusFlags, void * userData)
{
	AudioHandle* handle = (AudioHandle*)userData;

	//Sound volume and panning dependent on active camera
	CameraManager* cam = Core::GetInstance()->GetCameraManager();
	DirectX::XMFLOAT3 pos = cam->GetPosition();
	DirectX::XMFLOAT3 forward = cam->GetForward();
	DirectX::XMFLOAT3 up = cam->GetUp();

	DirectX::XMVECTOR playerPos = DirectX::XMLoadFloat3(&pos);

	float scalingFactor = 1.0f;
	float leftScale = 1.0f;
	float rightScale = 1.0f;
	//Check if the sound source has a position
	int gameObjectID = handle->gameObject;
	const GameObject& go = Core::GetInstance()->GetGameObject(gameObjectID);
	if (go.components[Components::TRANSFORM] >= 0)
	{
		TransformManager* tran = Core::GetInstance()->GetTransformManager();
		DirectX::XMFLOAT3 soundOrigin = tran->GetPosition(go.components[Components::TRANSFORM]);
		DirectX::XMVECTOR soundPos = DirectX::XMLoadFloat3(&soundOrigin);

		DirectX::XMVECTOR soundToPlayer = DirectX::XMVectorSubtract(playerPos, soundPos);
		if (handle->flags & AUDIO_ENABLE_MAX_RANGE)
		{
			float distance = DirectX::XMVectorGetX(DirectX::XMVector3Length(soundToPlayer));
			scalingFactor = handle->range / (handle->range + 4.0f * DirectX::XM_PI * distance * distance);
		}

		if (handle->flags & AUDIO_ENABLE_STEREO_PANNING)
		{
			DirectX::XMVECTOR xzPlane = DirectX::XMVectorSet(0, 1, 0, 0);
			DirectX::XMVECTOR playerForward = DirectX::XMLoadFloat3(&forward);
			DirectX::XMVECTOR playerToSound = DirectX::XMVectorScale(soundToPlayer, -1.0f);
			//Project both into xz-plane to calculate angle between player direction and direction to sound

			DirectX::XMVECTOR alongNormal = DirectX::XMVectorScale(xzPlane, DirectX::XMVectorGetX(DirectX::XMVector3Dot(xzPlane, playerForward)));
			playerForward = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(playerForward, alongNormal));

			alongNormal = DirectX::XMVectorScale(xzPlane, DirectX::XMVectorGetX(DirectX::XMVector3Dot(xzPlane, playerToSound)));
			playerToSound = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(playerToSound, alongNormal));

			float cosOfAngle = DirectX::XMVectorGetX(DirectX::XMVector3Dot(playerForward, playerToSound));
			float angle = std::acos(cosOfAngle);
			//If the cross product points "up", the sound is to the right
			DirectX::XMVECTOR leftOrRight = DirectX::XMVector3Cross(playerForward, playerToSound);
			rightScale = (sqrt(2) / 2.0f)*(cosOfAngle - std::sin(angle));
			leftScale = (sqrt(2) / 2.0f)*(cosOfAngle + std::sin(angle));
			if (DirectX::XMVectorGetY(leftOrRight) < 0)
			{
				std::swap(rightScale, leftScale);
			}
		}
		
	}

	float distanceCorrectedVolume = (handle->volume / 128.0f) * scalingFactor;
	int16_t* out = (int16_t*)output;
	int upper = (int)std::min((long long)frameCount, handle->data->sampleCount*2 - handle->offset);
	int16_t* curSound = (int16_t*)((uint8_t*)handle->data->data + handle->offset);
	for (int i = 0; i < upper; i++)
	{		
		*out++ = distanceCorrectedVolume * rightScale * curSound[i];
		*out++ = distanceCorrectedVolume * leftScale * curSound[i];
	}
	if (handle->flags & AUDIO_ENABLE_LOOPING)
	{
		curSound = (int16_t*)handle->data->data;
		for (int i = upper, j = 0; i < frameCount; i++, j++)
		{
			*out++ = distanceCorrectedVolume * rightScale * curSound[j];
			*out++ = distanceCorrectedVolume * leftScale * curSound[j];
		}
	}
	uint32_t prev = handle->offset;
	handle->offset = (handle->offset + frameCount*sizeof(int16_t)) % (handle->data->sampleCount*2);
	/*if (handle->offset >= handle->data->sampleCount*2)
		handle->offset = 0;*/

	if (prev >= handle->offset && !(handle->flags & AUDIO_ENABLE_LOOPING))
		return 1;

	return 0;
}

