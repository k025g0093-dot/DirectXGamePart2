#include "Ally.h"
#include "GameScene.h"

#include "UpdateWorldTransform.h"

using namespace KamataEngine;
	
void Ally::Initialize(
	KamataEngine::Model* model,
	KamataEngine::Camera* camera,
	const KamataEngine::Vector3& position
) {

		// assert(model); // セッターにモデルを入れる
	model_ = model;

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.rotation_ = {0.0f, 6.3f, 0.0f};
	objectColor_.Initialize();
	objectColor_.SetColor({1.0f, 1.0f, 1.0f, 1.0f});

	velocity_ = {0, 0, -0.01f};
	camera_ = camera;

}


void Ally::Updata() 
{


}

void Ally::Draw() 
{



}

