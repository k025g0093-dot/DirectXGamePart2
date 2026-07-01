#pragma once
#include "KamataEngine.h"

class Plane {

public:
	// SkyDome();
	/*~SkyDome();*/
	// 引数にモデルのポインタを追加
	void Initialize(KamataEngine::Model* model);

	void Update();

	void Draw(KamataEngine::Camera* camera);

private:
	KamataEngine::Camera camera_;

	KamataEngine::WorldTransform worldTransform_;

	KamataEngine::Model* model_ = nullptr;
};
