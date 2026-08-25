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
	model3DReticle_ = Model::CreateFromOBJ("3DReticle", true);
	deathParticleModel_ = Model::CreateFromOBJ("deathParticle", true);

	enemyModel_ = Model::CreateFromOBJ("enemy", true);
	enemyBulletModel_ = Model::CreateFromOBJ("enemyBullets", true);
	playerBulletModel_ = Model::CreateFromOBJ("playerBullets", true);

	skyDomeModel_ = Model::CreateFromOBJ("skydome", true);
	planeModel_ = Model::CreateFromOBJ("ground", true);
	shieldEnemyModel_ = Model::CreateFromOBJ("shieldEnemy", true);
	allyModel_ = Model::CreateFromOBJ("ally", true);

	// タイプ別モデルの割り当て（今後追加分岐用）
	SetEnemyTypeModel(EnemyType::kTank, shieldEnemyModel_);

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
	player_->SetBulletModel(playerBulletModel_);

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

	// ポーズ画面用オーバーレイ
	pauseTextureHandle_ = TextureManager::Load("white1x1.png");
	pauseOverlay_ = Sprite::Create(pauseTextureHandle_, {0, 0});
	pauseOverlay_->SetSize(Vector2(1280, 720));
	pauseOverlay_->SetColor(Vector4(0, 0, 0, 0.6f));

	// HP表示
	uiTexture_ = TextureManager::Load("white1x1.png");
	hpBarBg_ = Sprite::Create(uiTexture_, {0, 0});
	hpBarBg_->SetSize(Vector2(204, 24));
	hpBarBg_->SetPosition(Vector2(20, 20));
	hpBarBg_->SetColor(Vector4(0.2f, 0.2f, 0.2f, 0.8f));

	hpBarFill_ = Sprite::Create(uiTexture_, {0, 0});
	hpBarFill_->SetSize(Vector2(200, 20));
	hpBarFill_->SetPosition(Vector2(22, 22));
	hpBarFill_->SetColor(Vector4(0.2f, 0.9f, 0.2f, 1.0f));

	// チュートリアルメッセージ（画面下部に表示）
	tutorialMsg_ = Sprite::Create(uiTexture_, {0, 0});
	tutorialMsg_->SetSize(Vector2(600, 40));
	tutorialMsg_->SetPosition(Vector2(340, 650));
	tutorialMsg_->SetColor(Vector4(0.0f, 0.0f, 0.0f, 0.7f));

	LoadEnemyPopData();
	fade_->Start(Fade::Status::FadeIn, 1.0f);
}

