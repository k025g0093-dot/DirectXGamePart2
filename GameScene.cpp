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
	skyDome_ = new SkyDome();

	cameraController_ = new CameraController();

	// プレイヤーのモデル生成
	playerModel_ = Model::CreateFromOBJ("player", true);
	enemyModel_ = Model::CreateFromOBJ("enemy", true);
	skyDomeModel_ = Model::CreateFromOBJ("skydome", true);

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

	skyDome_->Initialize(skyDomeModel_);

	cameraController_->Reset();

	// キー入力の初期化
	input_ = Input::GetInstance();
	// ブレンダーみたいな表示線の関数初期化
	AxisIndicator::GetInstance()->SetVisible(true);
	AxisIndicator::GetInstance()->SetTargetCamera(&debugCamera_->GetCamera());

	// 敵に自キャラのアドレスを渡す
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
	skyDome_->Update();

	player_->Updata();
	enemy_->Update();
	cameraController_->Update();

	CheckAllCollisions();
}

void GameScene::Draw() {
	Model::PreDraw();

	skyDome_->Draw(&camera_);

	player_->Draw();
	enemy_->Draw();
	Model::PostDraw();
}

GameScene::~GameScene() {



	// ポインタのデリート
	delete player_;
	delete enemy_;
	delete skyDome_;
	// モデルのデリート
	delete playerModel_;
	delete enemyModel_;
	delete skyDomeModel_;

	delete debugCamera_;
}

void GameScene::CheckAllCollisions() {

	Vector3 posA, posB;

	// プレイヤーの弾のリストの取得
	const std::list<playerBullet*>& playerBullets = player_->GetBullets();

	// プレイヤーの弾のリストの取得
	const std::list<EnemyBullet*>& enemyBullets = enemy_->GetBullets();

	// 当たり判定の実装
	posA = player_->GetWorldPosition();
	for (EnemyBullet* bullet : enemyBullets) {
		posB = bullet->GetWorldPosition();

		Vector3 diff = posA - posB;
		
		// 距離を計算
		float distance = sqrt(
			diff.x * diff.x + 
			diff.y * diff.y + 
			diff.z * diff.z
		);
		float playerRadius = 1.0f;
		float bulletRadius = 0.5f;

		if (distance < playerRadius + bulletRadius) {
			//地キャラの衝突判定
			player_->OnCollision();
			//敵弾の衝突判定のコールバック
			bullet->OnCollision();
		}
	}


	// 当たり判定の実装
	posA = enemy_->GetWorldPosition();
	for (playerBullet* bullet : playerBullets) {
		posB = bullet->GetWorldPosition();

		Vector3 diff = posA - posB;
		
		// 距離を計算
		float distance = sqrt(
			diff.x * diff.x + 
			diff.y * diff.y + 
			diff.z * diff.z
		);
		float playerRadius = 1.0f;
		float bulletRadius = 0.5f;

		if (distance < playerRadius + bulletRadius) {
			//地キャラの衝突判定
			enemy_->OnCollision();
			//敵弾の衝突判定のコールバック
			bullet->OnCollision();
		}
	}



}
