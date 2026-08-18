#pragma once
#include "KamataEngine.h"

class Player;
class Enemy;
class GameScene;

class Ally {

public:
	// 初期化関数
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position, int32_t slotIndex);

	// 更新処理
	void Updata();

	void Draw();

	// 攻撃
	void Attack();

	// 編隊を組んでプレイヤーに追従する
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

	// 編隊内での自分の定位置（プレイヤーからの相対位置）
	KamataEngine::Vector3 slotOffset_ = {};

	float swayTimer_ = 0.0f;
	int32_t shotTimer_ = 0;
	static const int32_t kFireInterval = 90;

private: // 追従の調整用（数値を下げるほど動きが落ち着く）
	// 定位置へ寄っていく速さ
	float followSpeed_ = 0.05f;
	// ゆらぎの進む速さ
	float swaySpeed_ = 0.05f;
	// ゆらぎの振れ幅（0.0f にすると完全に定位置で止まる）
	float swayAmplitudeX_ = 0.5f;
	float swayAmplitudeY_ = 0.3f;
};
