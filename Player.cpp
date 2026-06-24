#define NOMINMAX
#include "Player.h"
#include "MyMath.h"

#include <algorithm>
#include <cassert>
#include <numbers>

#ifdef _DEBUG
#include <imgui.h>
#endif

using namespace KamataEngine;
Player::Player() {}
bool isHit = false;
void Player::Initialize(
    KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position

) {

	worldTransform_.scale_ = {1.0f, 1.0f, 1.0f};
	worldTransform_.rotation_ = {0.0f, 90.0f, 0.0f};
	worldTransform_.translation_ = {0.0f, 0.0f, 0.0f};

	velocity_ = {0.0f, 0.0f, 0.0f};

	assert(model);
	model_ = model;

	//ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = std::numbers::pi_v<float> * 3.0f / 2.0f;
	camera_ = camera;

	//キー入力の初期化
	input_ = Input::GetInstance();
}

void Player::Updata() {

	MovePlayer();


	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	worldTransform_.TransferMatrix();
}

void Player::Draw() { model_->Draw(worldTransform_, *camera_); }

Player::~Player() {}

void Player::MovePlayer() {



	Vector3 move = {0, 0, 0};
	//移動速度
	const float kCharacteaSpeed = 0.2f;

	//移動制限座標
	const float kMoveLimitX = 13;
	const float kMoveLimitY = 7;

	worldTransform_.translation_.x = std::max(worldTransform_.translation_.x, -kMoveLimitX);
	worldTransform_.translation_.x = std::min(worldTransform_.translation_.x, +kMoveLimitX);

	worldTransform_.translation_.y = std::max(worldTransform_.translation_.y, -kMoveLimitY);
	worldTransform_.translation_.y = std::min(worldTransform_.translation_.y, +kMoveLimitY);

#ifdef _DEBUG
	ImGui::Begin("PlayerPostion");
	ImGui::DragFloat3("position",&worldTransform_.translation_.x,0.01f,-100.0f,100.0f);

	ImGui::End();
#endif

	//横移動
	if (input_->PushKey(DIK_LEFT) || input_->PushKey(DIK_A)) {
		move.x -= kCharacteaSpeed;
	} else if (input_->PushKey(DIK_RIGHT) || input_->PushKey(DIK_D)) {
		move.x += kCharacteaSpeed;
	}

	//上下移動
	if (input_->PushKey(DIK_UP) || input_->PushKey(DIK_W)) {
		move.y += kCharacteaSpeed;
	} else if (input_->PushKey(DIK_DOWN) || input_->PushKey(DIK_S)) {
		move.y -= kCharacteaSpeed;
	}

	worldTransform_.translation_ += move;

}
