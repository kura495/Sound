#include"DirectSound.h"



LPDIRECTSOUNDBUFFER SoundBufferList[DirectSound::SoundFile::SoundFileMax];

DirectSound::DirectSound() {

}

DirectSound::~DirectSound() {

}
/// <summary>
/// DirectSoundの初期化
/// </summary>
/// <param name="window">音を出すウィンドウを指定</param>
void DirectSound::Initialize(WinApp* window) { 
		hr = DirectSoundCreate8(
	    NULL,             // サウンドデバイスを表すGUID　NULLの場合標準のデバイス
	    &soundInterFace_, // 生成したDirectSound保存先
	    NULL              // NULL固定
	);
	assert(SUCCEEDED(hr));
	//協調レベルを設定
	hr = soundInterFace_->SetCooperativeLevel(
	    window->GetHwnd(), // ウィンドウの指定
	    DSSCL_NORMAL // 標準協調レベル
	);
	assert(SUCCEEDED(hr));
	for (int i = 0; i < SoundFile::SoundFileMax; i++) {
		SoundBufferList[i] = NULL;
	}
	
}

bool DirectSound::LoadFile(SoundFile fileID, const wchar_t* fileName) { 
	
	WavData wavData;

	if (LoadAudio(fileName, &wavData) == false) {
		Log("LoadAudio failed");
		return false;
	}

	// バッファ情報の設定
	DSBUFFERDESC dsBufferDesc;
	ZeroMemory(&dsBufferDesc, sizeof(DSBUFFERDESC));
	dsBufferDesc.dwSize = sizeof(DSBUFFERDESC);
	dsBufferDesc.dwFlags = DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME;
	dsBufferDesc.dwBufferBytes = wavData.Size;
	dsBufferDesc.guid3DAlgorithm = DS3DALG_DEFAULT;
	dsBufferDesc.lpwfxFormat = &wavData.WavFormat;
	
	// セカンダリバッファ作成
	if (FAILED(soundInterFace_->CreateSoundBuffer(
	        &dsBufferDesc,                       // バッファ情報
	        &SoundBufferList[fileID], // 作成されたバッファの保存先
	        NULL))) {
		// 作成失敗
		delete[] wavData.SoundBuffer;
		return false;
	}

	// 波形データを書き込むためにセカンダリバッファをロックする
	void* buffer;
	DWORD buffer_size;
	if (FAILED(SoundBufferList[fileID]->Lock(
	        0,             // オフセット
	        wavData.Size, // ロックするバッファサイズ
	        &buffer,       // ロックされたバッファパート１の保存先
	        &buffer_size,  // ロックされたバッファパート１のサイズ
	        NULL,          // ロックされたバッファパート２の保存先
	        NULL,          // ロックされたバッファパート２のサイズ
	        0)))           // ロックオプション
	{
		// ロック失敗
		delete[] wavData.SoundBuffer;
		return false;
	}
	
	memcpy(buffer, wavData.SoundBuffer, buffer_size);

	SoundBufferList[fileID]->Unlock(
	    &buffer,     // アンロックするバッファパート１
	    buffer_size, // パート１のバッファサイズ
	    NULL,        // アンロックするバッファパート２
	    NULL);       // パート２のバッファサイズ

	// コピーが終わったのでサウンドデータを解放
	delete[] wavData.SoundBuffer;
}

bool DirectSound::LoadAudio(const wchar_t* fileName,WavData* outWaveData) { 
	// WindowsマルチメディアAPIのハンドル
	HMMIO mmioHandle = NULL;

	// チャンク情報
	MMCKINFO chankInfo;
	// RIFFチャンク用
	MMCKINFO RIFFchankInfo;

	// WAVファイル内音サンプルのサイズ
	//DWORD dwWavSize = 0;
	mmioHandle = mmioOpen(
	   (LPWSTR)fileName, // ファイル名
	    NULL,             // MMIO情報
	    MMIO_READ);       // モード
	if (mmioHandle == NULL) {
		Log("open failed");
		return false;
	}
	

	// RIFFチャンクに進入するためにfccTypeにWAVEを設定する
	RIFFchankInfo.fccType = mmioFOURCC('W', 'A', 'V', 'E');

	// RIFFチャンクに入る
	if (MMSYSERR_NOERROR != mmioDescend(
	                            mmioHandle,    // MMIOハンドル
	                            &RIFFchankInfo,   // 取得したチャンクの情報
	                            NULL,           // 親チャンク
	                            MMIO_FINDRIFF)) // 取得情報の種類
	{
		// 失敗
		mmioClose(mmioHandle, MMIO_FHOPEN);
		Log("RIFF");
		return false;
	}


	// fmtチャンクに入るためにIDに"fmt "を入れる
	chankInfo.ckid = mmioFOURCC('f', 'm', 't', ' ');
	if (MMSYSERR_NOERROR != mmioDescend(mmioHandle, &chankInfo, &RIFFchankInfo, MMIO_FINDCHUNK)) {
		// fmtチャンクがない
		mmioClose(mmioHandle, MMIO_FHOPEN);
		Log("fmt");
		return false;
	}


	// fmtデータの読み込み
	LONG read_size = mmioRead(
	    mmioHandle,                       // ハンドル
	    (HPSTR)&outWaveData->WavFormat,  // 読み込み用バッファ
	    sizeof(outWaveData->WavFormat)); // バッファサイズ

	if (read_size != sizeof(outWaveData->WavFormat)) {
		// 読み込みサイズが一致してないのでエラー
		mmioClose(mmioHandle, MMIO_FHOPEN);
		Log("fmt");
		return false;
	}

	// フォーマットチェック
	if (outWaveData->WavFormat.wFormatTag != WAVE_FORMAT_PCM) {
		// フォーマットエラー
		mmioClose(mmioHandle, MMIO_FHOPEN);
		return false;
	}

	// fmtチャンクを退出する
	if (mmioAscend(mmioHandle, &chankInfo, 0) != MMSYSERR_NOERROR) {
		mmioClose(mmioHandle, MMIO_FHOPEN);
		return false;
	}

	// dataチャンクに進入する
	chankInfo.ckid = mmioFOURCC('d', 'a', 't', 'a');
	if (mmioDescend(mmioHandle, &chankInfo, &RIFFchankInfo, MMIO_FINDCHUNK) != MMSYSERR_NOERROR) {
		// 進入失敗
		mmioClose(mmioHandle, MMIO_FHOPEN);
		Log("data");
		return false;
	}

	// サイズを保存
	outWaveData->Size = chankInfo.cksize;

	// dataチャンク読み込み
	outWaveData->SoundBuffer = new char[chankInfo.cksize];
	read_size = mmioRead(mmioHandle, (HPSTR)outWaveData->SoundBuffer, chankInfo.cksize);
	if (read_size != chankInfo.cksize) {
		mmioClose(mmioHandle, MMIO_FHOPEN);
		delete[] outWaveData->SoundBuffer;
		Log("data");
		return false;
	}

	// ファイルを閉じる
	mmioClose(mmioHandle, MMIO_FHOPEN);
	return true;
}


void DirectSound::PlayAudio(SoundFile fileID, bool is_loop) { 
		if (SoundBufferList[fileID] == NULL) {
			return;
		}

		int loop_bit = is_loop == true ? 1 : 0;
		// 再生
		SoundBufferList[fileID]->Play(0, 0, DSBPLAY_LOOPING & loop_bit);
	   
}

void DirectSound::SetPan(SoundFile fileID, int number) { 
	SoundBufferList[fileID]->SetPan(number);
}



void DirectSound::Log(const std::string& message) { 
	OutputDebugStringA(message.c_str()); 
}




