#pragma once
#include "MyMath.h"
#include "RandomEngine.h"
#include <KamataEngine.h>
#include <cmath>

class Enemy;
class ShieldEnemy;

enum class GuardEffectBehavior {
	kRoot,
	kFadeOut,
	kUnknown,
};

class GuardEffect {

public:
	static GuardEffect* Create(const KamataEngine::Vector3& effectPosition);
	~GuardEffect();

	void Initialize(const KamataEngine::Vector3& effectPosition);
	void Update();
	void Draw();

	static void SetModel(KamataEngine::Model* model) { model_ = model; }
	static void SetCamera(KamataEngine::Camera* camera) { camera_ = camera; }

	bool isDead_ = false;
	bool IsDead() const { return isDead_; }

private:
	// ビヘイビア関係
	GuardEffectBehavior behavior_ = GuardEffectBehavior::kRoot;
	GuardEffectBehavior behaviorRequest_ = GuardEffectBehavior::kUnknown;
	float fadeTimer_ = 0.0f;
	static inline const float kFadeOutDuration = 0.3f;

	// プライベート関数
	void BehaviorRootUpdate();
	void BehaviorFadeOutUpdate();

	static KamataEngine::Model* model_;
	static KamataEngine::Camera* camera_;
	KamataEngine::WorldTransform circWorldTransform_;
	std::array<KamataEngine::WorldTransform, 4> ellipseWorldTransforms_;
	float rotateZ;
};
