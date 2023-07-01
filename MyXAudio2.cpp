#include"MyXAudio2.h"

void MyXAudio2::Initialize() { 
	hr=XAudio2Create(&XAudioInterface);
	assert(SUCCEEDED(hr));
	hr = XAudioInterface->CreateMasteringVoice(&pMasteringVoice);
	assert(SUCCEEDED(hr));
	for (int i = 0; i < kMaxAudio; i++) {
		pSourceVoice[i] = nullptr;
		IsusedAudioIndex[i] = false;
	}
}

int MyXAudio2::LoadAudio(const wchar_t* filePath) { 
	//位置決め
#pragma region Index
	int AudioIndex = kMaxAudio + 1;
	    for (int i = 0; i < kMaxAudio; ++i) {
		if (IsusedAudioIndex[i] == false) {
			AudioIndex = i;
			IsusedAudioIndex[i] = true;
			break;
		}
	    }
	    if (AudioIndex < 0) {
		// 0より少ない
		assert(false);
	    }
	    if (kMaxAudio < AudioIndex) {
		// MaxSpriteより多い
		assert(false);
	    }
#pragma endregion 位置決め
	    // WindowsマルチメディアAPIのハンドル
	    HMMIO mmioHandle = NULL;

	    // ファイルを開く
	    mmioHandle = mmioOpen(
	        (LPWSTR)filePath, // ファイル名
	        NULL,             // MMIO情報
	        MMIO_READ);       // モード
	    if (mmioHandle == NULL) {
		Log("open failed\n");
		assert(false);
	    }
#pragma region RIFF
		// RIFFチャンク用
	    MMCKINFO RIFFchankInfo;
	    // RIFFチャンクに進入するためにfccTypeにWAVEを設定する
	    RIFFchankInfo.fccType = mmioFOURCC('W', 'A', 'V', 'E');
	    // RIFFチャンクに入る
	    if (MMSYSERR_NOERROR != mmioDescend(
	                                mmioHandle,     // MMIOハンドル
	                                &RIFFchankInfo, // 取得したチャンクの情報
	                                NULL,           // 親チャンク
	                                MMIO_FINDRIFF)) // 取得情報の種類
	    {
		// 失敗
		mmioClose(mmioHandle, MMIO_FHOPEN);
		Log("RIFF open failed\n");
		assert(false);
	    }
	#pragma endregion RIFF
#pragma region fmt
		// チャンク情報
	    MMCKINFO chankInfo;
	    // fmtチャンクに入るためにIDに"fmt "を入れる
	    chankInfo.ckid = mmioFOURCC('f', 'm', 't', ' ');
	    if (MMSYSERR_NOERROR !=
	        mmioDescend(mmioHandle, &chankInfo, &RIFFchankInfo, MMIO_FINDCHUNK)) {
		// fmtチャンクがない
		mmioClose(mmioHandle, MMIO_FHOPEN);
		Log("fmt");
		return false;
	    }
	    WAVEFORMATEX wfex{};
	    // fmtデータの読み込み
	    LONG read_size = mmioRead(
	        mmioHandle,                      // ハンドル
	        (HPSTR)&wfex,                    // 読み込み用バッファ
	        sizeof(wfex)); // バッファサイズ

	    if (read_size != sizeof(wfex)) {
		// 読み込みサイズが一致してないのでエラー
		mmioClose(mmioHandle, MMIO_FHOPEN);
		Log("fmt open failed\n");
		assert(false);
	    }

	    // フォーマットチェック
	    if (wfex.wFormatTag != WAVE_FORMAT_PCM) {
		// フォーマットエラー
		mmioClose(mmioHandle, MMIO_FHOPEN);
		assert(false);
	    }

	    // fmtチャンクを退出する
	    if (mmioAscend(mmioHandle, &chankInfo, 0) != MMSYSERR_NOERROR) {
		mmioClose(mmioHandle, MMIO_FHOPEN);
		assert(false);
	    }
#pragma endregion fmt
	    // dataチャンクに進入する
	    chankInfo.ckid = mmioFOURCC('d', 'a', 't', 'a');
	    if (mmioDescend(mmioHandle, &chankInfo, &RIFFchankInfo, MMIO_FINDCHUNK) !=
	        MMSYSERR_NOERROR) {
		// 進入失敗
		mmioClose(mmioHandle, MMIO_FHOPEN);
		Log("data open failed\n");
		assert(false);
	    }

	    // サイズを保存
	    //outWaveData->Size = chankInfo.cksize;
	    char* pBuffer = (char*)malloc(chankInfo.cksize);

	    // dataチャンク読み込み
	    read_size = mmioRead(mmioHandle, (HPSTR)pBuffer, chankInfo.cksize);
	    if (read_size != chankInfo.cksize) {
		mmioClose(mmioHandle, MMIO_FHOPEN);
		delete[] pBuffer;
		Log("data open failed\n");
		assert(false);
	    }
	    if (FAILED(XAudioInterface->CreateSourceVoice(&pSourceVoice[AudioIndex], &wfex))) {
		free(pBuffer);
		assert(false);
	    }
	    XAUDIO2_BUFFER buffer{};
	    buffer.pAudioData = (BYTE*)pBuffer;
	    buffer.Flags = XAUDIO2_END_OF_STREAM;
	    buffer.AudioBytes = chankInfo.cksize;
	    buffer.LoopBegin = 0;
	    buffer.LoopLength = 0;
	    buffer.LoopCount = XAUDIO2_LOOP_INFINITE;
	    pSourceVoice[AudioIndex]->SubmitSourceBuffer(&buffer);
	    // ファイルを閉じる
	    mmioClose(mmioHandle, MMIO_FHOPEN);
		return AudioIndex;
}

void MyXAudio2::Release() { 
	
	for (int i=0;i<kMaxAudio;i++) {
		if (pSourceVoice[i]) {
		pSourceVoice[i]->DestroyVoice();
		}
	}
	if (pMasteringVoice) {
		pMasteringVoice->DestroyVoice();
		pMasteringVoice = nullptr;
	}
	if (XAudioInterface) {
		XAudioInterface->Release();
		XAudioInterface = nullptr;
	}
	CoUninitialize();
}

void MyXAudio2::Play(int AudioIndex) { 
	pSourceVoice[AudioIndex]->Start();
}

void MyXAudio2::Log(const std::string& message) { OutputDebugStringA(message.c_str()); }