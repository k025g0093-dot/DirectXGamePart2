#include "MyMath.h"
#include "SelectLevel.h"
#include <cassert>

using namespace KamataEngine;

void SelectLevel::Initialize() {

	fade_ = new Fade;
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn, 1.0f);

	whiteTexture_ = TextureManager::Load("white1x1.png");
	sampleTexture_ = TextureManager::Load("sample.png");

	// タイトル（sample.pngを仮で使用）
	titleSprite_ = Sprite::Create(sampleTexture_, {0, 0});
	titleSprite_->SetSize(Vector2(400, 100));
	titleSprite_->SetPosition(Vector2(440, 80));

	// 難易度ボタン（3つ並べる）
	float baseX = 340.0f;
	float baseY = 300.0f;
	float spacing = 200.0f;

	btnEasy_ = Sprite::Create(whiteTexture_, {0, 0});
	btnEasy_->SetSize(Vector2(160, 60));
	btnEasy_->SetPosition(Vector2(baseX, baseY));
	btnEasy_->SetColor(Vector4(0.3f, 0.7f, 0.3f, 1.0f)); // 緑

	btnNormal_ = Sprite::Create(whiteTexture_, {0, 0});
	btnNormal_->SetSize(Vector2(160, 60));
	btnNormal_->SetPosition(Vector2(baseX + spacing, baseY));
	btnNormal_->SetColor(Vector4(0.3f, 0.3f, 0.8f, 1.0f)); // 青

	btnHard_ = Sprite::Create(whiteTexture_, {0, 0});
	btnHard_->SetSize(Vector2(160, 60));
	btnHard_->SetPosition(Vector2(baseX + spacing * 2, baseY));
	btnHard_->SetColor(Vector4(0.8f, 0.2f, 0.2f, 1.0f)); // 赤

	// 選択カーソル
	cursor_ = Sprite::Create(whiteTexture_, {0, 0});
	cursor_->SetSize(Vector2(170, 70));
	cursor_->SetColor(Vector4(1, 1, 1, 0.5f));
}

void SelectLevel::Update() {

	fade_->Update();

	switch (gamePhase_) {
	case GamePhase::kFadeIn:
		if (fade_->IsFinished()) {
			gamePhase_ = GamePhase::kPlay;
		}
		break;

	case GamePhase::kPlay: {

		// キーボード: 1/2/3 で直接選択、A/D で左右移動
		if (Input::GetInstance()->TriggerKey(DIK_1)) {
			selectIndex_ = 0;
		} else if (Input::GetInstance()->TriggerKey(DIK_2)) {
			selectIndex_ = 1;
		} else if (Input::GetInstance()->TriggerKey(DIK_3)) {
			selectIndex_ = 2;
		}
		if (Input::GetInstance()->TriggerKey(DIK_A)) {
			if (selectIndex_ > 0) selectIndex_--;
		}
		if (Input::GetInstance()->TriggerKey(DIK_D)) {
			if (selectIndex_ < 2) selectIndex_++;
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
			if (selectIndex_ < 2) selectIndex_++;
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
			switch (selectIndex_) {
			case 0: selectedDifficulty_ = DifficultyLevel::kEasy; break;
			case 1: selectedDifficulty_ = DifficultyLevel::kNormal; break;
			case 2: selectedDifficulty_ = DifficultyLevel::kHard; break;
			}
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

	// カーソル位置を選択中的ボタンに合わせる
	float baseX = 340.0f;
	float spacing = 200.0f;
	cursor_->SetPosition(Vector2(baseX + selectIndex_ * spacing - 5, 295));
}

void SelectLevel::Draw() {

	Sprite::PreDraw();

	titleSprite_->Draw();
	btnEasy_->Draw();
	btnNormal_->Draw();
	btnHard_->Draw();
	cursor_->Draw();

	Sprite::PostDraw();

	fade_->Draw();
}

SelectLevel::~SelectLevel() {
	delete fade_;
	delete titleSprite_;
	delete btnEasy_;
	delete btnNormal_;
	delete btnHard_;
	delete cursor_;
}
