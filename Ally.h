#pragma once
#include "KamataEngine.h"

class Ally {

public:
	// 初期化関数
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);

	// 更新処理
	void Updata();

	void Draw();
	KamataEngine::Vector3 GetWorldPosition();
	KamataEngine::WorldTransform worldTransform_;

private:
	// 3Dモデルで必要なモデルの呼び出し
	KamataEngine::Model* model_ = nullptr;
	// objectの色変更
	KamataEngine::ObjectColor objectColor_;
	// カメラ
	KamataEngine::Camera* camera_ = nullptr;
	// 敵の移動速度
	KamataEngine::Vector3 velocity_ = {};

	KamataEngine::Model* bulletModel_ = nullptr;
};
