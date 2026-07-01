#include "KamataEngine.h"
#include "GameScene.h"
#include <Windows.h>

using namespace KamataEngine;
GameScene* gameScene = nullptr;







int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	//============================
	// 初期化
	//============================

	KamataEngine::Initialize(L"LE2B_29_ヤマト_ユウヤ_AL3_3Dレールシューティング");
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	ImGuiManager* imguiManager = ImGuiManager::GetInstance();

	gameScene = new GameScene();



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
		
		gameScene->Update();


		//======================================================================
		// 更新処理ここまで
		//======================================================================

		imguiManager->End();

		//======================================================================
		// 描画処理ここから
		//======================================================================

		dxCommon->PreDraw();

		gameScene->Draw();
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
