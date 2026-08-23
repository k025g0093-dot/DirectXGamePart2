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

	// 出現アニメーション：スケール0から始める
	worldTransform_.scale_ = {0.0f, 0.0f, 0.0f};
	spawnTimer_ = 0;

	velocity_ = {0, 0, -0.01f};
	camera_ = camera;

	// 3体で1列、奥に広がる編隊
	int32_t row = slotIndex / 3;
	int32_t col = slotIndex % 3;

	slotOffset_ = {(col - 1) * 3.5f, 0.3f + row * 1.2f, (3.0f + row * 2.5f)};

	// ゆらぎの位相を機体ごとにずらして、全員が同じ揺れ方にならないようにする
	swayTimer_ = (float)(slotIndex % 7) * 0.5f;

	shotTimer_ = kFireInterval;
}

void Ally::Updata() {

	// 出現アニメーション（スケールアップ）
	if (spawnTimer_ < kSpawnDuration) {
		spawnTimer_++;
		float t = (float)spawnTimer_ / (float)kSpawnDuration;
		// イージング（-outQuad: 最後でスローになる）
		float scale = targetScale_ * (1.0f - (1.0f - t) * (1.0f - t));
		worldTransform_.scale_ = {scale, scale, scale};
	} else {
		worldTransform_.scale_ = {targetScale_, targetScale_, targetScale_};
	}

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
		newBullet->SetFlip(true);
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
