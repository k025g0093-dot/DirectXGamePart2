#pragma once
#include "KamataEngine.h"
#include "UpdateWorldTransform.h"
#include "playerBullet.h"
#include <list>

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



	KamataEngine::Model* modelAttack_ = nullptr;
	KamataEngine::WorldTransform worldTransformAttack_;

	// コンストラクタとデストラクタ
	Player();
	~Player();

	// 初期化関数
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);

	// 更新処理
	void Updata();

	void Draw();
	
	void Rotate();

	bool isDead_=false;
	bool IsDead() const { return isDead_; };


	KamataEngine::Vector3 GetWotldPosition();


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

	//プレイヤーの弾
	playerBullet* bullet_=nullptr;
	std::list<playerBullet*> bullets_;


private: // プライベート関数群とかのその他

	// カメラ
	KamataEngine::Camera* camera_ = nullptr;
	//キー入力
	KamataEngine::Input* input_ = nullptr;


	// プライベート関数
	void MovePlayer();
	void Attack();
	
};
