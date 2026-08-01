#pragma once
#include "KamataEngine.h"
#include "Plane.h"

#include "RailCameraController.h"
#include "Enemy.h"
#include "Player.h"
#include "SkyDome.h"

#include <sstream>

class GameScene

{
public:
	// ワールドトランスフォーム
	KamataEngine::WorldTransform worldTransform_;
	// カメラ
	KamataEngine::Camera camera_;

	GameScene();
	~GameScene();

	// 大枠の初期化、アップデート、描画処理
	void Initialize();
	void Update();
	void Draw();

	void CheckAllCollisions();
	void AddEnemyBullet(EnemyBullet* bullet);
	const std::list<EnemyBullet*>& GetBullets() const { return bullets_; }

	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Model* playerModel_ = nullptr;
	KamataEngine::Model* enemyModel_ = nullptr;
	KamataEngine::Model* enemyBulletModel_ = nullptr;

	KamataEngine::Model* skyDomeModel_=nullptr;
	KamataEngine::Model* planeModel_ = nullptr;

	//敵のポップデータ
	std::stringstream enemyPopCommands;
	void LoadEnemyPopData();
	void UpdateEnemyPopCommands();

	//敵の発生位置
	void SpawnEnemy(const KamataEngine::Vector3& position);


	// この先に小分けにしていく関数などを書く

	private :

	// playerなどのポインタ
	Player* player_ = nullptr;
	Enemy* enemy_ = nullptr;
	std::list<Enemy*> enemies_;

	SkyDome* skyDome_ = nullptr;
	Plane* plane_ = nullptr;
	EnemyBullet* enemyBullet_ = nullptr;
	std::list<EnemyBullet*> bullets_;

	RailCameraController* railCameraController_;
	KamataEngine::DebugCamera* debugCamera_ = nullptr;
	KamataEngine::Input* input_ = nullptr;

	bool isDebugCameraActive_ = false;

private://各エンティティの待機フラグ
	bool isPopEnemy_ = false;
	int32_t popEnemyWaitTime_ = 0;


};
