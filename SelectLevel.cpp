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

	// 難易度説明のUI画像
	easyTexture_ = TextureManager::Load("EZUI.png");
	normalTexture_ = TextureManager::Load("normalUI.png");
	hardTexture_ = TextureManager::Load("hardUi.png");
	tutorialDescTexture_ = TextureManager::Load("tutorialUI.png");
	tutorialTexture_ = TextureManager::Load("tutorial.png");

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

	// 難易度説明パネルの背景（画像が無いチュートリアル用にだけ使う）
	descBg_ = Sprite::Create(whiteTexture_, {0, 0});
	descBg_->SetSize(Vector2(700, 80));
	descBg_->SetPosition(Vector2(290.0f, 340.0f));
	descBg_->SetColor(Vector4(0.1f, 0.1f, 0.1f, 0.8f));

	// 難易度の説明は1280x720の全画面オーバーレイ画像をそのまま貼る
	descTextEasy_ = Sprite::Create(easyTexture_, {0, 0});
	descTextEasy_->SetSize(Vector2(1280, 720));
	descTextEasy_->SetPosition(Vector2(0.0f, 0.0f));

	descTextNormal_ = Sprite::Create(normalTexture_, {0, 0});
	descTextNormal_->SetSize(Vector2(1280, 720));
	descTextNormal_->SetPosition(Vector2(0.0f, 0.0f));

	descTextHard_ = Sprite::Create(hardTexture_, {0, 0});
	descTextHard_->SetSize(Vector2(1280, 720));
	// ハードの画像だけ文字が左に寄っているので、少し右にずらして他と揃える
	descTextHard_->SetPosition(Vector2(40.0f, 0.0f));

	// チュートリアルはまだ画像が無いので、これまで通り白い箱で表示
	// チュートリアルの説明も他の難易度と同じく全画面の画像
	descTextTutorial_ = Sprite::Create(tutorialDescTexture_, {0, 0});
	descTextTutorial_->SetSize(Vector2(1280, 720));
	descTextTutorial_->SetPosition(Vector2(0.0f, 0.0f));

	// チュートリアルパネル（全面オーバーレイ）
	tutorialBg_ = Sprite::Create(whiteTexture_, {0, 0});
	tutorialBg_->SetSize(Vector2(1280, 720));
	tutorialBg_->SetPosition(Vector2(0, 0));
	tutorialBg_->SetColor(Vector4(0.05f, 0.05f, 0.15f, 0.95f)); // 暗い青背景

	// 説明の中身は1枚の画像。タイトルも本文も戻る案内も全部この中に入っている
	tutorialImage_ = Sprite::Create(tutorialTexture_, {0, 0});
	tutorialImage_->SetSize(Vector2(1280, 720));
	tutorialImage_->SetPosition(Vector2(0.0f, 0.0f));

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

	// 難易度説明は Draw() で選択中のものだけ描くので、ここでは位置をいじらない
	// （Easy/Normal/Hard は全画面画像、Tutorial だけ白い箱）

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

	// 難易度説明（チュートリアルオーバーレイが開いていない時に表示）
	if (!showTutorial_) {
		switch (selectIndex_) {
		// 4つとも画像に帯まで描かれているので descBg_ は重ねない
		case 0: descTextTutorial_->Draw(); break;
		case 1: descTextEasy_->Draw(); break;
		case 2: descTextNormal_->Draw(); break;
		case 3: descTextHard_->Draw(); break;
		}
	}

	// チュートリアルパネル
	if (showTutorial_) {
		// 先に背景を暗くしてから、説明画像を重ねる
		tutorialBg_->Draw();
		tutorialImage_->Draw();
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
	delete tutorialImage_;
}
