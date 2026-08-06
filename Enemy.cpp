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
	objectColor_.Initialize();
	objectColor_.SetColor({1.0f, 1.0f, 1.0f, 1.0f});

	velocity_ = {0, 0, -0.01f};
	camera_ = camera;

	circleCenter_ = position;

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
	default:
		break;
	}

	switch (enemyType_) {
	case EnemyType::kTank:
		kMaxHp *= 3;                                 // 難易度×3
		worldTransform_.scale_ = {2.0f, 2.0f, 2.0f}; // ← 追加
		collisionRadius_ = 2.0f;                     // ← サイズに合わせて
		velocity_ = {0, 0, -0.004f};                 // 低速
		break;

	case EnemyType::kZigzag:
		kMaxHp/=2;                                      // 難易度×3
		worldTransform_.scale_ = {1.0f, 1.0f, 1.0f}; // ← 追加
		collisionRadius_ = 1.0f;                     // ← サイズに合わせて
		velocity_ = {0, 0, -0.08f};                   // 低速
		break;

	default:
		break;
	}
	hp_ = kMaxHp;

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
		model_->Draw(worldTransform_, *camera_, &objectColor_);
}

void Enemy::InitApproach() { kShotTimer = kFireInterval; }

void Enemy::UpdateApproach() {
	worldTransform_.translation_.x += velocity_.x;
	worldTransform_.translation_.y += velocity_.y;
	worldTransform_.translation_.z += velocity_.z;

	//if (worldTransform_.translation_.z <= 0.0f) {
	//	velocity_ = {0.01f, 0.01f, 0};
	//	phase_ = Phase::Leave;
	//}

	if (enemyType_ != EnemyType::kTank) {
		--kShotTimer;
		if (kShotTimer < 0) {
			EnemyShotUpdate();
			kShotTimer = kFireInterval;
		}
	}

	if (enemyType_ == EnemyType::kZigzag) {
		circleTimer_ += 0.03f; // 回転速度（大きくすると速い）
		worldTransform_.translation_.x = circleCenter_.x + sinf(circleTimer_) * circleRadius_;
		worldTransform_.translation_.y = circleCenter_.y + cosf(circleTimer_) * circleRadius_; // ← z を y に変更
		//worldTransform_.rotation_.y = -circleTimer_; // 向きも合わせて回す（任意）

		--kShotTimer;
		if (kShotTimer < 0) {
			EnemyShotUpdate();
			kShotTimer = 10;
		}

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

	if (enemyType_ == EnemyType::kZigzag) {

		EnemyBullet* newBullet = new EnemyBullet();
		newBullet->Initialize(bulletModel_, GetWorldPosition(), {0, 0, -1.0f});
		gameScene_->AddEnemyBullet(newBullet);

	} else {

		// 速度 = 正規化された方向 × スピード
		Vector3 velocity = {directionToPlayer.x * kBulletSpeed, directionToPlayer.y * kBulletSpeed, directionToPlayer.z * kBulletSpeed};

		EnemyBullet* newBullet = new EnemyBullet();
		newBullet->Initialize(bulletModel_, GetWorldPosition(), velocity);
		gameScene_->AddEnemyBullet(newBullet);
	}
}      

void Enemy::OnCollision() {

	// 無敵時間中はダメージを受けない
	if (incvincibleTimer_ > 0) {
		return;
	}
	hp_--;
	objectColor_.SetColor({1.0f, 1.0f, 1.0f, 1.0f});
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
