#pragma once
#include "AABB.h"
#include "CameraController.h"
#include "Collision.h"
#include "DeathParticles.h"
#include "Enemy.h"
#include "KamataEngine.h"
#include "MapChipField.h"
#include "Player.h"
#include "SkyDome.h"
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
	KamataEngine::Model* modelParticl_ = nullptr; // 敵のモデル

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
	SkyDome* SkyDome_ = nullptr;
	MapChipField* mapChipField_;
	CameraController* cameraController_;
	DeathParticles* deathParticles_ = nullptr;
	std::list<Enemy*> enemyis_;
	Fade* fade_ = nullptr;

private:
	void CheckAllCollisions();
	void PlayeUpdate();
	void DeathUpdate();

	uint32_t textureHandle_;
	bool isDebugCamera_ = false;
	// プライベートでシーンごとの旋回用の処理関数を追加
};
