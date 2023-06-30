#include"MyXAudio2.h"

void MyXAudio2::Initialize() { 
	hr=XAudio2Create(&XAudioInterface);
	assert(hr);
	hr = XAudioInterface->CreateMasteringVoice(&pMasteringVoice);
	assert(hr);
	for (int i = 0; i < kMaxAudio; i++) {
		pSourceVoice[i] = nullptr;
		IsusedAudioIndex[i] = false;
	}
}

int MyXAudio2::LoadAudio(const char& filePath) {
	FILE* file=nullptr;

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
	
	if (fopen_s(&file, &filePath, "rb") != 0) {
	
	}
	
		
		return AudioIndex;

}

void MyXAudio2::Release() { 
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

void Log(const std::string& message) { 
	OutputDebugStringA(message.c_str()); 
}