#include "playerBullet.h"

using namespace KamataEngine;


void playerBullet::Initialize(
	Model* model,
	const Vector3& position
) {

	assert(model);
	model_ = model;

	textureHandle_ = TextureManager::Load("white1x1.png");

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;

}

void playerBullet::Update() {
	UpdateWorldTransform(worldTransform_);
}


void playerBullet::Draw(
	const Camera* camera
) { 
	model_->Draw(worldTransform_, *camera); 
}
