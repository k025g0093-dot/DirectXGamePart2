#include "Enemy.h"
#include "UpdateWorldTransform.h"

using namespace KamataEngine;

void Enemy::Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position) {

	assert(model); // セッターにモデルを入れる
	model_ = model;

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.rotation_ = {0.0f, 6.3f, 0.0f};
	velocity_ = {0, 0, -0.01f};
	camera_ = camera;

	InitApproach();
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

void Enemy::Draw() { 
	for (EnemyBullet* bullet : bullets_) {
		bullet->Draw(camera_);
	}
	model_->Draw(worldTransform_, *camera_);

}

void Enemy::InitApproach() { kShotTimer = kFireInterval; }

void Enemy::UpdateApproach() {
	worldTransform_.translation_.x += velocity_.x;
	worldTransform_.translation_.y += velocity_.y;
	worldTransform_.translation_.z += velocity_.z;

	if (worldTransform_.translation_.z <= 0.0f) {
		velocity_ = {0.01f, 0.01f, 0};
		phase_ = Phase::Leave;
	}

	bullets_.remove_if([](EnemyBullet* bullet) {
		if (bullet->IsDead()) {
			delete bullet;
			return true;
		}
		return false;
	});
	
	--kShotTimer; 

	if (kShotTimer < 0) {
		EnemyShotUpdate();
		kShotTimer = kFireInterval;
	}

	for (EnemyBullet* bullet : bullets_) {
		bullet->Update();
	}

}

void Enemy::UpdateLeave() {
	worldTransform_.translation_.x += velocity_.x;
	worldTransform_.translation_.y += velocity_.y;
}

void Enemy::EnemyShotUpdate() {

	const float kBulletSpeed = -1.0f;
	Vector3 velocity(0, 0, kBulletSpeed);
	velocity = TransformNolmar(velocity, worldTransform_.matWorld_);

	EnemyBullet* newBullet = new EnemyBullet();
	newBullet->Initialize(model_, worldTransform_.translation_, velocity);
	bullets_.push_back(newBullet);
}
