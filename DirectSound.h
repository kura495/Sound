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
	enum SoundFile {
		TestBGM,
		TestSE,
		SoundFileMax,
	};
	struct WavData {
		WAVEFORMATEX WavFormat;
		char* SoundBuffer;
		DWORD Size;
	};


	DirectSound();
	~DirectSound();

	void Initialize(WinApp* window);
	bool LoadFile(SoundFile file_id, const wchar_t* file_name);
	bool LoadAudio(const wchar_t* fileName, WavData* out_wave_data);

	void PlayAudio(SoundFile file_id, bool is_loop);
	
	
	
private:
	//
	void Log(const std::string& message);
	HRESULT hr = 0;
	//インターフェイスの保存先
	LPDIRECTSOUND8 soundInterFace_=nullptr;

	// 波形データ
	//std::unordered_map < std::string, SoundData> waveFormData;
};


