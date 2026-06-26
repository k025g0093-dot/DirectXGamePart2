#pragma once
#include "KamataEngine.h"
#include "MyMath.h"

class Enemy {

public:

	void Initialize(
		KamataEngine::Model* model,
		KamataEngine::Camera* camera, 
		const KamataEngine::Vector3& position);
	void Update();
	void Draw();


private:

	// ワールドトランスフォーム
	KamataEngine::WorldTransform worldTransform_;
	// 3Dモデルで必要なモデルの呼び出し
	KamataEngine::Model* model_ = nullptr;
	// カメラ
	KamataEngine::Camera* camera_ = nullptr;
	// 敵の移動速度
	KamataEngine::Vector3 velocity_ = {};

};
