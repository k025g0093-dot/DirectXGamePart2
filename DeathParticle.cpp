#include "DeathParticle.h"
#include "UpdateWorldTransform.h"

using namespace KamataEngine;

void DeathParticle::Initialize(Model* model, Camera* camera, const Vector3& position) {

	model_ = model;
	camera_ = camera;

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.scale_ = {0.5f, 0.5f, 0.5f};

	objectColor_.Initialize();
	objectColor_.SetColor({1.0f, 0.8f, 0.2f, 1.0f});

	// ランダムな方向に飛ばす
	velocity_.x = ((float)rand() / RAND_MAX - 0.5f) * 0.3f;
	velocity_.y = ((float)rand() / RAND_MAX - 0.5f) * 0.3f;
	velocity_.z = ((float)rand() / RAND_MAX - 0.5f) * 0.3f;
}

void DeathParticle::Update() {

	if (isDead_) {
		return;
	}

	timer_++;
	if (timer_ >= lifeTime_) {
		isDead_ = true;
		return;
	}

	// 移動
	worldTransform_.translation_ = velocity_;

	// フェードアウト（後半で縮む+薄くなる）
	float progress = (float)timer_ / (float)lifeTime_;
	float alpha = 1.0f - progress;
	float scale = 0.5f * (1.0f - progress * 0.5f);

	worldTransform_.scale_ = {scale, scale, scale};
	objectColor_.SetColor({1.0f, 0.8f, 0.2f, alpha});

	UpdateWorldTransform(worldTransform_);
}

void DeathParticle::Draw() {
	if (!isDead_) {
		model_->Draw(worldTransform_, *camera_, &objectColor_);
	}
}
