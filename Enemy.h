#pragma once
#include "KamataEngine.h"
#include "MyMath.h"

enum class Phase {
	Approach,//接近
	Leave,//離脱
};

class Enemy {

public:
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);
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

private:

	Phase phase_ =Phase::Approach;

	void UpdateApproach();
	void UpdateLeave();

};
