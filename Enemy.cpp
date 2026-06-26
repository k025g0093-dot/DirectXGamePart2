#include "Enemy.h"
#include "UpdateWorldTransform.h"

using namespace KamataEngine;

void Enemy::Initialize(
	KamataEngine::Model* model, 
	KamataEngine::Camera* camera,
	const KamataEngine::Vector3& position
) {

	assert(model); // セッターにモデルを入れる
	model_ = model;

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.rotation_ = {0.0f, 180.0f, 0.0f};
	velocity_ = {0, 0, -0.01f};
	camera_ = camera;

}

void Enemy::Update() {

	worldTransform_.translation_.x += velocity_.x;
	worldTransform_.translation_.y += velocity_.y;
	worldTransform_.translation_.z += velocity_.z;
	
	UpdateWorldTransform(worldTransform_);

}

void Enemy::Draw() {
	model_->Draw(worldTransform_, *camera_);
}
