#pragma once
#include "KamataEngine.h"
#include "MyMath.h"
#include "EnemyBullet.h"

enum class Phase {
	Approach, // 接近
	Leave,    // 離脱
};


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
	KamataEngine::Vector3 GetWotldPosition();
	KamataEngine::WorldTransform worldTransform_;

private:

	// 3Dモデルで必要なモデルの呼び出し
	KamataEngine::Model* model_ = nullptr;
	// カメラ
	KamataEngine::Camera* camera_ = nullptr;
	// 敵の移動速度
	KamataEngine::Vector3 velocity_ = {};

	EnemyBullet* bullet_ = nullptr;
	std::list<EnemyBullet*> bullets_;

private:
	Phase phase_ = Phase::Approach;


	static const int kFireInterval = 60;
	int32_t kShotTimer = 0;

	void UpdateApproach();
	void UpdateLeave();
	void EnemyShotUpdate();
};
