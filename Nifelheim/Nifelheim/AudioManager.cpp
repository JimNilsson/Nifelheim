#include "AudioManager.h"
#include "Core.h"
#include <SDL.h>
#include <SDL_audio.h>
#include <SDL_mixer.h>

#define MIX_BUFFER_SIZE 44100

#undef max
#undef min
#include <algorithm>


AudioManager::AudioManager()
{
	_audioHandles.reserve(100);
	if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0)
		throw std::runtime_error("COuld not initialize sdl audio");

	error = Pa_Initialize();
	if (error != paNoError)
	{
		throw std::runtime_error("PortAudio broken");
	}	
}

AudioManager::~AudioManager()
{

	for (auto& i : _paStreams)
	{
		Pa_StopStream(i);
		Pa_CloseStream(i);
	}
	Pa_Sleep(1000);
	Pa_Terminate();
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
	Pa_OpenDefaultStream(&_objectToStream[gameObject], 0, 2, paInt16, audioData->sampleRate * audioData->channels, (unsigned long)FRAMES_PER_BUFFER, paCallback, (void*)&_audioHandles[index]);
	
	const GameObject& go = Core::GetInstance()->GetGameObject(gameObject);
	const_cast<GameObject&>(go).components[Components::AUDIO] = index;
	_audioToGameObject[index] = go.id;
}

void AudioManager::Play(int gameObject)
{
	const GameObject& go = Core::GetInstance()->GetGameObject(gameObject);
	_audioHandles[go.components[Components::AUDIO]].offset = 0;
	Pa_StopStream(_objectToStream[gameObject]);
	Pa_StartStream(_objectToStream[gameObject]);
}

void AudioManager::Stop(int gameObject)
{
	const GameObject& go = Core::GetInstance()->GetGameObject(gameObject);
	_audioHandles[go.components[Components::AUDIO]].offset = 0;
	Pa_StopStream(_objectToStream[gameObject]);
}

void AudioManager::Pause(int gameObject)
{
	Pa_StopStream(_objectToStream[gameObject]);

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


void AudioManager::SetFilter(int gameObject, AudioFilter * filter)
{
	const GameObject& go = Core::GetInstance()->GetGameObject(gameObject);
	_audioHandles[go.components[Components::AUDIO]].audioFilters.push_back(filter);
}

void AudioManager::ClearFilters(int gameObject)
{
	const GameObject& go = Core::GetInstance()->GetGameObject(gameObject);
	_audioHandles[go.components[Components::AUDIO]].shouldClear = true;
	
}

int AudioManager::paCallback(const void * input, void * output, unsigned long frameCount, const PaStreamCallbackTimeInfo * timeInfo, PaStreamCallbackFlags statusFlags, void * userData)
{
	AudioHandle* handle = (AudioHandle*)userData;
	int16_t outBuffer[MIX_BUFFER_SIZE];//Used for filters
	int16_t inBuffer[MIX_BUFFER_SIZE];//Used for filters



	float scalingFactor = 1.0f; //Will be changed by the volume setting
	float leftScale = 1.0f; //Depends on if there is stereo panning
	float rightScale = 1.0f; //Likewise

	//A precondition for having a range falloff and stereo panning is that
	//the sound has an origin, for that the gameobject emitting the sound
	//needs to have a transform component
	int gameObjectID = handle->gameObject;
	const GameObject& go = Core::GetInstance()->GetGameObject(gameObjectID);
	if (go.components[Components::TRANSFORM] >= 0)
	{
		//Sound volume and panning dependent on active camera
		CameraManager* cam = Core::GetInstance()->GetCameraManager();
		DirectX::XMFLOAT3 pos = cam->GetPosition();
		DirectX::XMFLOAT3 forward = cam->GetForward();
		DirectX::XMFLOAT3 up = cam->GetUp();

		DirectX::XMVECTOR playerPos = DirectX::XMLoadFloat3(&pos);

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
			
			rightScale = (sqrt(2) / 2.0f)*(cosOfAngle - std::sin(angle));
			leftScale = (sqrt(2) / 2.0f)*(cosOfAngle + std::sin(angle));

			//If the cross product points "down", the sound is to the left. (Unless the angle was greater than 90 degrees)
			DirectX::XMVECTOR leftOrRight = DirectX::XMVector3Cross(playerForward, playerToSound);
			if (DirectX::XMVectorGetY(leftOrRight) < 0 && cosOfAngle > 0)
			{
				std::swap(rightScale, leftScale);
			}
		}
		
	}

	float distanceCorrectedVolume = (handle->volume / 128.0f) * scalingFactor;
	int16_t* out = (int16_t*)output;
	int upper = (int)std::min((long long)frameCount, handle->data->sampleCount - handle->offset);
	int16_t* curSound = (int16_t*)(handle->data->data + handle->offset);

	uint32_t back = std::min((uint32_t)(MIX_BUFFER_SIZE / 2), handle->offset);
	uint32_t forwards = std::min((uint32_t)((MIX_BUFFER_SIZE / 2) - frameCount), (uint32_t)handle->data->sampleCount - handle->offset);
	int16_t* start = (int16_t*)(curSound - back);
	uint32_t byteCount = (frameCount + back + forwards) * sizeof(int16_t);
	memset(inBuffer, 0, sizeof(int16_t) * MIX_BUFFER_SIZE);
	memcpy(&inBuffer[(MIX_BUFFER_SIZE / 2) - back], start, byteCount);
	if (handle->shouldClear)
	{
		handle->audioFilters.clear();
		handle->shouldClear = false;
	}
	for (auto& filter : handle->audioFilters)
	{
		filter(inBuffer, outBuffer, upper);
		memcpy(&inBuffer[MIX_BUFFER_SIZE / 2], outBuffer, frameCount * sizeof(int16_t));
	}
	curSound = &inBuffer[MIX_BUFFER_SIZE / 2];
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
	handle->offset = (handle->offset + frameCount) % (handle->data->sampleCount);

	if (prev >= handle->offset && !(handle->flags & AUDIO_ENABLE_LOOPING))
		return 1;

	return 0;
}

void EchoFilter(const void * source, void * output, unsigned long frameCount)
{
	int16_t* in = (int16_t*)source;
	int16_t* inStart = &in[MIX_BUFFER_SIZE / 2];
	int16_t* out = (int16_t*)output;


	for (int i = 0; i < frameCount; i++)
	{
		//Simple echo effect by taking samples that were "heard" roughly 400, 280 and 140 milliseconds ago
		//with different weights on them. The recent sounds have more contribution as they are expected
		//to have lost less energy by bouncing around inside the "echo chamber" whereas the
		//least recent sample has the least energy since that sound has been "bouncing" around
		//longer
		*out++ = inStart[i - 18000] * 0.5f + inStart[i - 12000] * 0.7f + inStart[i - 6000] * 0.8f + inStart[i];
	}
	
}

