#pragma once
#include "KamataEngine.h"
#include "UpdateWorldTransform.h"

class EnemyBullet {

public:
	void Initialize(
		KamataEngine::Model* model, 
		const KamataEngine::Vector3& position, 
		const KamataEngine::Vector3& velocity);
	void Update();
	void Draw(const KamataEngine::Camera* camera);
	bool IsDead() const { return isDead_; }


	// 当たり判定のコールバック
	void OnCollision();
	KamataEngine::Vector3 GetWorldPosition();

private:

	// ワールドトランスフォーム
	KamataEngine::WorldTransform worldTransform_;
	// 3Dモデルで必要なモデルの呼び出し
	KamataEngine::Model* model_ = nullptr;
	// テクスチャハンドル
	uint32_t textureHandle_;
	// 弾の色。敵の弾は危険色の赤で統一する
	KamataEngine::ObjectColor objectColor_;

	KamataEngine::Vector3 velocity_;

	// 敵弾の色（真っ赤）。
	// シェーダーは shadecolor * texcolor * color で、shadecolor はライトの加算なので
	// 1.0 を超える。G/B が少しでも残っていると明るい面でそこが持ち上がって
	// ピンクに褪せるため、緑と青は完全に 0 にしておく
	static constexpr float kColorR = 1.0f;
	static constexpr float kColorG = 0.0f;
	static constexpr float kColorB = 0.0f;

	// 見た目の大きさ。モデルの直径が約2.1、当たり判定の直径が1.0なので縮める
	static constexpr float kScale = 0.5f;

	static const int32_t kLifeTime = 60 * 5;
	int32_t deathTime_ = kLifeTime;
	bool isDead_ = false;


};
