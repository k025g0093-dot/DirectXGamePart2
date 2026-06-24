#pragma once
#include "KamataEngine.h"

#include "Player.h"
#include "CameraController.h"


class GameScene 

{
public:

	// ワールドトランスフォーム
	KamataEngine::WorldTransform worldTransform_;
	// カメラ
	KamataEngine::Camera camera_;

	GameScene();
	~GameScene();

	//大枠の初期化、アップデート、描画処理
	void Initialize();
	void Update();
	void Draw();

	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Model* playerModel_ = nullptr;

	//この先に小分けにしていく関数などを書く

private:
	
	//playerなどのポインタ
	Player* player_ = nullptr;
	CameraController* cameraController_;
	KamataEngine::DebugCamera* debugCamera_ = nullptr;
	KamataEngine::Input* input_ = nullptr;

	bool isDebugCameraActive_ = false;

};
