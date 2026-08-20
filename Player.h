#pragma once
#include "KamataEngine.h"
#include "UpdateWorldTransform.h"
#include "playerBullet.h"
#include <list>

class LockOn;

class Player {

public:
	// コンストラクタとデストラクタ
	Player();
	~Player();

	// 初期化関数
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);

	// 更新処理
	void Updata();

	void Draw();
	// UI描画
	void DrawUI();

	void Rotate();

	bool IsDead() const { return isDead_; };

	int32_t GetHp() const { return hp_; }
	int32_t GetMaxHp() const { return kMaxHp; }

	void SetParent(const KamataEngine::WorldTransform* parent);

	void SetLockOn(LockOn* lockOn) { lockOn_ = lockOn; }

	KamataEngine::Vector3 GetWorldPosition();

	// プレイヤーの位置を扱えるように
	KamataEngine::WorldTransform& GetWorldTransform() { return worldTransform_; }

	const KamataEngine::Vector3& GetVelocity() const { return velocity_; };

	// 当たり判定のコールバック
	void OnCollision();

	const std::list<playerBullet*>& GetBullets() const { return bullets_; }

	void Get3DReticleModel(KamataEngine::Model* model3DReticle) { model3DReticle_ = model3DReticle; }
	void Gaet2DReticlePosition(KamataEngine::Vector2& position) { position = reticlePosition2D; }

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

	bool isDead_ = false;

	// 慣性系の物
	KamataEngine::Vector3 velocity_ = {};

	// Player.h の修正
	// Player.h 53行目付近
	struct CollisionMapInfo {
		bool isFloor = false;   // 天井
		bool isCeiling = false; // 着地
		bool isWall = false;    // 壁
		KamataEngine::Vector3 velocity_;
	};

	// ワールドトランスフォーム
	KamataEngine::WorldTransform worldTransform_;
	// 3Dモデルで必要なモデルの呼び出し
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Model* model3DReticle_ = nullptr;

	// 2Dレティクルの画像入れるやつ
	KamataEngine::Sprite* sprite2DReticle_ = nullptr;

	// テクスチャハンドル
	uint32_t textureHandle_;

	// プレイヤーの弾
	playerBullet* bullet_ = nullptr;
	std::list<playerBullet*> bullets_;

	LockOn* lockOn_;

	KamataEngine::WorldTransform worldTransform3DReticle_;

	KamataEngine::Vector2 reticlePosition2D = {};

private:
	// プライベート関数F
	void MovePlayer();
	void Attack();
	void Update3DReticlePosition();
	void Update2DReticlePosition();

	// カメラ
	KamataEngine::Camera* camera_ = nullptr;
	// キー入力
	KamataEngine::Input* input_ = nullptr;
	//objectのカラー
	KamataEngine::ObjectColor objectColor_;

	// プレイヤーのHP
	int32_t hp_ = 5;
	// プレイヤーのHPの最大値
	static const int32_t kMaxHp = 5;
	//30Fの無敵時間
	int32_t incvincibleTimer_ = 0;
	int32_t incvincibleTimerMax_ = 30;

	float alfaColor = 1.0f;
};