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
#include<array>
class DirectSound {
public:
	DirectSound();
	~DirectSound();

	struct SoundData {
		//サンプリング周波数
		unsigned short sample;
		//量子化ビット数
		unsigned char bit;
		//チャンネル数
		unsigned char channel;
		//バッファ(dataチャンクデータ)
		LPDIRECTSOUNDBUFFER buffer;
		//ファイル名
		std::string name;
	};
	typedef struct {
		char ID[4];//チャンクID
		long size;//ファイル全体のサイズ
		char formatType[4];//フォーマットタイプ
	} RIFF_CHUNK;
	// RIFFチャンクの定義

	typedef struct {
		char ID[4];//チャンクID
		long size;//FMTチャンクのサイズ
		short waveFormatType;//waveのタイプ
		short channel;//チャンネル数
		long sample;//サンプリング周波数
		long byte;//1回で処理するデータサイズ(sanple*channel)
		short block;//1サンプルのサイズ(bit/8*channel)
		short bit;//量子化ビット数
	} FMT_CHUNK;
	// fmtチャンクの定義

	typedef struct {
		char ID[4];//チャンクID
		long size;//DATAチャンクのサイズ
		std::vector<float> data; // 波形データ
	} DATA_CHUNK;
	// dataチャンクの定義

	typedef struct {
		RIFF_CHUNK riffChunk;
		FMT_CHUNK fmtChunk;
		DATA_CHUNK dataChunk;
	} WAVE_FORMAT;
	// WAVEフォーマット

	void Initialize(WinApp* window);
	
	void LoadAudio(const std::string& filename);

	void PlayAudio();
	
	
	
private:
	//
	void Log(const std::string& message);

	HRESULT hr = 0;
	//インターフェイスの保存先
	LPDIRECTSOUND8 soundInterFace_=nullptr;

	// 波形データ
	//std::unordered_map < std::string, SoundData> waveFormData;
	SoundData soundData;
};


