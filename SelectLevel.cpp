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
	selectLevelTexture_ = TextureManager::Load("selectLevelUI.png");
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

	// 難易度の並び（チュートリアル / イージー / ノーマル / ハード）は画像1枚で描く
	selectLevelSprite_ = Sprite::Create(selectLevelTexture_, {0, 0});
	selectLevelSprite_->SetSize(Vector2(1280, 720));
	selectLevelSprite_->SetPosition(Vector2(0.0f, 0.0f));

	// 選択カーソル
	// 選択中を示す帯
	cursor_ = Sprite::Create(whiteTexture_, {0, 0});
	cursor_->SetColor(Vector4(1.0f, 1.0f, 1.0f, 0.35f));

	// 選択中の文字の下に引く明るい線
	cursorBar_ = Sprite::Create(whiteTexture_, {0, 0});
	cursorBar_->SetColor(Vector4(1, 1, 1, 1.0f));

	// 選択中の文字を左右から挟むマーカー（黒）
	markerL_ = Sprite::Create(whiteTexture_, {0, 0});
	markerL_->SetSize(Vector2(10, 60));
	markerL_->SetColor(Vector4(0, 0, 0, 1.0f));

	markerR_ = Sprite::Create(whiteTexture_, {0, 0});
	markerR_->SetSize(Vector2(10, 60));
	markerR_->SetColor(Vector4(0, 0, 0, 1.0f));

	// 難易度説明パネルの背景（画像が無いチュートリアル用にだけ使う）
	descBg_ = Sprite::Create(whiteTexture_, {0, 0});
	descBg_->SetSize(Vector2(700, 80));
	descBg_->SetPosition(Vector2(290.0f, 340.0f));
	descBg_->SetColor(Vector4(0.1f, 0.1f, 0.1f, 0.8f));

	// 難易度の説明は1280x720の全画面オーバーレイ画像をそのまま貼る
	// 難易度説明は難易度名の行と重なっていたので、まとめて下へずらす
	// 数字を変えれば4つとも一緒に動く
	const float kDescOffsetY = 70.0f;
	// チュートリアルの説明画像だけ、文字が他の3枚より32px下に描かれている。
	// その分を引いて、4つとも同じ高さに文字が並ぶようにする
	const float kDescOffsetYTutorial = kDescOffsetY - 32.0f;

	descTextEasy_ = Sprite::Create(easyTexture_, {0, 0});
	descTextEasy_->SetSize(Vector2(1280, 720));
	descTextEasy_->SetPosition(Vector2(0.0f, kDescOffsetY));

	descTextNormal_ = Sprite::Create(normalTexture_, {0, 0});
	descTextNormal_->SetSize(Vector2(1280, 720));
	descTextNormal_->SetPosition(Vector2(0.0f, kDescOffsetY));

	descTextHard_ = Sprite::Create(hardTexture_, {0, 0});
	descTextHard_->SetSize(Vector2(1280, 720));
	// ハードの画像だけ文字が左に寄っているので、少し右にずらして他と揃える
	descTextHard_->SetPosition(Vector2(40.0f, kDescOffsetY));

	// チュートリアルはまだ画像が無いので、これまで通り白い箱で表示
	// チュートリアルの説明も他の難易度と同じく全画面の画像
	descTextTutorial_ = Sprite::Create(tutorialDescTexture_, {0, 0});
	descTextTutorial_->SetSize(Vector2(1280, 720));
	descTextTutorial_->SetPosition(Vector2(0.0f, kDescOffsetYTutorial));

	// チュートリアルパネル（全面オーバーレイ）
	tutorialBg_ = Sprite::Create(whiteTexture_, {0, 0});
	tutorialBg_->SetSize(Vector2(1280, 720));
	tutorialBg_->SetPosition(Vector2(0, 0));
	tutorialBg_->SetColor(Vector4(0.05f, 0.05f, 0.15f, 0.95f)); // 暗い青背景

	// 説明の中身は1枚の画像。タイトルも本文も戻る案内も全部この中に入っている
	tutorialImage_ = Sprite::Create(tutorialTexture_, {0, 0});
	tutorialImage_->SetSize(Vector2(1280, 720));
	tutorialImage_->SetPosition(Vector2(0.0f, 0.0f));

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

	// カーソルを選択中の文字に合わせる
	// 画像内の文字幅がバラバラなので、項目ごとに位置と幅を持たせている
	static const float kCursorX[4] = {122.0f, 499.0f, 747.0f, 989.0f};
	static const float kCursorW[4] = {347.0f, 218.0f, 223.0f, 186.0f};
	int32_t ci = selectIndex_;
	if (ci < 0) ci = 0;
	if (ci > 3) ci = 3;
	float cx = kCursorX[ci];
	float cw = kCursorW[ci];

	cursor_->SetPosition(Vector2(cx, 278.0f));
	cursor_->SetSize(Vector2(cw, 80.0f));

	// 下線は文字のすぐ下に
	cursorBar_->SetPosition(Vector2(cx, 358.0f));
	cursorBar_->SetSize(Vector2(cw, 6.0f));

	// マーカーは帯の外側に置く
	markerL_->SetPosition(Vector2(cx - 24.0f, 288.0f));
	markerR_->SetPosition(Vector2(cx + cw + 14.0f, 288.0f));

	// ゆっくり明滅させて「ここを選んでいる」を強調する
	selectPulse_ += 0.08f;
	float pulse = 0.5f + 0.5f * sinf(selectPulse_);
	cursor_->SetColor(Vector4(1.0f, 1.0f, 1.0f, 0.25f + 0.20f * pulse));
	float markAlpha = 0.65f + 0.35f * pulse;
	cursorBar_->SetColor(Vector4(1, 1, 1, markAlpha));
	markerL_->SetColor(Vector4(0, 0, 0, markAlpha));
	markerR_->SetColor(Vector4(0, 0, 0, markAlpha));

	// 難易度説明は Draw() で選択中のものだけ描くので、ここでは位置をいじらない
	// （Easy/Normal/Hard は全画面画像、Tutorial だけ白い箱）

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
	// 帯を敷く → 文字を重ねる → 下線とマーカーを一番上に出す
	cursor_->Draw();
	selectLevelSprite_->Draw();
	cursorBar_->Draw();
	markerL_->Draw();
	markerR_->Draw();

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
	delete selectLevelSprite_;
	delete cursor_;
	delete cursorBar_;
	delete markerL_;
	delete markerR_;
	delete descBg_;
	delete descTextEasy_;
	delete descTextNormal_;
	delete descTextHard_;
	delete descTextTutorial_;
	delete tutorialBg_;
	delete tutorialImage_;
}
