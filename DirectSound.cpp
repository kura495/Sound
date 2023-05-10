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

void DirectSound::LoadAudio(LPWSTR fileName) { 
	
	mmioHandle = mmioOpen(
	   fileName, // ファイル名
	    NULL,             // MMIO情報
	    MMIO_READ);       // モード
}

void DirectSound::PlayAudio() { 
	
}

void DirectSound::Log(const std::string& message) { 
	OutputDebugStringA(message.c_str()); 
}

std::wstring DirectSound::ConvertString(const std::string& str) {
	if (str.empty()) {
		return std::wstring();
	}

	auto sizeNeeded = MultiByteToWideChar(
	    CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), NULL, 0);
	if (sizeNeeded == 0) {
		return std::wstring();
	}
	std::wstring result(sizeNeeded, 0);
	MultiByteToWideChar(
	    CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()),
	    &result[0], sizeNeeded);
	return result;
}




