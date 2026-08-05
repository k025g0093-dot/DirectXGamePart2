#pragma once
#include "EnemyBullet.h"
#include "KamataEngine.h"
#include "MyMath.h"

enum class EnemyType {

	kNormal, // 既存
	kTank,   // タンク
	kZigzag,//ジグザグに動く敵
};

enum class Phase {
	Approach, // 接近
	Leave,    // 離脱
};

class GameScene;

class Player;

class Enemy {

public:
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);
	void InitApproach();
	void Update();
	void Draw();

	bool isDead_ = false;
	bool IsDead() const { return isDead_; };

	Player* player_ = nullptr;
	void SetPlayer(Player* player) { player_ = player; }
	KamataEngine::Vector3 GetWorldPosition();
	KamataEngine::WorldTransform worldTransform_;

	// 当たり判定のコールバック
	void OnCollision();

	void SetGameScene(GameScene* gameScene) { gameScene_ = gameScene; }
	void GetEnemyBulletModel(KamataEngine::Model* enemyBulletModel_) { bulletModel_ = enemyBulletModel_; }

	EnemyType enemyType_ = EnemyType::kNormal;
	void SetEnemyType(EnemyType type) { enemyType_ = type; }

	float collisionRadius_ = 1.0f;
	float GetCollisionRadius() const { return collisionRadius_; }

private:
	// 3Dモデルで必要なモデルの呼び出し
	KamataEngine::Model* model_ = nullptr;
	// カメラ
	KamataEngine::Camera* camera_ = nullptr;
	// 敵の移動速度
	KamataEngine::Vector3 velocity_ = {};

	KamataEngine::Model* bulletModel_ = nullptr;

	// 敵のHP
	int32_t hp_ = {};
	// 敵HPの最大値
	int32_t kMaxHp = {};
	// 30Fの無敵時間
	int32_t incvincibleTimer_ = 0;
	int32_t incvincibleTimerMax_ = 15;

	float circleTimer_ = 0.0f;  // 円運動の進行角（フレームごとに加算）
	float circleRadius_ = 3.0f; // 円の半径
	KamataEngine::Vector3 circleCenter_ = {}; // 円の中心（スポーン位置）

	GameScene* gameScene_ = nullptr;

private:
	Phase phase_ = Phase::Approach;

	static const int kFireInterval = 60;
	int32_t kShotTimer = 0;

	void UpdateApproach();
	void UpdateLeave();
	void EnemyShotUpdate();



};
