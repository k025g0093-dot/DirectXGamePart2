#include "GameOverScene.h"
#include "MyMath.h"

#include <cassert>

using namespace KamataEngine;

void GameOverScene::Initialize() {

	fade_ = new Fade;
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn, 1.0f);

	modelTitle_ = Model::CreateFromOBJ("titleFont", true);
	assert(modelTitle_);

	worldTransform_.Initialize();
	camera_.Initialize();
	objectColor_.Initialize();

	// 「PRESS START」スプライト
	pressStartTexture_ = TextureManager::Load("white1x1.png");
	pressStartSprite_ = Sprite::Create(pressStartTexture_, {0, 0});
	pressStartSprite_->SetPosition(Vector2(440.0f, 600.0f));
	pressStartSprite_->SetSize(Vector2(400.0f, 40.0f));
	pressStartSprite_->SetColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));

	// 1. キャラを正面に向ける設定
	worldTransform_.scale_ = {0.8f, 0.8f, 0.8f};    // 少し大きく設定
	worldTransform_.rotation_ = {0.0f, 0.0f, 0.0f}; // 向きが逆なら 3.1415f (180度) に
	worldTransform_.translation_ = {0.0f, 0.0f, 0.0f};

	// 2. カメラの初期位置（キャラの正面に配置）
	// zをマイナスにすると手前になります
	camera_.translation_ = {0.0f, 0.5f, -15.0f};
}

void GameOverScene::Update() {

	fade_->Update();
	
	switch (gamePhase_) {
	case GamePhase::kFadeIn:
		// フェードインが完全に終わるのを待つ
		if (fade_->IsFinished()) {
			gamePhase_ = GamePhase::kPlay;
		}
		break;

	case GamePhase::kPlay: {
		bool start = Input::GetInstance()->TriggerKey(DIK_SPACE);

		XINPUT_STATE joyState{};
		if (Input::GetInstance()->GetJoystickState(0, joyState)) {
			if (joyState.Gamepad.wButtons & (XINPUT_GAMEPAD_A | XINPUT_GAMEPAD_START)) {
				start = true;
			}
		}

		if (start) {
			showPressStart_ = false;
			fade_->Start(Fade::Status::FadeOut, 1.0f);
			gamePhase_ = GamePhase::kFadeOut;
		}
		break;
	}

	case GamePhase::kFadeOut:
		// フェードアウト中：フェードが完全に終わったら、ようやく終了フラグを立てる
		if (fade_->IsFinished()) {
			finished_ = true; // ここで初めて main.cpp がシーンを切り替えてくれる
		}
		break;
	default:
		break;
	}

	// --- キャラの行列更新 ---
	worldTransform_.rotation_.x += 0.1f;
	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	worldTransform_.TransferMatrix();

	// --- カメラの行列更新 ---
	// 座標などを変えた後は、UpdateMatrix を呼ばないと画面に反映されません
	camera_.UpdateMatrix();

	// 「PRESS START」点滅演出
	if (gamePhase_ == GamePhase::kPlay && showPressStart_) {
		blinkTimer_ += 0.05f;
		float alpha = (sinf(blinkTimer_) * 0.5f + 0.5f);
		pressStartSprite_->SetColor(Vector4(1.0f, 1.0f, 1.0f, alpha));
	}
}

void GameOverScene::Draw() {

	Model::PreDraw();
	// 2. 描画（引数は 3つ）
	if (modelTitle_) {
		modelTitle_->Draw(worldTransform_, camera_, &objectColor_);
	}
	fade_->Draw();

	// 3. 3Dモデル描画後の終了処理
	Model::PostDraw();

	// 2Dスプライト描画
	Sprite::PreDraw();
	if (showPressStart_) {
		pressStartSprite_->Draw();
	}
	Sprite::PostDraw();
}

GameOverScene::~GameOverScene() {
	delete fade_;
	delete pressStartSprite_;
}
