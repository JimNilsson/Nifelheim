#ifndef _AUDIO_MANAGER_H_
#define _AUDIO_MANAGER_H_

#include <DirectXMath.h>
#include <unordered_map>
#include <vector>
#include <string>

enum AudioSourceFlags : uint32_t
{
	AUDIO_ENABLE_STEREO_PANNING = 1 << 0,
	AUDIO_ENABLE_MAX_RANGE = 1 << 1


};

class AudioManager
{
public:
	AudioManager();
	~AudioManager();

	void GiveAudio(int gameObject, const std::string& filename);
	void Play(int gameObject);
	void Stop(int gameObject);
	void Pause(int gameObject);
	void SetFlags(int gameObject, AudioSourceFlags flags);

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
		uint8_t* data = nullptr;
		uint32_t sampleCount = 0;
		uint32_t sampleRate = 44100;
	};

	struct AudioHandle
	{
		AudioData* data = nullptr;
		uint32_t offset;
	};
	std::unordered_map<std::string, AudioData> _audioData;
	std::vector<AudioHandle> _audioHandles;

};


#endif