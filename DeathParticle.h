#pragma once
#include "KamataEngine.h"

class DeathParticle {
public:
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);
	void Update();
	void Draw();

	bool IsDead() const { return isDead_; }

private:
	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Camera* camera_ = nullptr;
	KamataEngine::ObjectColor objectColor_;
	KamataEngine::Vector3 velocity_ = {};

	int32_t lifeTime_ = 45;
	int32_t timer_ = 0;
	bool isDead_ = false;
};
