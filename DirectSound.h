#pragma once
#include "DirectXCommon.h"
//DirectSoundのライブラリ
#pragma comment(lib, "dsound.lib")
//MSBuildで用いられるプロジェクトIDや、オブジェクト認識のためのクラスID、インターフェイスIDなど
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")
#include<mmsystem.h>
#include"dsound.h"
#include<cassert>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
class DirectSound {
public:
	DirectSound();
	~DirectSound();

	void Initialize(WinApp* window);
	
	void LoadAudio(LPWSTR fileName);

	void PlayAudio();
	
	
	
private:
	//
	void Log(const std::string& message);
	std::wstring ConvertString(const std::string& str);
	HRESULT hr = 0;
	//インターフェイスの保存先
	LPDIRECTSOUND8 soundInterFace_=nullptr;

	HMMIO mmioHandle;

	// 波形データ
	//std::unordered_map < std::string, SoundData> waveFormData;
};


