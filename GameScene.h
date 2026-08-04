#pragma once
#include "KamataEngine.h"
#include "Plane.h"

#include "Enemy.h"
#include "LockOn.h"
#include "Player.h"
#include "RailCameraController.h"
#include "SkyDome.h"

#include "Fade.h"


#include <sstream>

// 難易度
enum class DifficultyLevel { 
	kEasy, 
	kNormal,
	kHard 
};

enum class GamePhase {
	kFadeIn,
	kPlay,
	kFadeOut
};

class GameScene

{
public:

	GamePhase gamePhase_;


	// ワールドトランスフォーム
	KamataEngine::WorldTransform worldTransform_;
	// カメラ
	KamataEngine::Camera camera_;

	GameScene();
	~GameScene();

	// 大枠の初期化、アップデート、描画処理
	void Initialize();
	void Update();
	void GameUpdate();
	void Draw();

	void CheckAllCollisions();
	void AddEnemyBullet(EnemyBullet* bullet);
	const std::list<EnemyBullet*>& GetBullets() const { return bullets_; }

	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Model* playerModel_ = nullptr;
	KamataEngine::Model* model3DReticle_ = nullptr;

	KamataEngine::Model* enemyModel_ = nullptr;
	KamataEngine::Model* enemyBulletModel_ = nullptr;

	KamataEngine::Model* skyDomeModel_ = nullptr;
	KamataEngine::Model* planeModel_ = nullptr;

	// 敵のポップデータ
	std::stringstream enemyPopCommands;
	void LoadEnemyPopData();
	void UpdateEnemyPopCommands();

	// 敵の発生位置
	void SpawnEnemy(const KamataEngine::Vector3& position);

	// この先に小分けにしていく関数などを書く

	// 終了フラグ
	bool finished_ = false;
	// 終了フラグ
	bool IsFinished() const { return finished_; }

	//フェードに関する者
	Fade* fade_ = nullptr;

	bool isGameClear_ = false; // ゲームクリアフラグ

private:
	// playerなどのポインタ
	Player* player_ = nullptr;
	Enemy* enemy_ = nullptr;
	LockOn* lockOn_ = nullptr;
	std::list<Enemy*> enemies_;

	SkyDome* skyDome_ = nullptr;
	Plane* plane_ = nullptr;
	EnemyBullet* enemyBullet_ = nullptr;
	std::list<EnemyBullet*> bullets_;

	RailCameraController* railCameraController_;
	KamataEngine::DebugCamera* debugCamera_ = nullptr;
	KamataEngine::Input* input_ = nullptr;

	bool isDebugCameraActive_ = false;


private: // 各エンティティの待機フラグ
	bool isPopEnemy_ = false;
	int32_t popEnemyWaitTime_ = 0;

public:  // 難易度の値を取得したり敵のstatusを変更するための関数
	void SetDifficultyLevel(DifficultyLevel level) { difficultyLevel_ = level; }

	// 最初はノーマルで始める、そこからeasyやhardに変更で
	DifficultyLevel GetDifficultyLevel() const { return difficultyLevel_; }
	DifficultyLevel difficultyLevel_ = DifficultyLevel::kHard; // 難易度の初期値を設定

};
