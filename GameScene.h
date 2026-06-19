#pragma once
#include "AABB.h"
#include "CameraController.h"
#include "Collision.h"
#include "DeathParticles.h"

#include "Enemy.h"
#include "ShieldEnemy.h"

#include "KamataEngine.h"
#include "MapChipField.h"
#include "Player.h"
#include "SkyDome.h"
#include "HitEffect.h"
#include "GuardEffect.h"

#include <vector>
#include "Fade.h"
enum class Phase {kFadeIn, kPlay, kDeath,kFadeOut };

class GameScene {

public:
	~GameScene();

	// シーンに関する変数
	Phase phase_;

	// ワールドトランスフォーム
	KamataEngine::WorldTransform worldTransform_;

	// カメラ
	KamataEngine::Camera camera_;
	KamataEngine::DebugCamera* debugCamera_ = nullptr;

#pragma region 3Dに非一ようなものを呼び出すもの

	// 3Dモデルで必要なモデルの呼び出し
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Model* Mapmodel_ = nullptr;     // マップのモデル
	KamataEngine::Model* modelSkydome_ = nullptr; // スカイドームのモデル
	KamataEngine::Model* modelPlayer_ = nullptr;  // プレイヤーのモデル
	KamataEngine::Model* modelAttack_ = nullptr;  // プレイヤーの攻撃モデル
	KamataEngine::Model* modelMap_ = nullptr;     // マップのモデル
	
	KamataEngine::Model* modelEnemy_ = nullptr;   // 敵のモデル
	KamataEngine::Model* modelshieldEnemy_ = nullptr;

	KamataEngine::Model* modelParticl_ = nullptr; // プレイヤーの死亡時に出るパーティクル
	KamataEngine::Model* modelHitEffect = nullptr;//敵が攻撃を食らったときに出るエフェクト
	KamataEngine::Model* modelGuardEffect_ = nullptr;

#pragma endregion

	std::vector<std::vector<KamataEngine::WorldTransform*>> worldTransformBlocks_;

	// 初期化関数
	void Initialize();

	// 更新処理関数
	void Update();

	// 描画処理
	void Draw();

	void GenerateBlocks();

	// 終了フラグ
	bool finished_ = false;
	// 終了フラグ
	bool IsFinished() const { return finished_; }

	Player* player_ = nullptr;
	Enemy* enemy_ = nullptr;
	std::list<Enemy*> enemyis_;

	ShieldEnemy* shieldEnemy_ = nullptr;
	std::list<ShieldEnemy*> shieldEnemyis_;

	SkyDome* SkyDome_ = nullptr;
	MapChipField* mapChipField_;
	CameraController* cameraController_;
	DeathParticles* deathParticles_ = nullptr;
	Fade* fade_ = nullptr;

	HitEffect* hitEffect_ = nullptr;
	std::list < HitEffect*> hitEffects_;

	GuardEffect* guardEffect_ = nullptr;
	std::list<GuardEffect*> guardEffects_;


	void CreateHitEffect(const KamataEngine::Vector3 postion);
	void CreateGuardEffect(const KamataEngine::Vector3 postion);

private:
	void CheckAllCollisions();
	void PlayeUpdate();
	void DeathUpdate();
	uint32_t textureHandle_;
	int32_t maxEnemyCount;
	bool isDebugCamera_ = false;
	// プライベートでシーンごとの旋回用の処理関数を追加
};
