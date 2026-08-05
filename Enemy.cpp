#include "Enemy.h"
#include "GameScene.h"
#include "Player.h"
#include "UpdateWorldTransform.h"

using namespace KamataEngine;

void Enemy::Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position) {

	// assert(model); // セッターにモデルを入れる
	model_ = model;

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.rotation_ = {0.0f, 6.3f, 0.0f};
	velocity_ = {0, 0, -0.01f};
	camera_ = camera;

	switch (gameScene_->GetDifficultyLevel()) {

	case DifficultyLevel::kEasy:
		kMaxHp = 3;
		break;

	case DifficultyLevel::kNormal:
		kMaxHp = 5;
		break;

	case DifficultyLevel::kHard:
		kMaxHp = 8;
		break;
	}

	hp_ = kMaxHp;

	InitApproach();
}

void Enemy::Update() {
	if (!isDead_) {

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
}

void Enemy::Draw() {
	if (!isDead_)
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

	--kShotTimer;

	if (kShotTimer < 0) {
		EnemyShotUpdate();
		kShotTimer = kFireInterval;
	}

	if (incvincibleTimer_ > 0) {
		incvincibleTimer_--;
	}
}

void Enemy::UpdateLeave() {
	worldTransform_.translation_.x += velocity_.x;
	worldTransform_.translation_.y += velocity_.y;
}

void Enemy::EnemyShotUpdate() {

	// assert(player_);

	float difficlutRate = gameScene_->GetDifficultyLevel() == DifficultyLevel::kEasy ? 0.5f : (gameScene_->GetDifficultyLevel() == DifficultyLevel::kNormal ? 1.0f : 1.5f);
	const float kBulletSpeed = 0.5f * difficlutRate;

	Vector3 directionToPlayer = player_->GetWorldPosition() - GetWorldPosition();

	float length = sqrt(directionToPlayer.x * directionToPlayer.x + directionToPlayer.y * directionToPlayer.y + directionToPlayer.z * directionToPlayer.z);

	// ゼロ除算チェック
	if (length > 0.0001f) {
		directionToPlayer.x /= length;
		directionToPlayer.y /= length;
		directionToPlayer.z /= length;
	}

	// 速度 = 正規化された方向 × スピード
	Vector3 velocity = {directionToPlayer.x * kBulletSpeed, directionToPlayer.y * kBulletSpeed, directionToPlayer.z * kBulletSpeed};

	EnemyBullet* newBullet = new EnemyBullet();
	newBullet->Initialize(bulletModel_, GetWorldPosition(), velocity);
	gameScene_->AddEnemyBullet(newBullet);
}

void Enemy::OnCollision() {

	// 無敵時間中はダメージを受けない
	if (incvincibleTimer_ > 0) {
		return;
	}
	hp_--;
	if (hp_ <= 0) {
		isDead_ = true;
	} else {
		incvincibleTimer_ = incvincibleTimerMax_; // 無敵開始
	}
};

Vector3 Enemy::GetWorldPosition() {
	Vector3 worldPos{};

	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}
