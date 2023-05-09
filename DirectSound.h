#pragma once
#include "DirectXCommon.h"

#include <windows.h>
#include <xaudio2.h>
#include <iostream>
class DirectSound {
public:
	DirectSound();
	~DirectSound();



	void Initialize(WinApp* window);
	
	void LoadAudio(const std::string& filename);

	void PlayAudio();
	
	void Reset();
	
private:
	//
	void Log(const std::string& message);
	HRESULT hr;
	IXAudio2* xaudio;
	IXAudio2MasteringVoice* mastering_voice;

	// 波形データ
	//std::unordered_map < std::string, SoundData> waveFormData;

};


