#include "GameScene.h"
#include "KamataEngine.h"
#include "TitleScene.h"
#include <Windows.h>

using namespace KamataEngine;

GameScene* gameScene = nullptr;
TitleScene* titleScene = nullptr;

enum class Scene {
	kUnknown = 0,
	kTitle,
	kGame,
};

Scene scene = Scene::kTitle;

void ChangeScene();
void UpdateScene();
void DrawScene();

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	//============================
	// 初期化
	//============================

	KamataEngine::Initialize(L"AE2?_99_ヤマト_ユウヤ_AL2");
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// ImGuiManagerインスタンスの取得を追加
	ImGuiManager* imguiManager = ImGuiManager::GetInstance();

	scene = Scene::kTitle;

	titleScene = new TitleScene();
	titleScene->Initialize();

	//============================
	// 更新処理
	//============================

	while (true) {

		if (KamataEngine::Update()) {
			break;
		}

		// ImGui受付開始を追加
		imguiManager->Begin();

		//======================================================================
		// 更新処理ここから
		//======================================================================

		ChangeScene();
		UpdateScene(); // この中でImGui::Begin("Debug Menu")等が呼ばれてもOKになる

		//======================================================================
		// 更新処理ここまで
		//======================================================================

		// ImGui受付終了を追加
		imguiManager->End();

		//======================================================================
		// 描画処理ここから
		//======================================================================

		dxCommon->PreDraw();

		DrawScene();

		// ImGui描画を追加
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
	// 解放エクササイズ
	//---------------------------------------------------

	delete gameScene;
	delete titleScene;
	gameScene = nullptr;
	titleScene = nullptr;

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
			gameScene->Initialize();
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
			delete gameScene;
			gameScene = nullptr;
			gameScene = new GameScene;
			gameScene->Initialize();
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
		titleScene->Update();
		break;
	case Scene::kGame:
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