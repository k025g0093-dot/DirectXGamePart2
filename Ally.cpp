#include "Ally.h"
#include "GameScene.h"

#include "Player.h"

#include "UpdateWorldTransform.h"

using namespace KamataEngine;

void Ally::Initialize(
    KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position, int32_t slotIndex

) {

	// assert(model); // セッターにモデルを入れる
	model_ = model;

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.rotation_ = {0.0f, 6.3f, 0.0f};
	objectColor_.Initialize();
	objectColor_.SetColor({1.0f, 1.0f, 1.0f, 1.0f});

	velocity_ = {0, 0, -0.01f};
	camera_ = camera;

	int32_t row = slotIndex / 5;
	int32_t col = slotIndex % 5;

	slotOffset_ = {(col - 2) * 2.0f, 0.5f + row * 1.5f, -(4.0f + row * 3.0f)};

	swayTimer_ = (float)(slotIndex % 7) * 0.5f;
	shotTimer_ = kFireInterval;
}

void Ally::Updata() {

	FollowFotmation();
	UpdateWorldTransform(worldTransform_);

}

void Ally::Draw() { model_->Draw(worldTransform_, *camera_); }

void Ally::FollowFotmation() {

	if (!player_) {
		return;
	}

	Vector3 playerPos = player_->GetWorldPosition();
	Vector3 targetPos = {playerPos.x + slotOffset_.x, playerPos.y + slotOffset_.y, playerPos.z + slotOffset_.z};

	swayTimer_ += 0.05f;
	targetPos.x = sin(swayTimer_) * 0.5f;
	targetPos.y = cos(swayTimer_ * 0.8f) * 0.3f;

	// 追従
	const float kFollowSpeed = 0.05f;
	worldTransform_.translation_ = (targetPos - worldTransform_.translation_) * kFollowSpeed;
}
