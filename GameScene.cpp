#include "GameScene.h"
#include "fstream"
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
	skyDome_ = new SkyDome();
	plane_ = new Plane();

	railCameraController_ = new RailCameraController();

	// プレイヤーのモデル生成
	TextureManager::Load("2DReticle.png");
	// ここのモデルは今後変更予定です、さらに名前も具ちゃってるのでそこも変更予定です
	playerModel_ = Model::CreateFromOBJ("player", true);
	model3DReticle_ = Model::CreateFromOBJ("deathParticle", true);

	enemyModel_ = Model::CreateFromOBJ("enemy", true);
	enemyBulletModel_ = Model::CreateFromOBJ("enemyBullets", true);

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
	player_->Get3DReticleModel(model3DReticle_);


	Vector3 enemyPosition = {0, 0, 10};

	Enemy* newEnemy = new Enemy();
	newEnemy->Initialize(enemyModel_, &railCameraController_->GetCamera(), {5, 0, 10});
	newEnemy->GetEnemyBulletModel(enemyBulletModel_);
	enemies_.push_back(newEnemy);

	skyDome_->Initialize(skyDomeModel_);
	plane_->Initialize(planeModel_);

	// キー入力の初期化
	input_ = Input::GetInstance();
	// ブレンダーみたいな表示線の関数初期化
	AxisIndicator::GetInstance()->SetVisible(true);
	AxisIndicator::GetInstance()->SetTargetCamera(&debugCamera_->GetCamera());

	LoadEnemyPopData();
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
	for (Enemy* enemy : enemies_) {
		enemy->Update();
		enemy->SetPlayer(player_);
		enemy->SetGameScene(this);
	}

	bullets_.remove_if([](EnemyBullet* bullet) {
		if (bullet->IsDead()) {
			delete bullet;
			return true;
		}
		return false;
	});

	for (EnemyBullet* bullet : bullets_) {
		bullet->Update();
	}

	UpdateEnemyPopCommands();
	CheckAllCollisions();
}

void GameScene::Draw() {

	Camera& activeCamera = railCameraController_->GetCamera();

	Model::PreDraw();

	plane_->Draw(&activeCamera);
	skyDome_->Draw(&activeCamera);
	for (EnemyBullet* bullet : bullets_) {
		bullet->Draw(&activeCamera);
	}

	player_->Draw();
	for (Enemy* enemy : enemies_) {
		enemy->Draw();
	}
	Model::PostDraw();

		// UIの描画
	Sprite::PreDraw();

	player_->DrawUI();

	Sprite::PostDraw();


}

GameScene::~GameScene() {

	// ポインタのデリート
	delete player_;
	for (Enemy* enemy : enemies_) {
		delete enemy;
	}
	delete skyDome_;
	delete plane_;
	// モデルのデリート
	delete playerModel_;
	delete enemyModel_;
	delete enemyBulletModel_;
	delete skyDomeModel_;
	delete planeModel_;
	delete debugCamera_;
}

void GameScene::CheckAllCollisions() {

	Vector3 posA, posB;

	// プレイヤーの弾のリストの取得
	const std::list<playerBullet*>& playerBullets = player_->GetBullets();

	// 敵の弾のリストの取得
	const std::list<EnemyBullet*>& enemyBullets = this->bullets_;

	// 当たり判定の実装
	posA = player_->GetWorldPosition();
	for (EnemyBullet* bullet : enemyBullets) {
		posB = bullet->GetWorldPosition();

		Vector3 diff = posA - posB;

		// 距離を計算
		float distance = sqrt(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);
		float playerRadius = 1.0f;
		float bulletRadius = 0.5f;

		if (distance < playerRadius + bulletRadius) {
			// 地キャラの衝突判定
			player_->OnCollision();
			// 敵弾の衝突判定のコールバック
			bullet->OnCollision();
		}
	}

	// 当たり判定の実装
	for (Enemy* enemy : enemies_) {
		posA = enemy->GetWorldPosition();
		for (playerBullet* bullet : playerBullets) {
			posB = bullet->GetWorldPosition();

			Vector3 diff = posA - posB;

			// 距離を計算
			float distance = sqrt(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);
			float playerRadius = 1.0f;
			float bulletRadius = 0.5f;

			if (distance < playerRadius + bulletRadius) {
				// 地キャラの衝突判定
				enemy->OnCollision();
				// 敵弾の衝突判定のコールバック
				bullet->OnCollision();
			}
		}
	}

	for (playerBullet* pBullet : playerBullets) {
		posA = pBullet->GetWorldPosition();
		for (EnemyBullet* eBullet : enemyBullets) {
			posB = eBullet->GetWorldPosition();

			Vector3 diff = posA - posB;

			// 距離を計算
			float distance = sqrt(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);
			float playerRadius = 1.0f;
			float bulletRadius = 0.5f;

			if (distance < playerRadius + bulletRadius) {
				// 地キャラの衝突判定
				eBullet->OnCollision();
				// 敵弾の衝突判定のコールバック
				pBullet->OnCollision();
			}
		}
	}
}

void GameScene::AddEnemyBullet(EnemyBullet* bullet) { bullets_.push_back(bullet); }

// ファイル読み込み関数
void GameScene::LoadEnemyPopData() {
	std::ifstream file;
	file.open("Resources/enemyPopData.csv");
#ifdef _DEBUG
	assert(file.is_open());
#endif // DEBUG
	enemyPopCommands << file.rdbuf();
	file.close();
}

void GameScene::UpdateEnemyPopCommands() {

	//待機処理
	if (isPopEnemy_) {
		//時間を減らす
		popEnemyWaitTime_--;
		if (popEnemyWaitTime_ <= 0) {
			isPopEnemy_ = false;
		} else {
			return;
		}
	}

	std::string line;

	while (std::getline(enemyPopCommands, line)) {
		// 処理を追加
		std::istringstream line_stream(line);

		std::string word;
		std::getline(line_stream, word, ',');

		// “//”これをコメントとして飛ばすように設定
		if (word.find("//") == 0)
			continue;

		if (word.find("POP") == 0) {

			// X座標読み取り
			std::getline(line_stream, word, (','));
			float x = (float)std::atof(word.c_str());

			// X座標読み取り
			std::getline(line_stream, word, (','));
			float y = (float)std::atof(word.c_str());

			// X座標読み取り
			std::getline(line_stream, word, (','));
			float z = (float)std::atof(word.c_str());

			SpawnEnemy({x, y, z});
		} else if (word.find("WAIT") == 0) {
			// 待機処理を追加する場合はここに記述
			std::getline(line_stream, word, (','));

			//待ち時間
			int32_t waitTime = atoi(word.c_str());

			isPopEnemy_ = true;
			popEnemyWaitTime_ = waitTime;

			break;
		}
	}
}

void GameScene::SpawnEnemy(const KamataEngine::Vector3& position) {
	Enemy* enemy = new Enemy();
	enemy->Initialize(enemyModel_, &railCameraController_->GetCamera(), position);
	enemy->GetEnemyBulletModel(enemyBulletModel_);
	enemies_.push_back(enemy);

}