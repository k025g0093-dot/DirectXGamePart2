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

	// 1. キャラを正面に向ける設定
	worldTransform_.scale_ = {0.8f, 0.8f, 0.8f};    // 少し大きく設定
	worldTransform_.rotation_ = {0.0f, 0.0f, 0.0f}; // 向きが逆なら 3.1415f (180度) に
	worldTransform_.translation_ = {0.0f, 0.0f, 0.0f};

	// 2. カメラの初期位置（キャラの正面に配置）
	// zをマイナスにすると手前になります
	camera_.translation_ = {0.0f, 0.5f, -15.0f};
}

void TitleScene::Update() {

	fade_->Update();
	
	switch (phase_) {
	case Phese::kFadeIn:
		// フェードインが完全に終わるのを待つ
		if (fade_->IsFinished()) {
			phase_ = Phese::kMain;
		}
		break;

	case Phese::kMain:
		// メイン待機中：スペースが押されたらフェードアウト開始
		if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
			fade_->Start(Fade::Status::FadeOut, 1.0f);
			phase_ = Phese::kFadeOut; // 次の状態へ
		}
		break;

	case Phese::kFadeOut:
		// フェードアウト中：フェードが完全に終わったら、ようやく終了フラグを立てる
		if (fade_->IsFinished()) {
			finished_ = true; // ここで初めて main.cpp がシーンを切り替えてくれる
		}
		break;
	default:
		break;
	}

	// --- キャラの行列更新 ---
	worldTransform_.rotation_.y -= 0.1f;
	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	worldTransform_.TransferMatrix();

	// --- カメラの行列更新 ---
	// 座標などを変えた後は、UpdateMatrix を呼ばないと画面に反映されません
	camera_.UpdateMatrix();
}

void TitleScene::Draw() {

	Model::PreDraw();
	// 2. 描画（引数は 3つ）
	if (modelTitle_) {
		modelTitle_->Draw(worldTransform_, camera_, &objectColor_);
	}
	fade_->Draw();

	// 3. 3Dモデル描画後の終了処理
	Model::PostDraw();
}

TitleScene::~TitleScene() {
	delete fade_;
}
