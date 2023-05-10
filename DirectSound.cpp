#include"DirectSound.h"
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
	
	
}

void DirectSound::LoadAudio(const std::string& fileName) { 
	WAVE_FORMAT waveFormat;
	FILE* file = nullptr;
	if (fopen_s(&file, fileName.c_str(), "rb") != 0) {
		Log("wav open failed\n");
		return;
	} else {
		Log("wav open success\n");
	}
	//RIFFチャンクの読み込み
	//fread(waveFormat.riffChunk.ID, 1, 4, file);
	//fread(&waveFormat.riffChunk.size, 4, 1, file);
	//fread(waveFormat.riffChunk.formatType, 1, 4, file);
	fread_s(&waveFormat.riffChunk, sizeof(RIFF_CHUNK), sizeof(RIFF_CHUNK), 1, file);
	std::string id(&waveFormat.riffChunk.ID[0], sizeof(waveFormat.riffChunk.ID));
	std::string type(&waveFormat.riffChunk.formatType[0], sizeof(waveFormat.riffChunk.formatType));
	if (id != "RIFF" || type != "WAVE") {
		Log("failed");
		return;
	}
	//Log(waveFormat.riffChunk.ID);
	//Log("\n");
	//Log(waveFormat.riffChunk.formatType);
	//Log("\n");

	//fmtチャンクの読み込み
	//fread(waveFormat.fmtChunk.ID, 1, 4, file);
	//fread(&waveFormat.fmtChunk.size, 4, 1, file);
	//fread(&waveFormat.fmtChunk.waveFormatType, 2, 1, file);
	//fread(&waveFormat.fmtChunk.channel, 2, 1, file);
	//fread(&waveFormat.fmtChunk.sample, 4, 1, file);
	//fread(&waveFormat.fmtChunk.byte, 4, 1, file);
	//fread(&waveFormat.fmtChunk.block, 2, 1, file);
	//fread(&waveFormat.fmtChunk.bit, 2, 1, file);
	fread_s(&waveFormat.fmtChunk, sizeof(FMT_CHUNK), sizeof(FMT_CHUNK), 1, file);
	//Log(waveFormat.riffChunk.ID);
	//Log("\n");
	//Log(waveFormat.riffChunk.formatType);
	//Log("\n");
	std::string id2(&waveFormat.fmtChunk.ID[0], sizeof(waveFormat.fmtChunk.ID));
	if (id2 != "fmt ") {
		Log("failed");
		return;
	}
	//FMTチャンクに拡張部分がある場合がある
	//std::vector<char> expansion(waveFormat.fmtChunk.size -( sizeof(waveFormat.fmtChunk) - sizeof(waveFormat.fmtChunk.ID) - sizeof(waveFormat.fmtChunk.size)));
	//fread_s(expansion.data(), sizeof(char) * expansion.size(), sizeof(char) * expansion.size(), 1,file);
	
	//dataチャンク読み込み

	fread(waveFormat.dataChunk.ID, 1, 4, file);
	fread(&waveFormat.dataChunk.size, 4, 1, file);
	for (int i = 0; i < waveFormat.dataChunk.size; i++) {
		fread(&waveFormat.dataChunk.data,2,1,file);
	}
	//fread_s(&waveFormat.dataChunk, sizeof(DATA_CHUNK), sizeof(DATA_CHUNK), 1, file);
	std::string id3(&waveFormat.dataChunk.ID[0], sizeof(waveFormat.dataChunk.ID));
	if (id3 != "data") {
		Log("failed");
		return;
	}
	/*soundData.sample = waveFormat.fmtChunk.sample;
	soundData.bit = waveFormat.fmtChunk.bit;
	soundData.channel=waveFormat*/
	fclose(file);

	// セカンダリバッファの情報設定
	WAVEFORMATEX Info{};
	Info.wFormatTag=waveFormat.fmtChunk.waveFormatType;
	Info.nChannels=waveFormat.fmtChunk.channel;
	Info.nSamplesPerSec=waveFormat.fmtChunk.sample;
	Info.wBitsPerSample=waveFormat.fmtChunk.bit;
	Info.nBlockAlign=waveFormat.fmtChunk.block;
	Info.nAvgBytesPerSec=waveFormat.fmtChunk.byte;
	Info.cbSize=0;
	DSBUFFERDESC dsBufferDesc{};
	dsBufferDesc.dwSize = sizeof(DSBUFFERDESC);
	dsBufferDesc.dwFlags = 0;
	dsBufferDesc.dwBufferBytes = Info.nSamplesPerSec * Info.wBitsPerSample*Info.nChannels;
	dsBufferDesc.dwReserved = 0;
	dsBufferDesc.lpwfxFormat = &Info;
	dsBufferDesc.guid3DAlgorithm = GUID_NULL;
	soundInterFace_->CreateSoundBuffer(&dsBufferDesc, &SoundBufferList, NULL);
	LPVOID buffer;
	DWORD bufferSize;
	if (FAILED(soundData.buffer->Lock(
	        0,waveFormat.dataChunk.size, &buffer, &bufferSize, NULL, NULL, 0))) {
		Log("Lock failed");
		return;
	}
	memcpy(buffer, &waveFormat.dataChunk.data, sizeof(waveFormat.dataChunk.data));
	soundData.buffer->Unlock(&buffer,bufferSize,NULL,0);
	

	/*IDirectSoundBuffer* tmpBuffer = 0;
	IDirectSoundBuffer8* DSBuffer;
	soundInterFace_->CreateSoundBuffer(&dsBufferDesc, &tmpBuffer, NULL);
	tmpBuffer->QueryInterface(IID_IDirectSoundBuffer8, (void**)&DSBuffer);
	tmpBuffer->Release();
	dsBufferDesc.dwSize = sizeof(DSBUFFERDESC);
	dsBufferDesc.dwFlags = DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME;
	dsBufferDesc.dwBufferBytes = waveFormat.dataChunk.size;
	LPVOID lpvWrite = 0;
	DWORD dwLength = 0;
	if (DS_OK == DSBuffer->Lock(0, 0, &lpvWrite, &dwLength, NULL, NULL, DSBLOCK_ENTIREBUFFER)) {
		memccpy(lpvWrite,waveFormat.dataChunk.,)
	}*/
	
	
}

void DirectSound::PlayAudio() { 
	soundData.buffer->Play(0, 0, 0);
}

void DirectSound::Log(const std::string& message) { 
	OutputDebugStringA(message.c_str()); 
}





