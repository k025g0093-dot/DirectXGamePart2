#pragma once
#include "KamataEngine.h"
#include "UpdateWorldTransform.h"

class EnemyBullet {

public:
	void Initialize(
		KamataEngine::Model* model, 
		const KamataEngine::Vector3& position, 
		const KamataEngine::Vector3& velocity);
	void Update();
	void Draw(const KamataEngine::Camera* camera);
	bool IsDead() const { return isDead_; }

private:

	// ワールドトランスフォーム
	KamataEngine::WorldTransform worldTransform_;
	// 3Dモデルで必要なモデルの呼び出し
	KamataEngine::Model* model_ = nullptr;
	// テクスチャハンドル
	uint32_t textureHandle_;

	KamataEngine::Vector3 velocity_;

	static const int32_t kLifeTime = 60 * 5;
	int32_t deathTime_ = kLifeTime;
	bool isDead_ = false;


};
