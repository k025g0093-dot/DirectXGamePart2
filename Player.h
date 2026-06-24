#pragma once

#include "KamataEngine.h"




class Player {

public:
#pragma region プレイヤーの向きを変えるのに使用するもの

	// プレイヤーの画像の向きを変えるためのイーナム
	enum class LRDirection { kLeft, kRight };
	// 初期の向き
	LRDirection lrDirection_ = LRDirection::kRight;
	// プレイヤーの向きの角度を補間
	float turnFistRotationY_ = 0.0f;
	// 旋回タイマー
	float turnTimer_ = 0.0f;

	LRDirection GetLRDirection() const { return lrDirection_; }

#pragma endregion

#pragma region プレイヤーのジャンプに関する処理

	// プレイヤーのジャンプに関するもの
	bool onGround_ = true;

#pragma endregion

	KamataEngine::Model* modelAttack_ = nullptr;
	KamataEngine::WorldTransform worldTransformAttack_;

	// コンストラクタとデストラクタ
	Player();
	~Player();

	// 初期化関数
	void Initialize(KamataEngine::Model* model, KamataEngine::Model* modelAttack, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);

	// 更新処理
	void Updata();

	void Draw();
	

	bool isDead_=false;
	bool IsDead() const { return isDead_; };

	bool isKnockBack_= false;
	void IsKnockBack();


	// 慣性系の物
	KamataEngine::Vector3 velocity_ = {};

	// プレイヤーの位置を扱えるように
	KamataEngine::WorldTransform& GetWorldTransform() { return worldTransform_; }

	const KamataEngine::Vector3& GetVelocity() const { return velocity_; };

	// Player.h の修正

	// Player.h 53行目付近
	struct CollisionMapInfo {
		bool isFloor = false;   // 天井
		bool isCeiling = false; // 着地
		bool isWall = false;  // 壁
		KamataEngine::Vector3 velocity_;
	};

	
	// ワールドトランスフォーム
	KamataEngine::WorldTransform worldTransform_;
	// 3Dモデルで必要なモデルの呼び出し
	KamataEngine::Model* model_ = nullptr;
	// テクスチャハンドル
	uint32_t textureHandle_;

private: // プライベート関数群とかのその他

	// カメラ
	KamataEngine::Camera* camera_ = nullptr;

	// マップチップによるフィールド

#pragma region プレイヤーの移動に関するもの

	// 加速度を入れるもの
	static inline const float kAcceleration = 0.1f;

	// 減速率
	static inline const float kAttenuation = 0.1f;

	// 最大加速度の設定
	static inline const float kLimitRunSpeed = 0.3f;

	// 天井に当たった際の速度
	static inline const float kBlank = 0.0f;

	// 着地時の速度減衰率
	static inline const float kAttenuationLanding = 0.01f;

	// 壁接着時の速度減衰率
	static inline const float kAttenuationWall = 0.1f;

#pragma endregion

#pragma region ジャンプに関するもの

	// 重力加速度
	static inline const float kGravityAcceleration = 0.03f;

	// 最大落下速度
	static inline const float kLimitFallSpeed = 0.5f;

	// ジャンプ初速
	static inline const float kJumpAcceleration = 0.6f;

#pragma endregion


	// 旋回時間＜秒＞
	static inline const float kTimeTrun = 0.3f;
	static constexpr float kAttackSpeed = 0.8f;
	// プライベート関数
	void MovePlayer();

	
};
