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

	// 移動パターンの基準になる位置（スポーン地点）を覚えておく
	circleCenter_ = position;

	// 難易度でベースのHPを決める
	DifficultyLevel level = gameScene_ ? gameScene_->GetDifficultyLevel() : DifficultyLevel::kNormal;

	switch (level) {
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

	// タイプごとのステータス設定
	switch (enemyType_) {
	case EnemyType::kTank:
		// 硬くて大きい、弾は撃たずに体当たりで迫ってくる
		kMaxHp *= 3;
		worldTransform_.scale_ = {2.0f, 2.0f, 2.0f};
		collisionRadius_ = 2.0f;
		velocity_ = {0, 0, -0.004f}; // 低速
		break;

	case EnemyType::kZigzag:
		// 円を描きながら小刻みに撃ってくる
		kMaxHp /= 2;
		worldTransform_.scale_ = {1.0f, 1.0f, 1.0f};
		collisionRadius_ = 1.0f;
		velocity_ = {0, 0, -0.08f};
		fireInterval_ = 10; // 連射
		break;

	case EnemyType::kWave:
		// 左右に大きく波打つので狙いにくい
		worldTransform_.scale_ = {1.0f, 1.0f, 1.0f};
		collisionRadius_ = 1.0f;
		velocity_ = {0, 0, -0.05f};
		moveAmplitude_ = 6.0f; // 左右の振れ幅
		moveSpeed_ = 0.04f;    // 波の速さ
		fireInterval_ = 50;
		break;

	case EnemyType::kRush:
		// 紙耐久のかわりに速い、溜めてから突っ込んでくる
		kMaxHp = kMaxHp / 3;
		if (kMaxHp < 1) {
			kMaxHp = 1;
		}
		worldTransform_.scale_ = {0.8f, 0.8f, 0.8f};
		collisionRadius_ = 0.8f;
		velocity_ = {0, 0, -0.02f}; // 溜め中はゆっくり
		rushChargeTime_ = 60;       // 溜め時間（フレーム）
		rushSpeed_ = 0.35f;         // 突進速度
		break;

	case EnemyType::kHover:
		// 前に出て止まり、左右に往復しながら撃ち続ける
		worldTransform_.scale_ = {1.2f, 1.2f, 1.2f};
		collisionRadius_ = 1.2f;
		velocity_ = {0, 0, -0.06f};
		advanceTime_ = 120;    // 前進する時間（フレーム）
		moveAmplitude_ = 6.0f; // 往復の幅
		moveSpeed_ = 0.03f;
		fireInterval_ = 40;
		break;

	case EnemyType::kDive:
		// 上空から急降下して、途中で水平飛行に切り替える
		worldTransform_.scale_ = {1.0f, 1.0f, 1.0f};
		collisionRadius_ = 1.0f;
		velocity_ = {0, 0, -0.04f};
		diveTargetY_ = position.y - 10.0f; // スポーン位置から10だけ降りる
		diveSpeed_ = 0.10f;
		fireInterval_ = 45;
		break;

	case EnemyType::kOrbit:
		// プレイヤーの周りを回り込むので画面内を動き回る
		kMaxHp += 2;
		worldTransform_.scale_ = {1.1f, 1.1f, 1.1f};
		collisionRadius_ = 1.1f;
		velocity_ = {0, 0, -0.03f};
		moveAmplitude_ = 7.0f; // プレイヤーからの周回半径
		moveSpeed_ = 0.025f;
		fireInterval_ = 70;
		break;

	case EnemyType::kBoss:
		// ボス：大きくて硬い、左右に移動しながら多方向弾を撃つ
		kMaxHp *= 10;
		worldTransform_.scale_ = {3.0f, 3.0f, 3.0f};
		collisionRadius_ = 3.0f;
		velocity_ = {0, 0, 0.016f}; // プレイヤーと同じ方向（+Z）に前進
		moveAmplitude_ = 10.0f;
		moveSpeed_ = 0.015f;
		fireInterval_ = 30;
		break;

	default:
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
		model_->Draw(worldTransform_, *camera_, &objectColor_);
}

void Enemy::InitApproach() { kShotTimer = fireInterval_; }

void Enemy::UpdateApproach() {

	// 共通の前進処理
	worldTransform_.translation_.x += velocity_.x;
	worldTransform_.translation_.y += velocity_.y;
	worldTransform_.translation_.z += velocity_.z;

	// タイプごとの移動パターン
	switch (enemyType_) {
	case EnemyType::kZigzag:
		UpdateMoveZigzag();
		break;

	case EnemyType::kWave:
		UpdateMoveWave();
		break;

	case EnemyType::kRush:
		UpdateMoveRush();
		break;

	case EnemyType::kHover:
		UpdateMoveHover();
		break;

	case EnemyType::kDive:
		UpdateMoveDive();
		break;

	case EnemyType::kOrbit:
		UpdateMoveOrbit();
		break;

	case EnemyType::kBoss:
		UpdateMoveBoss();
		break;

	default:
		break;
	}

	// 射撃処理（タンクと突進タイプは弾を撃たない）
	if (enemyType_ != EnemyType::kTank && enemyType_ != EnemyType::kRush&&!isWeakened_) {
		--kShotTimer;
		if (kShotTimer < 0) {
			EnemyShotUpdate();
			kShotTimer = fireInterval_;
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

#pragma region タイプごとの移動パターン

// 円を描きながら進む
void Enemy::UpdateMoveZigzag() {

	circleTimer_ += 0.03f; // 回転速度（大きくすると速い）
	worldTransform_.translation_.x = circleCenter_.x + sinf(circleTimer_) * circleRadius_;
	worldTransform_.translation_.y = circleCenter_.y + cosf(circleTimer_) * circleRadius_;
}

// 左右に大きく波打ちながら前進する
void Enemy::UpdateMoveWave() {

	moveTimer_ += moveSpeed_;

	worldTransform_.translation_.x = circleCenter_.x + sinf(moveTimer_) * moveAmplitude_;

	// 進行方向に合わせて機体を傾けると波打ってる感が出る
	worldTransform_.rotation_.z = -cosf(moveTimer_) * 0.5f;
}

// 溜めのあとプレイヤーめがけて高速で突進する
void Enemy::UpdateMoveRush() {

	// 突進を始めたあとは velocity_ をそのまま使うので何もしない
	if (isRushing_) {
		return;
	}

	// 溜め中：小刻みに震わせて突進の予兆を出す
	--rushChargeTime_;
	moveTimer_ += 0.8f;
	worldTransform_.translation_.x += sinf(moveTimer_) * 0.05f;

	if (rushChargeTime_ <= 0 && player_) {
		// この瞬間のプレイヤー方向へ velocity_ を固定して突進開始
		Vector3 direction = Normalize(player_->GetWorldPosition() - GetWorldPosition());
		velocity_ = direction * rushSpeed_;
		isRushing_ = true;
	}
}

// 手前まで進んだら停止して、左右に往復する
void Enemy::UpdateMoveHover() {

	if (advanceTime_ > 0) {
		// 進入中：そのまま前進させる
		--advanceTime_;

		if (advanceTime_ == 0) {
			// 停止した位置を往復の中心にする
			circleCenter_ = worldTransform_.translation_;
			velocity_ = {0.0f, 0.0f, -0.005f}; // ほぼ停止
		}
		return;
	}

	// 停止後：左右に往復する
	moveTimer_ += moveSpeed_;
	worldTransform_.translation_.x = circleCenter_.x + sinf(moveTimer_) * moveAmplitude_;
}

// 上空から急降下して、目標の高さで水平飛行に移る
void Enemy::UpdateMoveDive() {

	if (worldTransform_.translation_.y > diveTargetY_) {
		// 降下中：だんだん加速させる
		worldTransform_.translation_.y -= diveSpeed_;
		diveSpeed_ += 0.004f;

		// 機首を下に向ける
		worldTransform_.rotation_.x = -0.6f;
	} else {
		// 引き起こし：高さを固定して機体を水平に戻す
		worldTransform_.translation_.y = diveTargetY_;
		worldTransform_.rotation_.x += (0.0f - worldTransform_.rotation_.x) * 0.1f;
	}
}

// プレイヤーの周りを回り込むように動く
void Enemy::UpdateMoveOrbit() {

	if (!player_) {
		return;
	}

	moveTimer_ += moveSpeed_;

	// プレイヤーの位置を中心に、XY平面上を周回する
	Vector3 playerPos = player_->GetWorldPosition();
	float targetX = playerPos.x + cosf(moveTimer_) * moveAmplitude_;
	float targetY = playerPos.y + sinf(moveTimer_) * moveAmplitude_;

	// いきなり張り付くと不自然なので、少しずつ目標位置へ寄せる
	worldTransform_.translation_.x += (targetX - worldTransform_.translation_.x) * 0.05f;
	worldTransform_.translation_.y += (targetY - worldTransform_.translation_.y) * 0.05f;
}

// ボス：左右に大きく移動しながら弾を撃つ
void Enemy::UpdateMoveBoss() {

	moveTimer_ += moveSpeed_;

	// 左右に大きく往復
	worldTransform_.translation_.x = circleCenter_.x + sinf(moveTimer_) * moveAmplitude_;

	// HP残量に応じてフェーズ変化
	if (kMaxHp > 0) {
		float hpRatio = (float)hp_ / (float)kMaxHp;
		if (hpRatio > 0.5f) {
			bossPhase_ = 0; // 通常フェーズ
		} else if (hpRatio > 0.2f) {
			bossPhase_ = 1; // 高速移動
			moveSpeed_ = 0.03f;
		} else {
			bossPhase_ = 2; // 狂暴化
			moveSpeed_ = 0.05f;
			worldTransform_.rotation_.z = sinf(moveTimer_ * 2.0f) * 0.3f;
		}
	}
}

#pragma endregion

void Enemy::EnemyShotUpdate() {

	// プレイヤーがまだセットされていないフレームでは撃たない
	if (!player_ || !gameScene_) {
		return;
	}

	if (player_->GetWorldPosition().z > worldTransform_.translation_.z)
		return;


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

	} else if (enemyType_ == EnemyType::kBoss) {

		// ボス：扇形に弾を撃つ
		int32_t bulletCount = 3 + bossPhase_ * 2; // フェーズが上がるほど弾数増加
		float totalAngle = 0.8f + bossPhase_ * 0.3f; // 扇の広がりも増加
		float startAngle = -totalAngle / 2.0f;

		for (int32_t i = 0; i < bulletCount; i++) {
			float ratio = (bulletCount > 1) ? (float)i / (float)(bulletCount - 1) : 0.5f;
			float angle = startAngle + totalAngle * ratio;

			// プレイヤー方向を基準に角度を回転
			float baseAngle = atan2f(directionToPlayer.x, directionToPlayer.z);
			float finalAngle = baseAngle + angle;

			Vector3 velocity = {
			    sinf(finalAngle) * kBulletSpeed,
			    0.0f,
			    cosf(finalAngle) * kBulletSpeed
			};

			EnemyBullet* newBullet = new EnemyBullet();
			newBullet->Initialize(bulletModel_, GetWorldPosition(), velocity);
			gameScene_->AddEnemyBullet(newBullet);
		}

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

		if (enemyType_ != EnemyType::kTank && enemyType_ != EnemyType::kBoss && hp_ <= kWeakenHp) {
			isWeakened_ = true;
			objectColor_.SetColor({1.0f, 0.85f, 0.2f, 1.0f});
		}

	}
};

Vector3 Enemy::GetWorldPosition() {
	Vector3 worldPos{};

	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}