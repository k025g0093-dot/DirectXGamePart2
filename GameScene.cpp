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
	plane_ = new Plane();

	railCameraController_ = new RailCameraController();

	// プレイヤーのモデル生成
	playerModel_ = Model::CreateFromOBJ("player", true);
	enemyModel_ = Model::CreateFromOBJ("enemy", true);
	skyDomeModel_ = Model::CreateFromOBJ("skydome", true);
	planeModel_ = Model::CreateFromOBJ("plane", true);

	debugCamera_ = new DebugCamera(1280, 720);

#pragma region カメラコントローラーの設定
	railCameraController_->Initialize();
	railCameraController_->SetTarget(player_);
	// ここで距離の微調整が可能

	PrimitiveDrawer::GetInstance()->SetCamera(&railCameraController_->GetCamera());
#pragma endregion

	Vector3 playerPosition = {0, 5, 20};
	player_->Initialize(playerModel_, &railCameraController_->GetCamera(), playerPosition);
	player_->SetParent(&railCameraController_->GetWorldTransform());


	Vector3 enemyPosition = {0, 0, 10};
	enemy_->Initialize(enemyModel_, &railCameraController_->GetCamera(), enemyPosition);

	skyDome_->Initialize(skyDomeModel_);
	plane_->Initialize(planeModel_);


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


	railCameraController_->Update();
	skyDome_->Update();
	plane_->Update();

	player_->Updata();
	enemy_->Update();

	CheckAllCollisions();
}

void GameScene::Draw() {

	Camera& activeCamera = railCameraController_->GetCamera();

	Model::PreDraw();

	plane_->Draw(&activeCamera);
	skyDome_->Draw(&activeCamera);
	
	player_->Draw();
	enemy_->Draw();
	Model::PostDraw();
}

GameScene::~GameScene() {



	// ポインタのデリート
	delete player_;
	delete enemy_;
	delete skyDome_;
	delete plane_;
	// モデルのデリート
	delete playerModel_;
	delete enemyModel_;
	delete skyDomeModel_;
	delete planeModel_;
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
