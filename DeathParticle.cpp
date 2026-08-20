#include "DeathParticle.h"
#include "UpdateWorldTransform.h"

using namespace KamataEngine;

void DeathParticle::Initialize(Model* model, Camera* camera, const Vector3& position) {

	model_ = model;
	camera_ = camera;

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.scale_ = {1.5f, 1.5f, 1.5f};

	objectColor_.Initialize();

	// ランダムな色（オレンジ〜黄色〜白）
	float r = 0.8f + (float)rand() / RAND_MAX * 0.2f;
	float g = 0.3f + (float)rand() / RAND_MAX * 0.5f;
	float b = (float)rand() / RAND_MAX * 0.2f;
	objectColor_.SetColor({r, g, b, 1.0f});

	// ランダムな方向に大力で飛ばす
	velocity_.x = ((float)rand() / RAND_MAX - 0.5f) * 0.8f;
	velocity_.y = ((float)rand() / RAND_MAX - 0.5f) * 0.8f;
	velocity_.z = ((float)rand() / RAND_MAX - 0.5f) * 0.8f;
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
	worldTransform_.translation_.x += velocity_.x;
	worldTransform_.translation_.y += velocity_.y;
	worldTransform_.translation_.z += velocity_.z;

	// 減速
	velocity_.x *= 0.95f;
	velocity_.y *= 0.95f;
	velocity_.z *= 0.95f;

	// フェードアウト（大きく始まって縮みながら消える）
	float progress = (float)timer_ / (float)lifeTime_;
	float alpha = 1.0f - progress * progress;
	float scale = 1.5f * (1.0f - progress * 0.7f);

	worldTransform_.scale_ = {scale, scale, scale};
	objectColor_.SetColor({objectColor_.GetColor().x, objectColor_.GetColor().y, objectColor_.GetColor().z, alpha});

	UpdateWorldTransform(worldTransform_);
}

void DeathParticle::Draw() {
	if (!isDead_) {
		model_->Draw(worldTransform_, *camera_, &objectColor_);
	}
}
