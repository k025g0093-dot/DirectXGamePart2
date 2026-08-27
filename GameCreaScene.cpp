#include "GameCreaScene.h"
#include "MyMath.h"

#include <cassert>

using namespace KamataEngine;

void GameCreaScene::Initialize() {

	fade_ = new Fade;
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn, 1.0f);

	camera_.Initialize();

	// 結果表示は3Dモデルをやめて画像にする
	resultTexture_ = TextureManager::Load("GAMECLEAR.png");
	resultSprite_ = Sprite::Create(resultTexture_, {0, 0});
	resultSprite_->SetPosition(Vector2(0.0f, 0.0f));
	resultSprite_->SetSize(Vector2(1280.0f, 720.0f));

	// 「BACK TO TITLE」スプライト
	pressStartTexture_ = TextureManager::Load("backTITLEUI.png");
	pressStartSprite_ = Sprite::Create(pressStartTexture_, {0, 0});
	pressStartSprite_->SetPosition(Vector2(0.0f, 0.0f));
	pressStartSprite_->SetSize(Vector2(1280.0f, 720.0f));
	pressStartSprite_->SetColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));

	// カメラは背景（天球と地面）を映すためだけに使う
	camera_.translation_ = {0.0f, 0.0f, 0.0f};
	camera_.rotation_ = {0.18f, 0.0f, 0.0f};
	camera_.UpdateMatrix();

	// 天球と地面
	skyDomeModel_ = Model::CreateFromOBJ("skydome", true);
	planeModel_ = Model::CreateFromOBJ("ground", true);
	skyDome_ = new SkyDome();
	skyDome_->Initialize(skyDomeModel_);
	plane_ = new Plane();
	plane_->Initialize(planeModel_);
}

void GameCreaScene::Update() {

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

	// 背景をゆっくり回して、止まった絵に見えないようにする
	skyRotation_ += 0.0015f;
	camera_.rotation_.y = skyRotation_;
	camera_.UpdateMatrix();

	skyDome_->Update();
	plane_->Update();

	// 「PRESS START」点滅演出
	if (gamePhase_ == GamePhase::kPlay && showPressStart_) {
		blinkTimer_ += 0.05f;
		float alpha = (sinf(blinkTimer_) * 0.5f + 0.5f);
		pressStartSprite_->SetColor(Vector4(1.0f, 1.0f, 1.0f, alpha));
	}
}

void GameCreaScene::Draw() {

	// 3Dは背景（天球と地面）だけ
	Model::PreDraw();
	plane_->Draw(&camera_);
	skyDome_->Draw(&camera_);
	Model::PostDraw();

	// 2Dスプライト描画
	Sprite::PreDraw();
	resultSprite_->Draw();
	if (showPressStart_) {
		pressStartSprite_->Draw();
	}
	Sprite::PostDraw();

	fade_->Draw();
}

GameCreaScene::~GameCreaScene() {
	delete fade_;
	delete resultSprite_;
	delete pressStartSprite_;
	delete skyDome_;
	delete plane_;
	delete skyDomeModel_;
	delete planeModel_;
}
