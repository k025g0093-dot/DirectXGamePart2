#include "GameScene.h"
using namespace KamataEngine;

GameScene::GameScene() { 
	Initialize(); 
}

void GameScene::Initialize() {
#pragma region 基礎システムの初期化
	worldTransform_.Initialize();
	camera_.Initialize();
	camera_.farZ = 2000;
#pragma endregion

	//プレイヤーのインスタンス
	player_ = new Player();
	cameraController_ = new CameraController();

	//プレイヤーのモデル生成
	playerModel_ = Model::CreateFromOBJ("player", true);   


#pragma region カメラコントローラーの設定
	cameraController_->Initialize();
	cameraController_->SetTarget(player_);

	// ここで距離の微調整が可能
	cameraController_->targetOffset_ = {0.0f, 0.0f, -20.0f};
//	cameraController_->SetMovableArea({11.0f, 100.0f, 6.0f, 100.0f}); // カメラの移動できる最大値、最少値
	cameraController_->Reset();

	// ライン描画用カメラをコントローラー側に同期
	PrimitiveDrawer::GetInstance()->SetCamera(&cameraController_->GetCamera());
#pragma endregion


	Vector3 playerPosition = {0, 0, 0}; 
	player_->Initialize(playerModel_, &cameraController_->GetCamera(), playerPosition);
	cameraController_->Reset();
}

void GameScene::Update() { 
	player_->Updata(); 
	 cameraController_->Update();
}

void GameScene::Draw() {
	Model::PreDraw();

	player_->Draw();
	
	Model::PostDraw();
}

GameScene::~GameScene() {

	//ポインタのデリート
	delete player_;

	//モデルのデリート
	delete playerModel_;


}



