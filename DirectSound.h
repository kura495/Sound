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
class DirectSound {
public:

	enum SoundFile {
		TestBGM,
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
	bool LoadFile(SoundFile fileID, const wchar_t* fileName);
	bool LoadAudio(const wchar_t* fileName, WavData* outWaveData);

	void PlayAudio(SoundFile fileID, bool is_loop);
	
	void SetPan(SoundFile fileID, int number);
	
private:
	//
	void Log(const std::string& message);

	HRESULT hr = 0;
	//インターフェースの保存先
	LPDIRECTSOUND8 soundInterFace_=nullptr;

	//セカンダリバッファ格納場所
	LPDIRECTSOUNDBUFFER SoundBufferList[DirectSound::SoundFile::SoundFileMax];

};


