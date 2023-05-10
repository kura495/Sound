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

bool DirectSound::LoadFile(SoundFile file_id, const wchar_t* file_name) { 
	
	WavData wav_data;

	if (LoadAudio(file_name, &wav_data) == false) {
		Log("LoadAudio failed");
		return false;
	}

	// バッファ情報の設定
	DSBUFFERDESC dsbd;
	ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
	dsbd.dwSize = sizeof(DSBUFFERDESC);
	dsbd.dwFlags = DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME;
	dsbd.dwBufferBytes = wav_data.Size;
	dsbd.guid3DAlgorithm = DS3DALG_DEFAULT;
	dsbd.lpwfxFormat = &wav_data.WavFormat;

	// セカンダリバッファ作成
	if (FAILED(soundInterFace_->CreateSoundBuffer(
	        &dsbd,                       // バッファ情報
	        &SoundBufferList[file_id], // 作成されたバッファの保存先
	        NULL))) {
		// 作成失敗
		delete[] wav_data.SoundBuffer;
		return false;
	}

	// 波形データを書き込むためにセカンダリバッファをロックする
	void* buffer;
	DWORD buffer_size;
	if (FAILED(SoundBufferList[file_id]->Lock(
	        0,             // オフセット
	        wav_data.Size, // ロックするバッファサイズ
	        &buffer,       // ロックされたバッファパート１の保存先
	        &buffer_size,  // ロックされたバッファパート１のサイズ
	        NULL,          // ロックされたバッファパート２の保存先
	        NULL,          // ロックされたバッファパート２のサイズ
	        0)))           // ロックオプション
	{
		// ロック失敗
		delete[] wav_data.SoundBuffer;
		return false;
	}

	memcpy(buffer, wav_data.SoundBuffer, buffer_size);

	SoundBufferList[file_id]->Unlock(
	    &buffer,     // アンロックするバッファパート１
	    buffer_size, // パート１のバッファサイズ
	    NULL,        // アンロックするバッファパート２
	    NULL);       // パート２のバッファサイズ

	// コピーが終わったのでサウンドデータを解放
	delete[] wav_data.SoundBuffer;
}

bool DirectSound::LoadAudio(const wchar_t* fileName,WavData* out_wave_data) { 
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

	// RIFFチャンクに侵入する
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

	// 進入先のチャンクを"fmt "として設定する
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
	    (HPSTR)&out_wave_data->WavFormat,  // 読み込み用バッファ
	    sizeof(out_wave_data->WavFormat)); // バッファサイズ

	if (read_size != sizeof(out_wave_data->WavFormat)) {
		// 読み込みサイズが一致してないのでエラー
		mmioClose(mmioHandle, MMIO_FHOPEN);
		Log("fmt");
		return false;
	}

	// フォーマットチェック
	if (out_wave_data->WavFormat.wFormatTag != WAVE_FORMAT_PCM) {
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
	out_wave_data->Size = chankInfo.cksize;

	// dataチャンク読み込み
	out_wave_data->SoundBuffer = new char[chankInfo.cksize];
	read_size = mmioRead(mmioHandle, (HPSTR)out_wave_data->SoundBuffer, chankInfo.cksize);
	if (read_size != chankInfo.cksize) {
		mmioClose(mmioHandle, MMIO_FHOPEN);
		delete[] out_wave_data->SoundBuffer;
		Log("data");
		return false;
	}

	// ファイルを閉じる
	mmioClose(mmioHandle, MMIO_FHOPEN);
	return true;
}

void DirectSound::PlayAudio(SoundFile file_id, bool is_loop) { 
		if (SoundBufferList[file_id] == NULL) {
			return;
		}

		int loop_bit = is_loop == true ? 1 : 0;
		// 再生
		SoundBufferList[file_id]->Play(0, 0, DSBPLAY_LOOPING & loop_bit);
}

void DirectSound::Log(const std::string& message) { 
	OutputDebugStringA(message.c_str()); 
}




