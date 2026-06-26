#include "Enemy.h"
#include "UpdateWorldTransform.h"

using namespace KamataEngine;

void Enemy::Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position) {

	assert(model); // セッターにモデルを入れる
	model_ = model;

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.rotation_ = {0.0f, 180.0f, 0.0f};
	velocity_ = {0, 0, -0.01f};
	camera_ = camera;
}

void Enemy::Update() {

	switch (phase_) {
	case Phase::Approach:

		UpdateApproach();

		break;
	case Phase::Leave:

		UpdateLeave();

		break;
	default:
		break;
	}

	UpdateWorldTransform(worldTransform_);
}

void Enemy::Draw() { model_->Draw(worldTransform_, *camera_); }

void Enemy::UpdateApproach() {
	worldTransform_.translation_.x += velocity_.x;
	worldTransform_.translation_.y += velocity_.y;
	worldTransform_.translation_.z += velocity_.z;

	if (worldTransform_.translation_.z <= 0.0f) {
		velocity_ = {0.01f, 0.01f, 0};
		phase_ = Phase::Leave;
	}
}

void Enemy::UpdateLeave() {
	worldTransform_.translation_.x += velocity_.x;
	worldTransform_.translation_.y += velocity_.y;
}
