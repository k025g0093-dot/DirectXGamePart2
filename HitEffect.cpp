#include "HitEffect.h"

KamataEngine::Model* HitEffect::model_ = nullptr;
KamataEngine::Camera* HitEffect::camera_ = nullptr;

HitEffect* HitEffect::Create(const KamataEngine::Vector3& effectPosition) {
	HitEffect* instance = new HitEffect;
	assert(instance);
	instance->Initialize(effectPosition);
	return instance;
}

HitEffect::~HitEffect() {  }

void HitEffect::Initialize(const KamataEngine::Vector3& effectPosition) {
	circWorldTransform_.Initialize();
	circWorldTransform_.scale_ = {1.0f, 1.0f, 1.0f};
	circWorldTransform_.translation_ = effectPosition;

	rotateZ = 0.0f;
	fadeTimer_ = 0.0f;

	for (KamataEngine::WorldTransform& worldTransform : ellipseWorldTransforms_) {
		worldTransform.Initialize();
		worldTransform.scale_ = {0.1f, 2.4f, 1.0f};
		worldTransform.rotation_ = {0.0f, 0.0f, rotateZ};
		worldTransform.translation_ = effectPosition;

		rotateZ = RandomEngine::GetRandomFloat(0.0f, 360.0f);
	}

	behavior_ = HitEffectBehavior::kRoot;
}

void HitEffect::Update() {
	if (behaviorRequest_ != HitEffectBehavior::kUnknown) {
		behavior_ = behaviorRequest_;
		behaviorRequest_ = HitEffectBehavior::kUnknown;
	}

	switch (behavior_) {
	case HitEffectBehavior::kRoot:
		BehaviorRootUpdate();
		break;
	case HitEffectBehavior::kFadeOut:
		BehaviorFadeOutUpdate();
		break;
	case HitEffectBehavior::kUnknown:
		break;
	default:
		break;
	}

	for (KamataEngine::WorldTransform& worldTransform : ellipseWorldTransforms_) {
		worldTransform.matWorld_ = MakeAffineMatrix(worldTransform.scale_, worldTransform.rotation_, worldTransform.translation_);
		worldTransform.TransferMatrix();
	}

	circWorldTransform_.matWorld_ = MakeAffineMatrix(circWorldTransform_.scale_, circWorldTransform_.rotation_, circWorldTransform_.translation_);
	circWorldTransform_.TransferMatrix();

}

void HitEffect::BehaviorRootUpdate() {
	fadeTimer_ += 1.0f / 60.0f;
	float t = fadeTimer_ / kFadeOutDuration;
	if (t > 1.0f) {
		fadeTimer_ = 0.0f;
		behavior_ = HitEffectBehavior::kFadeOut;
	}

	float scale = 0.1f + t;

	for (KamataEngine::WorldTransform& worldTransform : ellipseWorldTransforms_) {
		worldTransform.scale_.x = 0.1f * scale;
		worldTransform.scale_.y = 2.4f * scale;
	}

	circWorldTransform_.scale_ = {scale, scale, scale};
}

void HitEffect::BehaviorFadeOutUpdate() {
	fadeTimer_ += 1.0f / 60.0f;
	float t = fadeTimer_ / kFadeOutDuration;
	if (t > 1.0f) {
		t = 1.0f;
		isDead_ = true;
	}

	float scale = 1.0f - t;

	for (KamataEngine::WorldTransform& worldTransform : ellipseWorldTransforms_) {
		worldTransform.scale_.x = 0.1f * scale;
		worldTransform.scale_.y = 2.4f * scale;
	}

	circWorldTransform_.scale_ = {scale, scale, scale};
}

void HitEffect::Draw() {
	for (KamataEngine::WorldTransform& worldTransform : ellipseWorldTransforms_) {
		model_->Draw(worldTransform, *camera_);
	}
	model_->Draw(circWorldTransform_, *camera_);
}