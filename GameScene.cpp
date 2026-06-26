#include "GameScene.h"
using namespace KamataEngine;

GameScene::GameScene() { Initialize(); }

void GameScene::Initialize() {
#pragma region 基礎システムの初期化
	worldTransform_.Initialize();
	camera_.Initialize();
	camera_.translation_ = {0, 0, -20.0f};
	camera_.UpdateMatrix();
#pragma endregion

	// プレイヤーのインスタンス
	player_ = new Player();
	enemy_ = new Enemy();

	cameraController_ = new CameraController();

	// プレイヤーのモデル生成
	playerModel_ = Model::CreateFromOBJ("player", true);
	enemyModel_ = Model::CreateFromOBJ("enemy", true);

	debugCamera_ = new DebugCamera(1280, 720);

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
	player_->Initialize(playerModel_, &camera_, playerPosition);

	Vector3 enemyPosition = {0, 0, 10};
	enemy_->Initialize(enemyModel_, &camera_, enemyPosition);

	cameraController_->Reset();

	//キー入力の初期化
	input_ = Input::GetInstance();
	// ブレンダーみたいな表示線の関数初期化
	AxisIndicator::GetInstance()->SetVisible(true);
	AxisIndicator::GetInstance()->SetTargetCamera(&debugCamera_->GetCamera());


	//敵に自キャラのアドレスを渡す
	enemy_->SetPlayer(player_);

}

void GameScene::Update() {

#ifdef _DEBUG
	if (input_->TriggerKey(DIK_0)) {
		isDebugCameraActive_ = true;
	}
#endif // _DEBUG

	if (isDebugCameraActive_) {
		debugCamera_->Update();
		camera_.matView = debugCamera_->GetCamera().matView;
		camera_.matProjection = debugCamera_->GetCamera().matProjection;
		camera_.TransferMatrix();
	} else {
		camera_.UpdateMatrix();
	}

	player_->Updata();
	enemy_->Update();
	cameraController_->Update();
}

void GameScene::Draw() {
	Model::PreDraw();

	player_->Draw();
	enemy_->Draw();
	Model::PostDraw();
}

GameScene::~GameScene() {

	// ポインタのデリート
	delete player_;
	delete enemy_;
	// モデルのデリート
	delete playerModel_;
	delete enemyModel_;
	delete debugCamera_;
}

