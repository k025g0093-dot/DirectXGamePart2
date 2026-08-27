#include "EnemyBullet.h"

using namespace KamataEngine;

void EnemyBullet::Initialize(Model* model, const Vector3& position, const Vector3& velocity) {

	//assert(model);
	model_ = model;

	textureHandle_ = TextureManager::Load("white1x1.png");

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	// モデルが当たり判定より大きいので見た目を縮める
	worldTransform_.scale_ = {kScale, kScale, kScale};
	velocity_ = velocity;

	// 敵の弾は赤。テクスチャの色に掛け算されるので白いテクスチャならそのまま赤になる
	objectColor_.Initialize();
	objectColor_.SetColor({kColorR, kColorG, kColorB, 1.0f});
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



void EnemyBullet::Draw(const Camera* camera) {
	model_->Draw(worldTransform_, *camera, &objectColor_);
}

Vector3 EnemyBullet::GetWorldPosition() {
	Vector3 worldPos{};

	worldPos.x = worldTransform_.translation_.x;
	worldPos.y = worldTransform_.translation_.y;
	worldPos.z = worldTransform_.translation_.z;

	return worldPos;
}


