#include "GameScene.h"
#include "KamataEngine.h"
#include "TitleScene.h"
#include "stageManager.h"

#include <Windows.h>

using namespace KamataEngine;

GameScene* gameScene = nullptr;
TitleScene* titleScene = nullptr;
stageManager* stageManager_ = nullptr;

enum class Scene {
	kUnknown = 0,
	kTitle,
	kGame,
};

Scene scene = Scene::kTitle;

void ChangeScene();
void UpdateScene();
void DrawScene();

// デバッグ用：毎フレーム呼ばれることで正しくキー入力を受け付ける
static void LoadDebugSettings() {
	if (Input::GetInstance()->TriggerKey(DIK_P)) {
		// ← ここにブレークポイントを置くか、
		OutputDebugStringA("DEBUG: DIK_RIGHT triggered!\n"); // これを追加

		stageManager_->SetCurrentStageIndexByName("field03");
		if (scene == Scene::kGame && gameScene != nullptr) {
			gameScene->reloadRequested_ = true;
			OutputDebugStringA("DEBUG: reloadRequested_ = true\n");
		}
	}
}


int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	//============================
	// 初期化
	//============================

	KamataEngine::Initialize(L"AE2?_99_ヤマト_ユウヤ_AL2");
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	ImGuiManager* imguiManager = ImGuiManager::GetInstance();

	scene = Scene::kTitle;
	stageManager_ = new stageManager;
	stageManager_->LoadStageDataCsv();

	titleScene = new TitleScene();
	titleScene->Initialize();

	//============================
	// メインループ
	//============================

	while (true) {

		if (KamataEngine::Update()) {
			break;
		}

		imguiManager->Begin();

		//======================================================================
		// 更新処理ここから
		//======================================================================

		// ✅ デバッグキー入力は毎フレーム・シーン切り替えより前にチェック
		

		ChangeScene();
		UpdateScene();

		//======================================================================
		// 更新処理ここまで
		//======================================================================

		imguiManager->End();

		//======================================================================
		// 描画処理ここから
		//======================================================================

		dxCommon->PreDraw();

		DrawScene();

		imguiManager->Draw();

		dxCommon->PostDraw();

		//======================================================================
		// 描画処理ここまで
		//======================================================================

		if (Input::GetInstance()->PushKey(DIK_ESCAPE)) {
			break;
		}
	}

	//---------------------------------------------------
	// 解放
	//---------------------------------------------------

	delete gameScene;
	delete titleScene;
	delete stageManager_;
	gameScene = nullptr;
	titleScene = nullptr;
	stageManager_ = nullptr;

	KamataEngine::Finalize();
	return 0;
}

#pragma region ゲームのシーンの切り替え
void ChangeScene() {

	switch (scene) {
	case Scene::kTitle:
		if (titleScene->IsFinished()) {


			scene = Scene::kGame;
			delete titleScene;
			titleScene = nullptr;

			gameScene = new GameScene;
			gameScene->Initialize(stageManager_);
		}
		break;

	case Scene::kGame:
		if (gameScene->IsFinished()) {
			scene = Scene::kTitle;
			delete gameScene;
			gameScene = nullptr;

			titleScene = new TitleScene;
			titleScene->Initialize();

		} else if (gameScene->reloadRequested_) {
			// ✅ リロード：stageManager_はそのまま使いシーンだけ作り直す
			delete gameScene;
			gameScene = nullptr;

			gameScene = new GameScene;
			gameScene->Initialize(stageManager_);
		}
		break;

	default:
		break;
	}
}
#pragma endregion

#pragma region ゲームの更新処理
void UpdateScene() {
	switch (scene) {
	case Scene::kTitle:
		LoadDebugSettings();

		titleScene->Update();
		break;
	case Scene::kGame:
		LoadDebugSettings();

		gameScene->Update();
		break;
	default:
		break;
	}
}
#pragma endregion

#pragma region ゲームシーンの描画
void DrawScene() {
	switch (scene) {
	case Scene::kTitle:
		titleScene->Draw();
		break;
	case Scene::kGame:
		gameScene->Draw();
		break;
	default:
		break;
	}
}
#pragma endregion