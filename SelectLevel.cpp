#include "MyMath.h"
#include "SelectLevel.h"
#include <cassert>

using namespace KamataEngine;

void SelectLevel::Initialize() {

	fade_ = new Fade;
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn, 1.0f);

	whiteTexture_ = TextureManager::Load("white1x1.png");
	sampleTexture_ = TextureManager::Load("startUI.png");

#pragma region 背景の3D（天球と地面）

	camera_.Initialize();
	// 天球は原点にあるので、その内側にカメラを置く
	camera_.translation_ = {0.0f, 0.0f, 0.0f};
	// 少しだけ下を向けて、地面が画面下側に見えるようにする
	camera_.rotation_ = {0.18f, 0.0f, 0.0f};
	camera_.UpdateMatrix();

	skyDomeModel_ = Model::CreateFromOBJ("skydome", true);
	groundModel_ = Model::CreateFromOBJ("ground", true);

	skyDome_ = new SkyDome();
	skyDome_->Initialize(skyDomeModel_);

	plane_ = new Plane();
	plane_->Initialize(groundModel_);

#pragma endregion

	// タイトル画像
	titleSprite_ = Sprite::Create(sampleTexture_, {0, 0});
	titleSprite_->SetSize(Vector2(1280, 720));
	titleSprite_->SetPosition(Vector2(0.0f, 0.0f));

	// 難易度ボタン（4つ並べる：Easy, Normal, Hard, Tutorial）中央寄せ
	float baseX = 280.0f;
	float baseY = 260.0f;
	float spacing = 180.0f;

	btnTutorial_ = Sprite::Create(whiteTexture_, {0, 0});
	btnTutorial_->SetSize(Vector2(150, 60));
	btnTutorial_->SetPosition(Vector2(baseX, baseY));
	btnTutorial_->SetColor(Vector4(0.6f, 0.6f, 0.2f, 1.0f)); // 黄

	btnEasy_ = Sprite::Create(whiteTexture_, {0, 0});
	btnEasy_->SetSize(Vector2(150, 60));
	btnEasy_->SetPosition(Vector2(baseX + spacing, baseY));
	btnEasy_->SetColor(Vector4(0.3f, 0.7f, 0.3f, 1.0f)); // 緑

	btnNormal_ = Sprite::Create(whiteTexture_, {0, 0});
	btnNormal_->SetSize(Vector2(150, 60));
	btnNormal_->SetPosition(Vector2(baseX + spacing * 2, baseY));
	btnNormal_->SetColor(Vector4(0.3f, 0.3f, 0.8f, 1.0f)); // 青

	btnHard_ = Sprite::Create(whiteTexture_, {0, 0});
	btnHard_->SetSize(Vector2(150, 60));
	btnHard_->SetPosition(Vector2(baseX + spacing * 3, baseY));
	btnHard_->SetColor(Vector4(0.8f, 0.2f, 0.2f, 1.0f)); // 赤

	// 選択カーソル
	cursor_ = Sprite::Create(whiteTexture_, {0, 0});
	cursor_->SetSize(Vector2(160, 70));
	cursor_->SetColor(Vector4(1, 1, 1, 0.5f));

	// 難易度説明パネル（ボタンの下に表示）
	descBg_ = Sprite::Create(whiteTexture_, {0, 0});
	descBg_->SetSize(Vector2(700, 80));
	descBg_->SetPosition(Vector2(290.0f, 340.0f));
	descBg_->SetColor(Vector4(0.1f, 0.1f, 0.1f, 0.8f));

	descTextEasy_ = Sprite::Create(whiteTexture_, {0, 0});
	descTextEasy_->SetSize(Vector2(680, 60));
	descTextEasy_->SetPosition(Vector2(300, 350));
	descTextEasy_->SetColor(Vector4(0.3f, 0.7f, 0.3f, 1.0f)); // 緑テキスト背景

	descTextNormal_ = Sprite::Create(whiteTexture_, {0, 0});
	descTextNormal_->SetSize(Vector2(680, 60));
	descTextNormal_->SetPosition(Vector2(300, 350));
	descTextNormal_->SetColor(Vector4(0.3f, 0.3f, 0.8f, 1.0f)); // 青テキスト背景

	descTextHard_ = Sprite::Create(whiteTexture_, {0, 0});
	descTextHard_->SetSize(Vector2(680, 60));
	descTextHard_->SetPosition(Vector2(300, 350));
	descTextHard_->SetColor(Vector4(0.8f, 0.2f, 0.2f, 1.0f)); // 赤テキスト背景

	descTextTutorial_ = Sprite::Create(whiteTexture_, {0, 0});
	descTextTutorial_->SetSize(Vector2(680, 60));
	descTextTutorial_->SetPosition(Vector2(300, 350));
	descTextTutorial_->SetColor(Vector4(1.0f, 0.8f, 0.2f, 1.0f)); // 金色テキスト背景

	// チュートリアルパネル（全面オーバーレイ）
	tutorialBg_ = Sprite::Create(whiteTexture_, {0, 0});
	tutorialBg_->SetSize(Vector2(1280, 720));
	tutorialBg_->SetPosition(Vector2(0, 0));
	tutorialBg_->SetColor(Vector4(0.05f, 0.05f, 0.15f, 0.95f)); // 暗い青背景

	tutorialTitle_ = Sprite::Create(whiteTexture_, {0, 0});
	tutorialTitle_->SetSize(Vector2(500, 50));
	tutorialTitle_->SetPosition(Vector2(390, 40));
	tutorialTitle_->SetColor(Vector4(1.0f, 0.8f, 0.2f, 1.0f)); // 金色

	tutorialText1_ = Sprite::Create(whiteTexture_, {0, 0});
	tutorialText1_->SetSize(Vector2(600, 30));
	tutorialText1_->SetPosition(Vector2(340, 130));
	tutorialText1_->SetColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f)); // 白

	tutorialText2_ = Sprite::Create(whiteTexture_, {0, 0});
	tutorialText2_->SetSize(Vector2(600, 30));
	tutorialText2_->SetPosition(Vector2(340, 180));
	tutorialText2_->SetColor(Vector4(0.8f, 0.8f, 1.0f, 1.0f)); // 水色

	tutorialText3_ = Sprite::Create(whiteTexture_, {0, 0});
	tutorialText3_->SetSize(Vector2(600, 30));
	tutorialText3_->SetPosition(Vector2(340, 230));
	tutorialText3_->SetColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f)); // 白

	tutorialText4_ = Sprite::Create(whiteTexture_, {0, 0});
	tutorialText4_->SetSize(Vector2(600, 30));
	tutorialText4_->SetPosition(Vector2(340, 280));
	tutorialText4_->SetColor(Vector4(0.8f, 1.0f, 0.8f, 1.0f)); // 薄緑

	tutorialText5_ = Sprite::Create(whiteTexture_, {0, 0});
	tutorialText5_->SetSize(Vector2(600, 30));
	tutorialText5_->SetPosition(Vector2(340, 330));
	tutorialText5_->SetColor(Vector4(1.0f, 0.8f, 0.8f, 1.0f)); // 薄赤

	tutorialBack_ = Sprite::Create(whiteTexture_, {0, 0});
	tutorialBack_->SetSize(Vector2(400, 40));
	tutorialBack_->SetPosition(Vector2(440, 650));
	tutorialBack_->SetColor(Vector4(0.5f, 0.5f, 0.5f, 1.0f)); // 灰色

	// 「PRESS START」スプライト
	pressStartSprite_ = Sprite::Create(whiteTexture_, {0, 0});
	pressStartSprite_->SetPosition(Vector2(440.0f, 500.0f));
	pressStartSprite_->SetSize(Vector2(400.0f, 40.0f));
	pressStartSprite_->SetColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
}

