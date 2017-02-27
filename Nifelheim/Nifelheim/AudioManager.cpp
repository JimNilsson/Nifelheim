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
	if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0)
		throw std::runtime_error("COuld not initialize sdl audio");
	if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) == -1)
		throw std::runtime_error("Could not initilaize SDL_mixer");

	
}

AudioManager::~AudioManager()
{
	for (auto& i : _chunksToDelete)
		Mix_FreeChunk(i);
	Mix_CloseAudio();
}

void AudioManager::GiveAudio(int gameObject, const std::string & filename, int32_t flags)
{
	AudioData* audioData = nullptr;
	auto exists = _audioData.find(filename);
	if (exists == _audioData.end())
	{
		//SNDFILE* sf = nullptr;
		//SF_INFO info = {};
		//sf = sf_open(filename.c_str(), SFM_READ, &info);
		//if (sf == NULL)
		//	throw std::runtime_error(std::string("Could not open file: ").append(filename));
		//if (!(info.format & SF_FORMAT_WAV))
		//{
		//	sf_close(sf);
		//	throw std::runtime_error("Sorry we only support wav because just doing an assignment");
		//}
		Mix_Chunk* audio = Mix_LoadWAV(filename.c_str());
		AudioData ad;
		ad.channels = 2;
		ad.data = (int16_t*)audio->abuf;
		ad.sampleCount = audio->alen;
		ad.sampleRate = 44100;
		_chunksToDelete.push_back(audio);
		//int16_t* rawData = new int16_t[info.frames * info.channels * sizeof(int)];
		//sf_read_int(sf, (int*)rawData, info.frames * info.channels);
		//sf_close(sf);
		//test = rawData;
		//AudioData ad;
		//ad.channels = info.channels;
		//ad.data = (int16_t*)fucker->abuf;
		//ad.sampleCount = info.frames;
		//ad.sampleRate = info.samplerate;
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
	int index = (int)_audioHandles.size();
	_audioHandles.push_back(handle);
	
	const GameObject& go = Core::GetInstance()->GetGameObject(gameObject);
	const_cast<GameObject&>(go).components[Components::AUDIO] = index;
}

void AudioManager::Play(int gameObject)
{
	const GameObject& go = Core::GetInstance()->GetGameObject(gameObject);
	_audioHandles[go.components[Components::AUDIO]].offset = 0;
	_currentlyPlaying.insert(go.components[Components::AUDIO]);
}

void AudioManager::Stop(int gameObject)
{
	const GameObject& go = Core::GetInstance()->GetGameObject(gameObject);
	_audioHandles[go.components[Components::AUDIO]].offset = 0;
	_currentlyPlaying.erase(go.components[Components::AUDIO]);
}

void AudioManager::Pause(int gameObject)
{
	const GameObject& go = Core::GetInstance()->GetGameObject(gameObject);
	auto g = _currentlyPlaying.find(go.components[Components::AUDIO]);
	_currentlyPlaying.erase(go.components[Components::AUDIO]);
}

void AudioManager::SetFlags(int gameObject, AudioSourceFlags flags)
{
	const GameObject& go = Core::GetInstance()->GetGameObject(gameObject);
	_audioHandles[go.components[Components::AUDIO]].flags = flags;
}

void AudioManager::Update(float dt)
{

	if (_load)
	{
		memset(_audioLoadBuffer, 0, AUDIO_CHUNK_SIZE * sizeof(int16_t));
		std::set<uint32_t> swapWithCurrent;
		for (auto& h : _currentlyPlaying)
		{

			int16_t* curSound = (int16_t*)((uint8_t*)_audioHandles[h].data->data + _audioHandles[h].offset);
			
			int upper = (int)std::min((long long)AUDIO_CHUNK_SIZE, _audioHandles[h].data->sampleCount - _audioHandles[h].offset);
			//memcpy(_audioLoadBuffer, curSound, upper * sizeof(int16_t));
			for (int i = 0; i < upper; i++)
			{
				_audioLoadBuffer[i] += (_audioHandles[h].volume / 128.0f) * curSound[i];
			}
			if (_audioHandles[h].flags & AUDIO_ENABLE_LOOPING)
			{
				curSound = (int16_t*)_audioHandles[h].data->data;
				//memcpy(&_audioLoadBuffer[upper], curSound, (AUDIO_CHUNK_SIZE - upper) * sizeof(int16_t));
				for (int i = upper; i < AUDIO_CHUNK_SIZE; i++)
				{
					_audioLoadBuffer[i] += (_audioHandles[h].volume / 128.0f) * curSound[i];
				}
			}
			_audioHandles[h].offset = (_audioHandles[h].offset + AUDIO_CHUNK_SIZE*sizeof(int16_t)) % _audioHandles[h].data->sampleCount;

			if (_audioHandles[h].offset >= AUDIO_CHUNK_SIZE * sizeof(int16_t) || _audioHandles[h].flags & AUDIO_ENABLE_LOOPING)
				swapWithCurrent.insert(h);
		}
		std::swap(_currentlyPlaying, swapWithCurrent);
		

		_load = false;
	}

	if (Mix_Playing(-1) == 0)
	{
		_load = true;
		std::swap(_audioPlayBuffer, _audioLoadBuffer);
		Mix_Chunk chunk;
		chunk.abuf = (Uint8*)_audioPlayBuffer;

		chunk.alen = AUDIO_CHUNK_SIZE * sizeof(int16_t);
		chunk.allocated = 0;
		chunk.volume = 128;

		Mix_PlayChannel(-1, &chunk, 0);		
	}


}

