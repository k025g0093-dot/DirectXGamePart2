#include "GameScene.h"
#include "KamataEngine.h"
#include "SelectLevel.h"
#include "TitleScene.h"
#include <Windows.h>

using namespace KamataEngine;
GameScene* gameScene = nullptr;
TitleScene* titleScene = nullptr;
SelectLevel* selectLevel = nullptr;

enum class Scene {
	kUnknown = 0,
	kTitle,
	kSelectLevel,
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

	KamataEngine::Initialize(L"LE2B_29_ヤマト_ユウヤ_AL3_3Dレールシューティング");
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	ImGuiManager* imguiManager = ImGuiManager::GetInstance();

	gameScene = new GameScene();

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
		AxisIndicator::GetInstance()->Draw();

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
	gameScene = nullptr;

	KamataEngine::Finalize();
	return 0;
}

#pragma region ゲームのシーンの切り替え
void ChangeScene() {

	switch (scene) {
	case Scene::kTitle:
		if (titleScene->IsFinished()) {

			scene = Scene::kSelectLevel;
			delete titleScene;
			titleScene = nullptr;

			selectLevel = new SelectLevel;
			selectLevel->Initialize();

		}
		break;

	case Scene::kSelectLevel:
		if (selectLevel->IsFinished()) {
			scene = Scene::kGame;
			DifficultyLevel selected = selectLevel->GetSelectedDifficulty();
			delete selectLevel;
			selectLevel = nullptr;
			gameScene = new GameScene;
			gameScene->Initialize();
			gameScene->SetDifficultyLevel(selected);
		}
		break;

	case Scene::kGame:
		if (gameScene->IsFinished()) {
			scene = Scene::kTitle;
			delete gameScene;
			gameScene = nullptr;

			titleScene = new TitleScene;
			titleScene->Initialize();
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
	case Scene::kSelectLevel:

		selectLevel->Update();
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

	case Scene::kSelectLevel:
		selectLevel->Draw();
		break;

	case Scene::kGame:
		gameScene->Draw();
		break;
	default:
		break;
	}
}
#pragma endregion
