#ifndef _AUDIO_MANAGER_H_
#define _AUDIO_MANAGER_H_

#define AUDIO_CHUNK_SIZE 4096*2

#define FRAMES_PER_BUFFER 1024

#include <DirectXMath.h>
#include <unordered_map>
#include <vector>
#include <string>
#include <set>
#include <stdio.h>
#include <stdlib.h>
#include <sndfile.h>
#include <portaudio.h>


typedef void AudioFilter(const void * source, void* output, unsigned long frameCount);

void EchoFilter(const void* source, void* output, unsigned long frameCount);

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
	void SetVolume(int gameObject, uint8_t volume);
	void SetRange(int gameObject, float range);

	void SetFilter(int gameObject, AudioFilter* filter);
	void ClearFilters(int gameObject);

	static int paCallback(const void* input,
		void* output,
		unsigned long frameCount,
		const PaStreamCallbackTimeInfo* timeInfo,
		PaStreamCallbackFlags statusFlags,
		void* userData);

	

private:
	std::vector<PaStream*> _paStreams;
	PaStream *paStream;
	PaError error;

	int16_t lp = 0;
	int16_t rp = 0;
	struct AudioData
	{
		int16_t* data = nullptr;
		int64_t sampleCount = 0;
		int32_t sampleRate = 44100;
		int32_t channels = 2;
	};

	struct AudioHandle
	{
		AudioData* data = nullptr; 
		int32_t flags = 0;
		uint32_t offset = 0; //WHich sample in the file we're at.
		uint8_t volume = 128;
		float range = 150.0f;
		int gameObject;
		bool shouldClear = 0;
		std::vector<AudioFilter*> audioFilters;
	};
	std::vector<AudioFilter*> _globalFilters;
	std::unordered_map<int, PaStream*> _objectToStream;
	std::unordered_map<std::string, AudioData> _audioData;
	std::unordered_map<uint32_t, int32_t> _audioToGameObject;
	std::vector<AudioHandle> _audioHandles;


};


#endif