void SelectLevel::Update() {

	fade_->Update();

#pragma region 背景の3Dを更新

	// カメラをゆっくり回して、背景が流れているように見せる
	skyRotation_ += 0.0015f;
	camera_.rotation_.y = skyRotation_;
	camera_.UpdateMatrix();

	skyDome_->Update();
	plane_->Update();

#pragma endregion

	switch (gamePhase_) {
	case GamePhase::kFadeIn:
		if (fade_->IsFinished()) {
			gamePhase_ = GamePhase::kPlay;
		}
		break;

	case GamePhase::kPlay: {

		// チュートリアル表示中
		if (showTutorial_) {
			// Bボタン or BackSpace でチュートリアルを閉じる
			bool back = Input::GetInstance()->TriggerKey(DIK_BACKSPACE);
			XINPUT_STATE joyState{};
			if (Input::GetInstance()->GetJoystickState(0, joyState)) {
				if ((joyState.Gamepad.wButtons & XINPUT_GAMEPAD_B) && !prevBack_) {
					back = true;
				}
				prevBack_ = (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_B) != 0;
			}
			if (back) {
				showTutorial_ = false;
			}
			break;
		}

		// キーボード: A/D で左右移動
		if (Input::GetInstance()->TriggerKey(DIK_A)) {
			if (selectIndex_ > 0) selectIndex_--;
		}
		if (Input::GetInstance()->TriggerKey(DIK_D)) {
			if (selectIndex_ < 3) selectIndex_++;
		}

		// コントローラー: D-pad or 左スティック左右で選択（押した瞬間だけ反応）
		XINPUT_STATE joyState{};
		bool curLeft = false, curRight = false, curConfirm = false;
		if (Input::GetInstance()->GetJoystickState(0, joyState)) {
			curLeft = (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) != 0;
			curRight = (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) != 0;
			curConfirm = (joyState.Gamepad.wButtons & (XINPUT_GAMEPAD_A | XINPUT_GAMEPAD_START)) != 0;

			// 左スティックの横軸（デッドゾーン8000以上で反応）
			if (joyState.Gamepad.sThumbLX < -8000) {
				curLeft = true;
			} else if (joyState.Gamepad.sThumbLX > 8000) {
				curRight = true;
			}
		}
		if (curLeft && !prevDpadLeft_) {
			if (selectIndex_ > 0) selectIndex_--;
		}
		if (curRight && !prevDpadRight_) {
			if (selectIndex_ < 3) selectIndex_++;
		}
		prevDpadLeft_ = curLeft;
		prevDpadRight_ = curRight;

		// 決定（スペース or Aボタンの押した瞬間）
		bool confirm = Input::GetInstance()->TriggerKey(DIK_SPACE);
		if (curConfirm && !prevConfirm_) {
			confirm = true;
		}
		prevConfirm_ = curConfirm;

		if (confirm) {
			if (selectIndex_ == 0) {
				// チュートリアルを開く
				showTutorial_ = true;
			} else {
				switch (selectIndex_) {
				case 1: selectedDifficulty_ = DifficultyLevel::kEasy; break;
				case 2: selectedDifficulty_ = DifficultyLevel::kNormal; break;
				case 3: selectedDifficulty_ = DifficultyLevel::kHard; break;
				}
				fade_->Start(Fade::Status::FadeOut, 1.0f);
				gamePhase_ = GamePhase::kFadeOut;
			}
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

	// カーソル位置を選択中のボタンに合わせる
	float baseX = 280.0f;
	float spacing = 180.0f;
	cursor_->SetPosition(Vector2(baseX + selectIndex_ * spacing - 5, 255));

	// 難易度説明の表示切替
	descTextEasy_->SetPosition(Vector2(300, 350));
	descTextNormal_->SetPosition(Vector2(300, 350));
	descTextHard_->SetPosition(Vector2(300, 350));
	descTextTutorial_->SetPosition(Vector2(300, 350));

	// 「PRESS START」点滅演出
	if (gamePhase_ == GamePhase::kPlay && !showTutorial_) {
		blinkTimer_ += 0.05f;
		float alpha = (sinf(blinkTimer_) * 0.5f + 0.5f);
		pressStartSprite_->SetColor(Vector4(1.0f, 1.0f, 1.0f, alpha));
	}
}

void SelectLevel::Draw() {

	// ===== 先に3Dの背景を描く（スプライトより奥に来る）=====
	Model::PreDraw();

	plane_->Draw(&camera_);
	skyDome_->Draw(&camera_);

	Model::PostDraw();

	// ===== その上にUIを重ねる =====
	Sprite::PreDraw();

	titleSprite_->Draw();
	btnEasy_->Draw();
	btnNormal_->Draw();
	btnHard_->Draw();
	btnTutorial_->Draw();
	cursor_->Draw();

	// 難易度説明パネル（チュートリアルオーバーレイが開いていない時に表示）
	if (!showTutorial_) {
		descBg_->Draw();
		switch (selectIndex_) {
		case 0: descTextTutorial_->Draw(); break;
		case 1: descTextEasy_->Draw(); break;
		case 2: descTextNormal_->Draw(); break;
		case 3: descTextHard_->Draw(); break;
		}
	}

	// チュートリアルパネル
	if (showTutorial_) {
		tutorialBg_->Draw();
		tutorialTitle_->Draw();
		tutorialText1_->Draw();
		tutorialText2_->Draw();
		tutorialText3_->Draw();
		tutorialText4_->Draw();
		tutorialText5_->Draw();
		tutorialBack_->Draw();
	} else {
		pressStartSprite_->Draw();
	}

	Sprite::PostDraw();

	fade_->Draw();
}

SelectLevel::~SelectLevel() {
	delete fade_;

	// 背景の3D
	delete skyDome_;
	delete plane_;
	delete skyDomeModel_;
	delete groundModel_;

	delete titleSprite_;
	delete btnEasy_;
	delete btnNormal_;
	delete btnHard_;
	delete btnTutorial_;
	delete cursor_;
	delete pressStartSprite_;
	delete descBg_;
	delete descTextEasy_;
	delete descTextNormal_;
	delete descTextHard_;
	delete descTextTutorial_;
	delete tutorialBg_;
	delete tutorialTitle_;
	delete tutorialText1_;
	delete tutorialText2_;
	delete tutorialText3_;
	delete tutorialText4_;
	delete tutorialText5_;
	delete tutorialBack_;
}
