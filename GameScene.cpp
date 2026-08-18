#include "GameScene.h"
#include "fstream"
using namespace KamataEngine;

GameScene::GameScene() { Initialize(); }

void GameScene::Initialize() {

	gamePhase_ = GamePhase::kFadeIn;

#pragma region 基礎システムの初期化
	worldTransform_.Initialize();
	camera_.Initialize();
	camera_.translation_ = {0, 0, -20.0f};
	camera_.UpdateMatrix();
#pragma endregion

#pragma region インスタン生成(new)
	// プレイヤーのインスタンス
	player_ = new Player();
	skyDome_ = new SkyDome();
	plane_ = new Plane();
	lockOn_ = new LockOn();
	fade_ = new Fade();
	railCameraController_ = new RailCameraController();
	debugCamera_ = new DebugCamera(1280, 720);

#pragma endregion

	// プレイヤーのモデル生成
	TextureManager::Load("2DReticle.png");
	// ここのモデルは今後変更予定です、さらに名前も具ちゃってるのでそこも変更予定です
	playerModel_ = Model::CreateFromOBJ("player", true);
	model3DReticle_ = Model::CreateFromOBJ("deathParticle", true);

	enemyModel_ = Model::CreateFromOBJ("enemy", true);
	enemyBulletModel_ = Model::CreateFromOBJ("enemyBullets", true);

	skyDomeModel_ = Model::CreateFromOBJ("skydome", true);
	planeModel_ = Model::CreateFromOBJ("plane", true);

#pragma region カメラコントローラーの設定
	railCameraController_->Initialize();
	railCameraController_->SetTarget(player_);
	// ここで距離の微調整が可能

	PrimitiveDrawer::GetInstance()->SetCamera(&railCameraController_->GetCamera());
#pragma endregion

#pragma region キャラクターの初期化

	Vector3 playerPosition = {0, 5, 20};
	player_->Initialize(playerModel_, &railCameraController_->GetCamera(), playerPosition);
	player_->SetParent(&railCameraController_->GetWorldTransform());
	player_->Get3DReticleModel(model3DReticle_);
	player_->SetLockOn(lockOn_);

	// 最初の1体も SpawnEnemy 経由で出すことで、参照のセット漏れをなくす
	SpawnEnemy(EnemyType::kNormal, {5, 0, 10});

#pragma endregion

#pragma region 各オブジェクトの初期化

	skyDome_->Initialize(skyDomeModel_);
	plane_->Initialize(planeModel_);
	fade_->Initialize();
	lockOn_->Initialize();
#pragma endregion

	// キー入力の初期化
	input_ = Input::GetInstance();
	// ブレンダーみたいな表示線の関数初期化
	AxisIndicator::GetInstance()->SetVisible(true);
	AxisIndicator::GetInstance()->SetTargetCamera(&debugCamera_->GetCamera());

	LoadEnemyPopData();
	fade_->Start(Fade::Status::FadeIn, 1.0f);
}

void GameScene::Update() {

	fade_->Update();
	switch (gamePhase_) {
	case GamePhase::kFadeIn:
		if (!fade_->IsFinished()) {
			gamePhase_ = GamePhase::kPlay;
		}
		break;

	case GamePhase::kPlay:

		// ゲーム中の処理
		GameUpdate();

		if (player_->IsDead()) {
			fade_->Start(Fade::Status::FadeOut, 1.0f);
			gamePhase_ = GamePhase::kFadeOut;
		} else if (isGoalReached_ && enemies_.empty()) {
			isGameClear_ = true;
			fade_->Start(Fade::Status::FadeOut, 1.0f);
			gamePhase_ = GamePhase::kFadeOut;
		}

		break;

	case GamePhase::kFadeOut:
		if (fade_->IsFinished()) {
			finished_ = true;
		}

		break;
	default:
		break;
	}
}

