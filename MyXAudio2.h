#pragma once
#include"ChunkData.h"
#include "DirectXCommon.h"
#include <mmsystem.h>
#include <xaudio2.h>
#include<cassert>
#pragma comment(lib, "winmm.lib")
#pragma comment(lib,"xaudio2.lib")

class MyXAudio2 {
public:

	void Initialize();
	int LoadAudio(const wchar_t* filePath);
	void Release();
	void Play(int AudioInDex);

private:
	static const int kMaxAudio = 8;
	HRESULT hr;
	IXAudio2* XAudioInterface=nullptr;
	IXAudio2MasteringVoice* pMasteringVoice = nullptr;
	IXAudio2SourceVoice* pSourceVoice[kMaxAudio];
	bool IsusedAudioIndex[kMaxAudio];


	void Log(const std::string& message);
};

