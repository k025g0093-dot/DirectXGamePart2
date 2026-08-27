#pragma once
#include "EnemyBullet.h"
#include "KamataEngine.h"
#include "MyMath.h"

enum class EnemyType {

	kNormal, // 既存：まっすぐ前進しながら自機狙い弾を撃つ
	kTank,   // タンク：大きくて硬い、弾は撃たない
	kZigzag, // 円を描きながら進む
	kWave,   // 左右に大きく波打ちながら前進する
	kHover,  // 手前まで進んだら停止して、左右に往復しながら撃ち続ける
	kDive,   // 上空から急降下して、途中で水平飛行に切り替える
	kOrbit,  // プレイヤーの周りを回り込むように動く
	kBoss,   // ボス：左右に大きく移動しながら多方向弾を撃ち続ける
};

enum class Phase {
	Approach, // 接近
	Leave,    // 離脱
};

class GameScene;

class Player;

class Enemy {

public:
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);
	void InitApproach();
	void Update();
	void Draw();

	bool isDead_ = false;
	bool isConverting_ = false; // 味方化イージング中
	bool IsDead() const { return isDead_; };
	bool IsConverting() const { return isConverting_; };

	// 味方化イージング開始
	void StartConversion(const KamataEngine::Vector3& targetPos);
	bool IsConversionDone() const { return conversionDone_; };

	// 画面外へ抜けて退場扱いになったか（撃破ではないのでエフェクトは出さない）
	bool IsExpired() const { return isExpired_; };

	Player* player_ = nullptr;
	void SetPlayer(Player* player) { player_ = player; }
	KamataEngine::Vector3 GetWorldPosition();
	KamataEngine::WorldTransform worldTransform_;

	// 当たり判定のコールバック
	void OnCollision();

	void SetGameScene(GameScene* gameScene) { gameScene_ = gameScene; }
	void GetEnemyBulletModel(KamataEngine::Model* enemyBulletModel_) { bulletModel_ = enemyBulletModel_; }

	void SetModel(KamataEngine::Model* model) { model_ = model; }
	KamataEngine::Model* GetModel() const { return model_; }
	EnemyType GetEnemyType() const { return enemyType_; }

	EnemyType enemyType_ = EnemyType::kNormal;
	void SetEnemyType(EnemyType type) { enemyType_ = type; }

	float collisionRadius_ = 1.0f;
	float GetCollisionRadius() const { return collisionRadius_; }
	bool isWeakened_ = false;
	bool IsWeakened() const { return isWeakened_; }
	int32_t GetHp() const { return hp_; }
	int32_t GetMaxHp() const { return kMaxHp; }

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

	// 敵のHP
	int32_t hp_ = {};
	// 敵HPの最大値
	int32_t kMaxHp = {};
	// 30Fの無敵時間
	int32_t incvincibleTimer_ = 0;
	int32_t incvincibleTimerMax_ = 15;

	float circleTimer_ = 0.0f;                // 円運動の進行角（フレームごとに加算）
	float circleRadius_ = 3.0f;               // 円の半径
	KamataEngine::Vector3 circleCenter_ = {}; // 円の中心（スポーン位置）

	GameScene* gameScene_ = nullptr;

	// 味方化イージング
	int32_t convertTimer_ = 0;
	static const int32_t kConvertDuration = 30;
	KamataEngine::Vector3 convertStartPos_ = {};
	KamataEngine::Vector3 convertTargetPos_ = {};
	bool conversionDone_ = false;

	// 画面外へ抜けたときの退場処理
	bool isExpired_ = false;
	int32_t offScreenTimer_ = 0;
	// 画面外と判定してから消えるまでのフレーム数（60 = 約1秒）
	static const int32_t kOffScreenLife = 60;
	// カメラからこれだけ後ろへ下がったら画面外とみなす
	static constexpr float kOffScreenMargin = 3.0f;

	// プレイヤーを追い越した敵のフェードアウト
	// 現在の不透明度（1.0で不透明）と、弱体化などで変わる元の色
	float fadeAlpha_ = 1.0f;
	KamataEngine::Vector3 baseColor_ = {1.0f, 1.0f, 1.0f};
	// プレイヤーからこれだけ後ろへ下がると完全に透明になる
	static constexpr float kFadeDistance = 16.0f;

private: // 追加した移動パターンで使うパラメータ
	// 波状移動・往復移動などで共通して使う進行角
	float moveTimer_ = 0.0f;
	// 左右の振れ幅（周回タイプでは半径として使う）
	float moveAmplitude_ = 3.0f;
	// 移動パターンの進行速度（大きいほど速く揺れる）
	float moveSpeed_ = 0.05f;

	// kHover：停止するまで前進し続けるフレーム数
	int32_t advanceTime_ = 120;

	// kDive：降下を止める高さと降下速度
	float diveTargetY_ = 0.0f;
	float diveSpeed_ = 0.10f;

	// kBoss：多方向弾の角度オフセット
	float bossSpreadAngle_ = 0.0f;
	int32_t bossPhase_ = 0; // HP残量に応じたフェーズ

private:
	Phase phase_ = Phase::Approach;

	// 射撃間隔の初期値（タイプごとに fireInterval_ で上書きする）
	static const int kFireInterval = 60;
	int32_t fireInterval_ = kFireInterval;
	int32_t kShotTimer = 0;
	static const int32_t kWeakenHp = 2;


	void UpdateApproach();
	void UpdateLeave();
	void EnemyShotUpdate();

	// タイプごとの移動パターン
	void UpdateMoveZigzag();
	void UpdateMoveWave();
	void UpdateMoveHover();
	void UpdateMoveDive();
	void UpdateMoveOrbit();
	void UpdateMoveBoss();

	// 画面外へ抜けた敵の退場判定
	void UpdateOffScreen();

	// プレイヤーを追い越した敵を薄くしていく
	void UpdateFadeOut();
};