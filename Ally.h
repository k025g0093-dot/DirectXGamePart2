#pragma once
#include "KamataEngine.h"

class Player;
class Enemy;
class GameScene;

class Ally {

public:
	// 初期化関数
	void Initialize(
		KamataEngine::Model* model,
		KamataEngine::Camera* camera, 
		const KamataEngine::Vector3& position,
		int32_t slotIndex
		);

	// 更新処理
	void Updata();

	void Draw();

	void FollowFotmation();

	KamataEngine::Vector3 GetWorldPosition();
	KamataEngine::WorldTransform worldTransform_;


	void SetPlayer(Player* player) { player_ = player; }
	void SetTarget(Enemy* target) { target_ = target; }
	void SetGameScene(GameScene* gameScene) { gamescene_ = gameScene; };
	void SetBulletModel(KamataEngine::Model* model) { bulletModel_ = model; }

	Player* player_ = nullptr;
	Enemy* target_ = nullptr;
	GameScene* gamescene_ = nullptr;



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

	KamataEngine::Vector3 slotOffset_ = {};

	float swayTimer_ = 0.0f;
	int32_t shotTimer_ = 0;
	static const int32_t kFireInterval = 90;



};
