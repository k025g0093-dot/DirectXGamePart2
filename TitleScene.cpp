#include "TitleScene.h"
#include "MyMath.h"

#include <cassert>

using namespace KamataEngine;

void TitleScene::Initialize() {

	fade_ = new Fade;
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn, 1.0f);

	modelTitle_ = Model::CreateFromOBJ("titleFont", true);
	assert(modelTitle_);

	worldTransform_.Initialize();
	camera_.Initialize();
	objectColor_.Initialize();

	// タイトル画像
	titleImageTexture_ = TextureManager::Load("startUI.png");
	titleImageSprite_ = Sprite::Create(titleImageTexture_, {0, 0});
	titleImageSprite_->SetPosition(Vector2(0.0f, 0.0f));
	titleImageSprite_->SetSize(Vector2(1280.0f, 720.0f));

	// 天球と地面
	skyDomeModel_ = Model::CreateFromOBJ("skydome", true);
	planeModel_ = Model::CreateFromOBJ("ground", true);
	skyDome_ = new SkyDome();
	skyDome_->Initialize(skyDomeModel_);
	plane_ = new Plane();
	plane_->Initialize(planeModel_);
}

void TitleScene::Update() {

	fade_->Update();
	
	switch (gamePhase_) {
	case GamePhase::kFadeIn:
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
			fade_->Start(Fade::Status::FadeOut, 1.0f);
			gamePhase_ = GamePhase::kFadeOut;
		}
		break;
	}

	case GamePhase::kFadeOut:
		if (fade_->IsFinished()) {
			finished_ = true;
		}
		break;
	default:
		break;
	}

	// キャラの行列更新
	worldTransform_.rotation_.y -= 0.1f;
	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	worldTransform_.TransferMatrix();

	camera_.UpdateMatrix();

	skyDome_->Update();
	plane_->Update();
}

void TitleScene::Draw() {

	Model::PreDraw();
	plane_->Draw(&camera_);
	skyDome_->Draw(&camera_);
	if (modelTitle_) {
		modelTitle_->Draw(worldTransform_, camera_, &objectColor_);
	}
	Model::PostDraw();

	Sprite::PreDraw();
	titleImageSprite_->Draw();
	Sprite::PostDraw();

	fade_->Draw();
}

TitleScene::~TitleScene() {
	delete fade_;
	delete titleImageSprite_;
	delete skyDome_;
	delete plane_;
	delete skyDomeModel_;
	delete planeModel_;
}
