#include "TitleScene.h"
#include "MyMath.h"

#include <cassert>

using namespace KamataEngine;

void TitleScene::Initialize() {

	fade_ = new Fade;
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn, 1.0f);

	// タイトルは3Dモデルをやめて画像で表示する

	// ゲームタイトルのロゴ
	titleLogoTexture_ = TextureManager::Load("TITLE.png");
	titleLogoSprite_ = Sprite::Create(titleLogoTexture_, {0, 0});
	titleLogoSprite_->SetPosition(Vector2(0.0f, 0.0f));
	titleLogoSprite_->SetSize(Vector2(1280.0f, 720.0f));

	// 「Ⓐ/SPACE start」の案内
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

	// 背景用のカメラ。天球の内側に置いて、少し下を向ける
	bgCamera_.Initialize();
	bgCamera_.translation_ = {0.0f, 0.0f, 0.0f};
	bgCamera_.rotation_ = {0.18f, 0.0f, 0.0f};
	bgCamera_.UpdateMatrix();
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

	// 背景カメラをゆっくり回して、天球が流れているように見せる
	skyRotation_ += 0.0015f;
	bgCamera_.rotation_.y = skyRotation_;
	bgCamera_.UpdateMatrix();

	skyDome_->Update();
	plane_->Update();
}

void TitleScene::Draw() {

	// 3Dは回転する背景（天球と地面）だけ
	Model::PreDraw();
	plane_->Draw(&bgCamera_);
	skyDome_->Draw(&bgCamera_);
	Model::PostDraw();

	Sprite::PreDraw();
	// タイトルロゴ → 操作案内の順に重ねる
	titleLogoSprite_->Draw();
	titleImageSprite_->Draw();
	Sprite::PostDraw();

	fade_->Draw();
}

TitleScene::~TitleScene() {
	delete fade_;
	delete titleLogoSprite_;
	delete titleImageSprite_;
	delete skyDome_;
	delete plane_;
	delete skyDomeModel_;
	delete planeModel_;
}
