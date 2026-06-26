#pragma once
#include "KamataEngine.h"
#include "UpdateWorldTransform.h"

class playerBullet {

public:
	void Initialize(KamataEngine::Model* model, const KamataEngine::Vector3& position);
	void Update();
	void Draw(const KamataEngine::Camera* camera);

private:

	// ワールドトランスフォーム
	KamataEngine::WorldTransform worldTransform_;
	// 3Dモデルで必要なモデルの呼び出し
	KamataEngine::Model* model_ = nullptr;
	// テクスチャハンドル
	uint32_t textureHandle_;


};
