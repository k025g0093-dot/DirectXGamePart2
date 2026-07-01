#include "EnemyBullet.h"

using namespace KamataEngine;

void EnemyBullet::Initialize(Model* model, const Vector3& position, const Vector3& velocity) {

	assert(model);
	model_ = model;

	textureHandle_ = TextureManager::Load("white1x1.png");

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	velocity_ = velocity;
}

void EnemyBullet::Update() {

	if (--deathTime_ <= 0) {
		isDead_ = true;
	}

	worldTransform_.translation_.x += velocity_.x;
	worldTransform_.translation_.y += velocity_.y;
	worldTransform_.translation_.z += velocity_.z;

	UpdateWorldTransform(worldTransform_);
}


void EnemyBullet::OnCollision() { 
	isDead_ = true;
}



void EnemyBullet::Draw(const Camera* camera) { model_->Draw(worldTransform_, *camera); }

Vector3 EnemyBullet::GetWorldPosition() {
	Vector3 worldPos{};

	worldPos.x = worldTransform_.translation_.x;
	worldPos.y = worldTransform_.translation_.y;
	worldPos.z = worldTransform_.translation_.z;

	return worldPos;
}


