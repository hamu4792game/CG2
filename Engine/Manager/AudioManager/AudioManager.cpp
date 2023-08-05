#include "AudioManager.h"

AudioManager* AudioManager::GetInstance()
{
	static AudioManager instance;
	return &instance;
}

void AudioManager::Initialize()
{
	HRESULT result = 0;
	//	XAudioエンジンのインスタンス生成
	result = XAudio2Create(&AudioManager::GetInstance()->xAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
	//	マスターボイスの生成
	result = AudioManager::GetInstance()->xAudio2->CreateMasteringVoice(&AudioManager::GetInstance()->masterVoice);
}
