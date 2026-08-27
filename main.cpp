#include "GameCreaScene.h"
#include "GameOverScene.h"
#include "GameScene.h"
#include "KamataEngine.h"
#include "SelectLevel.h"
#include "TitleScene.h"
#include <Windows.h>

using namespace KamataEngine;
GameScene* gameScene = nullptr;
TitleScene* titleScene = nullptr;
SelectLevel* selectLevel = nullptr;

GameOverScene* gameOverScene = nullptr;
GameCreaScene* gameCreaScene = nullptr;

enum class Scene {
	kUnknown = 0,
	kTitle,
	kSelectLevel,
	kGame,
	kGameOver,
	kGameCrea,
};

Scene scene = Scene::kTitle;

//============================
// BGM
// タイトル・難易度選択・リザルトは TitleAndSelect、ゲーム中は GamePlaye。
// シーンをまたいで鳴らし続けたいので、シーン側ではなくここで持つ
//============================
namespace {
uint32_t bgmTitleData = 0;  // TitleAndSelect のサウンドデータ
uint32_t bgmGameData = 0;   // GamePlaye のサウンドデータ
uint32_t bgmClearData = 0;  // GameClear のサウンドデータ
uint32_t bgmOverData = 0;   // GameOver のサウンドデータ
uint32_t bgmVoice = 0;      // 今鳴っている再生ハンドル
uint32_t bgmCurrent = 0;    // 今鳴っているサウンドデータ（同じなら鳴らし直さない）
bool bgmStarted = false;    // 一度でも再生したか。ハンドルは0もあり得るのでフラグで持つ

const float kBgmVolume = 0.35f;

// BGMを切り替える。すでに同じ曲が鳴っていれば何もしない（頭出しに戻さない）
void PlayBgm(uint32_t soundData) {
	if (bgmStarted && bgmCurrent == soundData && Audio::GetInstance()->IsPlaying(bgmVoice)) {
		return;
	}
	if (bgmStarted) {
		Audio::GetInstance()->StopWave(bgmVoice);
	}
	bgmVoice = Audio::GetInstance()->PlayWave(soundData, true, kBgmVolume);
	bgmCurrent = soundData;
	bgmStarted = true;
}
} // namespace

void ChangeScene();
void UpdateScene();
void DrawScene();

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	//============================
	// 初期化
	//============================

	KamataEngine::Initialize(L"LE2B_29_ヤマト_ユウヤ_味方はぜんぶ、元は敵");
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	ImGuiManager* imguiManager = ImGuiManager::GetInstance();

	// BGMの読み込みと、タイトルBGMの再生開始
	bgmTitleData = Audio::GetInstance()->LoadWave("sound/TitleAndSelect.wav");
	bgmGameData = Audio::GetInstance()->LoadWave("sound/GamePlaye.wav");
	bgmClearData = Audio::GetInstance()->LoadWave("sound/GameClear.wav");
	bgmOverData = Audio::GetInstance()->LoadWave("sound/GameOver.wav");
	PlayBgm(bgmTitleData);

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

		//if (Input::GetInstance()->PushKey(DIK_ESCAPE)) {
		//	break;
		//}
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
			// タイトルと同じ曲なので、切り替えず流し続ける
			PlayBgm(bgmTitleData);
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
			// ゲーム中のBGMへ切り替え
			PlayBgm(bgmGameData);
		}
		break;

	case Scene::kGame:
		if (gameScene->IsFinished()) {
			if (gameScene->isGameClear_) {
				scene = Scene::kGameCrea;
				delete gameScene;
				gameScene = nullptr;
				gameCreaScene = new GameCreaScene;
				gameCreaScene->Initialize();
				PlayBgm(bgmClearData);
			} else {
				scene = Scene::kGameOver;
				delete gameScene;
				gameScene = nullptr;

				gameOverScene = new GameOverScene;
				gameOverScene->Initialize();
				PlayBgm(bgmOverData);
			}
		}
		break;

	case Scene::kGameOver:
		if (gameOverScene->IsFinished()) {
			scene = Scene::kTitle;
			delete gameOverScene;
			gameOverScene = nullptr;
			titleScene = new TitleScene;
			titleScene->Initialize();
		}

		break;

	case Scene::kGameCrea:
		if (gameCreaScene->IsFinished()) {
			scene = Scene::kTitle;
			delete gameCreaScene;
			gameCreaScene = nullptr;
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

	case Scene::kGameOver:
		gameOverScene->Update();
		break;

	case Scene::kGameCrea:
		gameCreaScene->Update();
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

	case Scene::kGameOver:
		gameOverScene->Draw();
		break;

	case Scene::kGameCrea:
		gameCreaScene->Draw();
		break;

	default:
		break;
	}
}
#pragma endregion
