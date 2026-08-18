#include "Ally.h"
#include "GameScene.h"

#include "MyMath.h"
#include "Player.h"

#include "UpdateWorldTransform.h"

#include <cmath>

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

	// 5体で1列の編隊を組み、いっぱいになったら後ろの列に回す
	int32_t row = slotIndex / 5;
	int32_t col = slotIndex % 5;

	slotOffset_ = {(col - 2) * 2.0f, 0.5f + row * 1.5f, -(4.0f + row * 3.0f)};

	// ゆらぎの位相を機体ごとにずらして、全員が同じ揺れ方にならないようにする
	swayTimer_ = (float)(slotIndex % 7) * 0.5f;

	shotTimer_ = kFireInterval;
}

void Ally::Updata() {

	FollowFotmation();
	Attack();
	UpdateWorldTransform(worldTransform_);
}

void Ally::Draw() { model_->Draw(worldTransform_, *camera_); }

// 攻撃
void Ally::Attack() {

	if (!target_ || !bulletModel_ || !gamescene_) {
		return;
	}

	--shotTimer_;
	if (shotTimer_ <= 0) {
		Vector3 allyPos = GetWorldPosition();
		Vector3 targetPos = target_->GetWorldPosition();
		Vector3 direction = targetPos - allyPos;
		direction = Normalize(direction);
		const float kBulletSpeed = 1.0f;
		Vector3 velocity = direction * kBulletSpeed;

		playerBullet* newBullet = new playerBullet();
		newBullet->Initialize(bulletModel_, allyPos, velocity);
		gamescene_->AddAllyBullet(newBullet);

		shotTimer_ = kFireInterval;
	}
}

// 編隊を組んでプレイヤーに追従する
void Ally::FollowFotmation() {

	if (!player_) {
		return;
	}

	// 定位置はプレイヤーからの相対位置で持っているので、毎フレーム今のプレイヤー位置に足す
	Vector3 playerPos = player_->GetWorldPosition();
	Vector3 targetPos = {playerPos.x + slotOffset_.x, playerPos.y + slotOffset_.y, playerPos.z + slotOffset_.z};

	// その場で止まって見えないように、定位置をゆっくり揺らす
	swayTimer_ += swaySpeed_;
	targetPos.x += sinf(swayTimer_) * swayAmplitudeX_;
	targetPos.y += cosf(swayTimer_ * 0.8f) * swayAmplitudeY_;

	// 追従
	worldTransform_.translation_ += (targetPos - worldTransform_.translation_) * followSpeed_;
}

Vector3 Ally::GetWorldPosition() {
	Vector3 worldPos{};

	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}
