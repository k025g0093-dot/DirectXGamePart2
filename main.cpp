#include "GameScene.h"
#include "KamataEngine.h"
#include "TitleScene.h"
#include <Windows.h>

// グローバルな名前を持つ蒲田エンジンを作成
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
// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	//============================
	// 初期化
	//============================

	// 蒲田エンジンのロード
	KamataEngine::Initialize(L"AE2?_99_ヤマト_ユウヤ_AL2");
	// DirectXのロード
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	scene = Scene::kTitle;

	titleScene = new TitleScene();
	titleScene->Initialize();
	// ここから下のクラスのインスタンスなどの情報を追加

	// ここから下のクラスのインスタンスなどの情報を追加

	//============================
	// 更新処理
	//============================

	while (true) {

		// 蒲田エンジンのアップデート
		if (KamataEngine::Update()) {
			break;
		}

		//======================================================================
		// 更新処理ここから
		//======================================================================

		ChangeScene();
		UpdateScene();

		//======================================================================
		// 更新処理ここまで
		//======================================================================

		//======================================================================
		// 描画処理ここから
		//======================================================================

		// DirectXの描画処理を追加
		dxCommon->PreDraw(); // ここから開始して

		DrawScene();

		dxCommon->PostDraw(); // ここで終了

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

	// ゲームシーンの解放

	delete gameScene;
	delete titleScene;
	// nullptrの代入
	gameScene = nullptr;
	titleScene = nullptr;

	// ゲームの終了
	KamataEngine::Finalize();
	return 0;
}

#pragma region	ゲームのシーンの切り替え
void ChangeScene() {

	switch (scene) {
	case Scene::kTitle:

		if (titleScene->IsFinished()) {
			// シーン交換
			scene = Scene::kGame;
			delete titleScene;
			titleScene = nullptr;

			// 新しいシーン生成
			gameScene = new GameScene;
			gameScene->Initialize();
		}
		break;
	case Scene::kGame:
		if (gameScene->IsFinished()) {
			// シーン交換
			scene = Scene::kTitle;
			delete gameScene;
			gameScene = nullptr;

			// 新しいシーン生成
			titleScene = new TitleScene;
			titleScene->Initialize();
		}
		break;
	default:
		break;
	}
}
#pragma endregion
#pragma region	ゲームの更新処理
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