void GameScene::GameUpdate() {
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

	lockOn_->Update(player_, enemies_, railCameraController_->GetCamera());

	player_->Updata();

	// Player と GameScene の参照は SpawnEnemy 側で渡しているのでここでは更新処理だけ
	for (Enemy* enemy : enemies_) {
		enemy->Update();
	}

	// 仲間の更新（ターゲットはロックオン対象。敵の削除前なので参照が安全）
	for (Ally* ally : allies_) {
		ally->SetTarget(lockOn_->GetTarget());
		ally->Updata();
	}

	enemies_.remove_if([](Enemy* enemy) {
		if (enemy->IsDead()) {
			delete enemy;
			return true;
		}
		return false;
	});

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

	allyBullets_.remove_if([](playerBullet* bullet) {
		if (bullet->IsDead()) {
			delete bullet;
			return true;
		}
		return false;
	});

	for (playerBullet* bullet : allyBullets_) {
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
	for (Ally* ally : allies_) {
		ally->Draw();
	}
	for (playerBullet* bullet : allyBullets_) {
		bullet->Draw(&activeCamera);
	}
	Model::PostDraw();

	// UIの描画
	Sprite::PreDraw();

	// player_->DrawUI();
	lockOn_->Draw();

	Sprite::PostDraw();
}

GameScene::~GameScene() {

	// ポインタのデリート
	delete player_;
	for (Enemy* enemy : enemies_) {
		delete enemy;
	}
	for (Ally* ally : allies_) {
		delete ally;
	}
	for (playerBullet* bullet : allyBullets_) {
		delete bullet;
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

	// プレイヤーと敵本体の当たり判定（突進タイプの体当たり用）
	posA = player_->GetWorldPosition();
	for (Enemy* enemy : enemies_) {
		posB = enemy->GetWorldPosition();

		Vector3 diff = posA - posB;

		// 距離を計算
		float distance = sqrt(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);
		float playerRadius = 1.0f;
		float enemyRadius = enemy->GetCollisionRadius();

		if (distance < playerRadius + enemyRadius) {
			player_->OnCollision();
			enemy->OnCollision();
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
			float EnemyRadius = enemy->GetCollisionRadius(); // 固定1.0fを置き換え
			float bulletRadius = 0.5f;

			if (distance < EnemyRadius + bulletRadius) {
				// 弱体化した敵をロックオン中に撃つと仲間化する
				if (enemy->IsWeakened() && enemy == lockOn_->GetTarget() && allies_.size() < (size_t)kMaxAllyCount) {
					SpawnAlly(enemy->GetWorldPosition());
					enemy->isDead_ = true;
				} else {
					// 地キャラの衝突判定
					enemy->OnCollision();
				}
				// 敵弾の衝突判定のコールバック
				bullet->OnCollision();
			}
		}
	}

	// 仲間の弾 vs 敵（仲間の弾では仲間化はしない。通常ダメージのみ）
	for (Enemy* enemy : enemies_) {
		posA = enemy->GetWorldPosition();
		for (playerBullet* bullet : allyBullets_) {
			posB = bullet->GetWorldPosition();

			Vector3 diff = posA - posB;

			// 距離を計算
			float distance = sqrt(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);
			float EnemyRadius = enemy->GetCollisionRadius();
			float bulletRadius = 0.5f;

			if (distance < EnemyRadius + bulletRadius) {
				enemy->OnCollision();
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

	// 待機処理
	if (isPopEnemy_) {
		// 時間を減らす
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

			// 敵タイプの読み取り
			std::getline(line_stream, word, (','));
			int32_t typeId = std::atoi(word.c_str());
			EnemyType type = static_cast<EnemyType>(typeId);

			// X座標読み取り
			std::getline(line_stream, word, (','));
			float x = (float)std::atof(word.c_str());

			// X座標読み取り
			std::getline(line_stream, word, (','));
			float y = (float)std::atof(word.c_str());

			// X座標読み取り
			std::getline(line_stream, word, (','));
			float z = (float)std::atof(word.c_str());

			SpawnEnemy(type, {x, y, z});
		} else if (word.find("WAIT") == 0) {
			// 待機処理を追加する場合はここに記述
			std::getline(line_stream, word, (','));

			// 待ち時間
			int32_t waitTime = atoi(word.c_str());

			isPopEnemy_ = true;
			popEnemyWaitTime_ = waitTime;

			break;
		} else if (word.find("GOAL") == 0) {
			// ゴール到達
			isGoalReached_ = true;

			break;
		}
	}
}

void GameScene::SpawnEnemy(EnemyType type, const KamataEngine::Vector3& position) {
	Enemy* enemy = new Enemy();

	// Initialize の中で難易度やプレイヤー位置を参照するので、必ず先に渡しておく
	enemy->SetGameScene(this);
	enemy->SetPlayer(player_);
	enemy->SetEnemyType(type);

	enemy->Initialize(enemyModel_, &railCameraController_->GetCamera(), position);
	enemy->GetEnemyBulletModel(enemyBulletModel_);
	enemies_.push_back(enemy);
}

void GameScene::SpawnAlly(const KamataEngine::Vector3& position) {
	Ally* ally = new Ally();

	ally->Initialize(playerModel_, &railCameraController_->GetCamera(), position, (int32_t)allies_.size());
	ally->SetPlayer(player_);
	ally->SetGameScene(this);
	ally->SetBulletModel(playerModel_);
	allies_.push_back(ally);
}

void GameScene::AddAllyBullet(playerBullet* bullet) { allyBullets_.push_back(bullet); }