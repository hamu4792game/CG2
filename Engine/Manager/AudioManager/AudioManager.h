#pragma once
#include <xaudio2.h>
#pragma comment(lib,"xaudio2.lib")
#include <fstream>
#include <wrl.h>
#include <array>


class AudioManager
{
public:
	AudioManager() = default;
	~AudioManager() = default;

	static AudioManager* GetInstance();

	static void Initialize();

private:
	Microsoft::WRL::ComPtr<IXAudio2> xAudio2;
	IXAudio2MasteringVoice* masterVoice;
	
public:
	Microsoft::WRL::ComPtr<IXAudio2> GetAudio2() { return GetInstance()->xAudio2; };

};