void GameScene::Update() {

	fade_->Update();
	XINPUT_STATE joyState{};
	bool pauseTrigger = false;
	bool currentStart = false;

	switch (gamePhase_) {
	case GamePhase::kFadeIn:
		if (!fade_->IsFinished()) {
			gamePhase_ = GamePhase::kPlay;
		}
		break;

	case GamePhase::kPlay: {

		// ポーズ切り替え（ESC or スタートボタンの押した瞬間だけ反応）
		pauseTrigger = input_->TriggerKey(DIK_ESCAPE);

		if (input_->GetJoystickState(0, joyState)) {
			currentStart = (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_START) != 0;
			if (currentStart && !prevStartButton_) {
				pauseTrigger = true;
			}
			prevStartButton_ = currentStart;
		} else {
			prevStartButton_ = false;
		}

		if (pauseTrigger) {
			isPaused_ = !isPaused_;
		}

		// ポーズ中はゲーム更新を止める
		if (!isPaused_) {
			GameUpdate();
		}

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

	// 初回の弱体化時にチュートリアル表示
	if (!tutorialShown_) {
		for (Enemy* enemy : enemies_) {
			if (enemy->IsWeakened()) {
				tutorialShown_ = true;
				tutorialTimer_ = 300; // 5秒間表示
				break;
			}
		}
	}
	if (tutorialTimer_ > 0) {
		tutorialTimer_--;
	}

	// 仲間の更新（ターゲットはロックオン対象。敵の削除前なので参照が安全）
	for (Ally* ally : allies_) {
		ally->SetTarget(lockOn_->GetTarget());
		ally->Updata();
	}

	enemies_.remove_if([this](Enemy* enemy) {
		if (enemy->IsDead()) {
			// 通常死亡時のみデスパーティクルを生成
			for (int i = 0; i < 8; i++) {
				DeathParticle* p = new DeathParticle();
				p->Initialize(deathParticleModel_, &railCameraController_->GetCamera(), enemy->GetWorldPosition());
				deathParticles_.push_back(p);
			}
			delete enemy;
			return true;
		}
		// 味方化イージング完了時に味方をスポーンして敵を削除
		if (enemy->IsConversionDone()) {
			SpawnAlly(enemy->GetWorldPosition(), allyModel_);
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

	// デスパーティクル更新
	for (DeathParticle* p : deathParticles_) {
		p->Update();
	}
	deathParticles_.remove_if([](DeathParticle* p) {
		if (p->IsDead()) {
			delete p;
			return true;
		}
		return false;
	});

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
	for (DeathParticle* p : deathParticles_) {
		p->Draw();
	}
	Model::PostDraw();

	// UIの描画
	Sprite::PreDraw();

	// HPバー更新・描画
	if (player_) {
		float hpRatio = (float)player_->GetHp() / (float)player_->GetMaxHp();
		hpBarFill_->SetSize(Vector2(200.0f * hpRatio, 20));
		// HP低いと赤、高いと緑
		float r = (1.0f - hpRatio) * 0.9f;
		float g = hpRatio * 0.9f;
		hpBarFill_->SetColor(Vector4(r, g, 0.2f, 1.0f));
		hpBarBg_->Draw();
		hpBarFill_->Draw();
	}

	// player_->DrawUI();
	lockOn_->Draw();

	// ポーズ中は暗転オーバーレイを描画
	if (isPaused_) {
		pauseOverlay_->Draw();
	}

	// チュートリアルメッセージ
	if (tutorialTimer_ > 0) {
		tutorialMsg_->Draw();
	}

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
	for (DeathParticle* p : deathParticles_) {
		delete p;
	}
	delete skyDome_;
	delete plane_;
	// モデルのデリート
	delete playerModel_;
	delete enemyModel_;
	delete enemyBulletModel_;
	delete playerBulletModel_;
	delete skyDomeModel_;
	delete planeModel_;
	delete shieldEnemyModel_;
	delete allyModel_;
	delete model3DReticle_;
	delete deathParticleModel_;
	delete debugCamera_;
	delete pauseOverlay_;
	delete hpBarBg_;
	delete hpBarFill_;
	delete tutorialMsg_;
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
				// 弱体化した敵をロックオン中に撃つと味方化イージング開始
				if (enemy->IsWeakened() && enemy == lockOn_->GetTarget() && !enemy->IsConverting() && allies_.size() < (size_t)kMaxAllyCount) {
					enemy->StartConversion(player_->GetWorldPosition());
				} else if (!enemy->IsConverting()) {
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
				if (!enemy->IsConverting()) {
					enemy->OnCollision();
				}
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

	// タイプ別モデルがあればそれ、なければデフォルト
	int typeIndex = (int)type;
	Model* useModel = (typeIndex < kEnemyTypeCount && enemyTypeModels_[typeIndex]) ? enemyTypeModels_[typeIndex] : enemyModel_;

	enemy->Initialize(useModel, &railCameraController_->GetCamera(), position);
	enemy->GetEnemyBulletModel(enemyBulletModel_);
	enemies_.push_back(enemy);
}

void GameScene::SpawnAlly(const KamataEngine::Vector3& position, KamataEngine::Model* model) {
	Ally* ally = new Ally();

	ally->Initialize(model, &railCameraController_->GetCamera(), position, (int32_t)allies_.size());
	ally->SetPlayer(player_);
	ally->SetGameScene(this);
	ally->SetBulletModel(enemyBulletModel_);
	allies_.push_back(ally);
}

void GameScene::AddAllyBullet(playerBullet* bullet) { allyBullets_.push_back(bullet); }