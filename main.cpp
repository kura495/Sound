#include "Audio.h"
#include "AxisIndicator.h"
#include "DirectXCommon.h"
#include "GameScene.h"
#include "ImGuiManager.h"
#include "PrimitiveDrawer.h"
#include "TextureManager.h"
#include "WinApp.h"
#include"DirectSound.h"

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	WinApp* win = nullptr;
	DirectXCommon* dxCommon = nullptr;
	// 汎用機能
	Input* input = nullptr;
	Audio* audio = nullptr;
	AxisIndicator* axisIndicator = nullptr;
	PrimitiveDrawer* primitiveDrawer = nullptr;
	GameScene* gameScene = nullptr;
	//DirectSound
	DirectSound* directSound=nullptr;
	
	// ゲームウィンドウの作成
	win = WinApp::GetInstance();
	win->CreateGameWindow();

	// DirectX初期化処理
	dxCommon = DirectXCommon::GetInstance();
	dxCommon->Initialize(win);
#pragma region 汎用機能初期化
	// ImGuiの初期化
	ImGuiManager* imguiManager = ImGuiManager::GetInstance();
	imguiManager->Initialize(win, dxCommon);

	// 入力の初期化
	input = Input::GetInstance();
	input->Initialize();

	// オーディオの初期化
	audio = Audio::GetInstance();
	audio->Initialize();

	// テクスチャマネージャの初期化
	TextureManager::GetInstance()->Initialize(dxCommon->GetDevice());
	TextureManager::Load("white1x1.png");

	// スプライト静的初期化
	Sprite::StaticInitialize(dxCommon->GetDevice(), WinApp::kWindowWidth, WinApp::kWindowHeight);

	// 3Dモデル静的初期化
	Model::StaticInitialize();

	// 軸方向表示初期化
	axisIndicator = AxisIndicator::GetInstance();
	axisIndicator->Initialize();

	primitiveDrawer = PrimitiveDrawer::GetInstance();
	primitiveDrawer->Initialize();
#pragma endregion

	//DirectSound初期化
	directSound = new DirectSound();
	directSound->Initialize(win);

	directSound->LoadFile(DirectSound::SoundFile::TestBGM,L"./Resources/mokugyo.wav");

	POINT MouseCurcor;
	int PAN=0;

	// ゲームシーンの初期化
	gameScene = new GameScene();
	gameScene->Initialize();
	
	// メインループ
	while (true) {
		// メッセージ処理
		if (win->ProcessMessage()) {
			break;
		}
		// ImGui受付開始
		imguiManager->Begin();
		// 入力関連の毎フレーム処理
		input->Update();
		// ゲームシーンの毎フレーム処理
		gameScene->Update();

		GetCursorPos(&MouseCurcor);
		MouseCurcor.x -= 640;
		PAN = MouseCurcor.x*10;
		if (PAN > 10000) {
			PAN = 10000;
		} else if (PAN < -10000) {
			PAN = -10000;
		}
		//DSBPAN_LEFT -10000 右チャンネルが100デシベル分減衰する　右が無音になる
		//DSBPAN_RIGHT 10000 左チャンネルが100デシベル分減衰する　左が無音になる
		//DSBPAN_CENTER 0 両方のチャンネルから音が出る
		directSound->SetPan(DirectSound::SoundFile::TestBGM, PAN);
		//再生
		directSound->PlayAudio(DirectSound::SoundFile::TestBGM, 1);
		

		// 軸表示の更新
		axisIndicator->Update();
		// ImGui受付終了
		imguiManager->End();

		// 描画開始
		dxCommon->PreDraw();
		
		// ゲームシーンの描画
		gameScene->Draw();
		// 軸表示の描画
		axisIndicator->Draw();
		// プリミティブ描画のリセット
		primitiveDrawer->Reset();
		// ImGui描画
		imguiManager->Draw();
		// 描画終了
		dxCommon->PostDraw();
	}

	// 各種解放
	SafeDelete(gameScene);
	audio->Finalize();
	delete directSound;
	// ImGui解放
	imguiManager->Finalize();

	// ゲームウィンドウの破棄
	win->TerminateGameWindow();

	return 0;
}