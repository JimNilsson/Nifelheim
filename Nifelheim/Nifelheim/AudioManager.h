#ifndef _AUDIO_MANAGER_H_
#define _AUDIO_MANAGER_H_

#define AUDIO_CHUNK_SIZE 4096*4

#include <DirectXMath.h>
#include <unordered_map>
#include <vector>
#include <string>
#include <set>
#include <stdio.h>
#include <stdlib.h>

#include <SDL_mixer.h>
enum AudioSourceFlags : int32_t
{
	AUDIO_ENABLE_STEREO_PANNING = 1 << 0,
	AUDIO_ENABLE_MAX_RANGE = 1 << 1,
	AUDIO_ENABLE_LOOPING = 1 << 2
};

class AudioManager
{
public:
	AudioManager();
	~AudioManager();

	void GiveAudio(int gameObject, const std::string& filename, int32_t flags = 0);
	void Play(int gameObject);
	void Stop(int gameObject);
	void Pause(int gameObject);
	void SetFlags(int gameObject, AudioSourceFlags flags);
	void Update(float dt);

private:


	struct SoundReceiver
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 forward;
		DirectX::XMFLOAT3 right;
	};

	SoundReceiver receiver;

	struct AudioData
	{
		int16_t* data = nullptr;
		int64_t sampleCount = 0;
		int32_t sampleRate = 44100;
		int32_t channels = 2;
	};

	struct AudioHandle
	{
		AudioData* data = nullptr; //Borrows a pointer form Mix_Chunk, dont delete
		int32_t flags = 0;
		uint32_t offset = 0; //WHich sample in the file we're at.
		uint8_t volume = 128;
	};
	std::unordered_map<std::string, AudioData> _audioData;
	std::vector<AudioHandle> _audioHandles;
	std::set<uint32_t> _currentlyPlaying;
	std::vector<Mix_Chunk*> _chunksToDelete;

	int16_t _audioPlayBuffer[AUDIO_CHUNK_SIZE] = {};
	int16_t _audioLoadBuffer[AUDIO_CHUNK_SIZE] = {};

	bool _load = true;


};


#endif