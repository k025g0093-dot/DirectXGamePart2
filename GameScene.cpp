#include "GameScene.h"
#include "MyMath.h"
#include <cassert>
//===================================================
// パブリックの処理
//===================================================

using namespace KamataEngine;

// コンストラクタ

// 初期化処理
void GameScene::Initialize() {

	// ゲームフェーズから開始
	phase_ = Phase::kPlay;

#pragma region 基礎システムの初期化
	worldTransform_.Initialize();
	camera_.Initialize();
	camera_.farZ = 2000;
#pragma endregion

#pragma region インスタンスの生成(new)
	player_ = new Player();
	debugCamera_ = new DebugCamera(1280, 720);
	SkyDome_ = new SkyDome();
	mapChipField_ = new MapChipField();
	cameraController_ = new CameraController();
	deathParticles_ = new DeathParticles();
	fade_ = new Fade();
#pragma endregion

#pragma region モデルの読み込み
	modelMap_ = Model::CreateFromOBJ("block", true);         // マップのモデル
	modelSkydome_ = Model::CreateFromOBJ("skydome", true);   // スカイドームのモデル
	modelPlayer_ = Model::CreateFromOBJ("player", true);     // プレイヤーのモデル
	modelAttack_ = Model::CreateFromOBJ("hit_effect", true); // プレイヤーの攻撃モデル

	modelEnemy_ = Model::CreateFromOBJ("enemy", true);             // 敵のモデル
	modelshieldEnemy_ = Model::CreateFromOBJ("shieldEnemy", true); // シールドを持った敵

	modelParticl_ = Model::CreateFromOBJ("player", true); // 仮モデルでプレイヤーのモデルを使用
	modelHitEffect = Model::CreateFromOBJ("particle", true);
	modelGuardEffect_ = Model::CreateFromOBJ("particle", true);

	// 描画用ポインタへの代入
	assert(modelMap_);
	model_ = modelMap_;
#pragma endregion

#pragma region カメラコントローラーの設定
	cameraController_->Initialize();
	cameraController_->SetTarget(player_);

	// ここで距離の微調整が可能
	cameraController_->targetOffset_ = {0.0f, 0.0f, -20.0f};
	cameraController_->SetMovableArea({11, 100, 6, 100}); // カメラの移動できる最大値、最少値
	cameraController_->Reset();

	// ライン描画用カメラをコントローラー側に同期
	PrimitiveDrawer::GetInstance()->SetCamera(&cameraController_->GetCamera());
#pragma endregion

#pragma region マップ・オブジェクトの初期化
	// マップチップ読み込み
	mapChipField_->LoadMapChipCsv("./Resources/map.csv");
	GenerateBlocks();

	fade_->Initialize();

	// プレイヤー初期化 (座標計算とカメラの紐付け)
	Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(2, 15);
	maxEnemyCount = 3;
	for (int32_t i = 0; i < maxEnemyCount; i++) {
		// 1. 毎回新しいメモリを確保する
		Enemy* newEnemy = new Enemy();

		// 2. 座標が重ならないように計算（例：X軸方向に5ずつずらす）
		Vector3 enemyPosition = mapChipField_->GetMapChipPositionByIndex(17 + i*2, 14 + i * 2);

		// 3. 新しく作った個体に対して初期化
		newEnemy->Initialize(modelEnemy_, &cameraController_->GetCamera(), enemyPosition);
		newEnemy->SetGameScene(this);
		// 4. vectorに追加
		enemyis_.push_back(newEnemy);
	}

	for (int32_t i = 0; i < maxEnemyCount; i++) {
		// 1. 毎回新しいメモリを確保する
		ShieldEnemy* newshieldEnemy = new ShieldEnemy();

		// 2. 座標が重ならないように計算（例：X軸方向に5ずつずらす）
		Vector3 enemyPosition = mapChipField_->GetMapChipPositionByIndex(20 + i*2, 14 + i*2);

		// 3. 新しく作った個体に対して初期化
		newshieldEnemy->Initialize(modelshieldEnemy_, &cameraController_->GetCamera(), enemyPosition);
		newshieldEnemy->SetGameScene(this);
		// 4. vectorに追加
		shieldEnemyis_.push_back(newshieldEnemy);
	}

	// playerの初期化とセッターによる情報の受け渡しw
	player_->Initialize(modelPlayer_, modelAttack_, &cameraController_->GetCamera(), playerPosition);
	player_->SetMapChipFiled(mapChipField_); // マップのデータをプレイヤーに渡す

	// やられたときに出るパーティクル（仮として現在プレイヤーのモデルが入っている
	deathParticles_->Initialize(modelParticl_, &cameraController_->GetCamera(), playerPosition);

	// スカイドーム初期化
	SkyDome_->Initialize(modelSkydome_);
#pragma endregion

	// ヒットエフェクトのセッター
	HitEffect::SetModel(modelHitEffect);
	HitEffect::SetCamera(&cameraController_->GetCamera());

	GuardEffect::SetModel(modelGuardEffect_);
	GuardEffect::SetCamera(&cameraController_->GetCamera());


	fade_->Start(Fade::Status::FadeIn, 1.0f);
}

// 更新処理
void GameScene::Update() {
	fade_->Update();
	switch (phase_) {
	case Phase::kFadeIn:

		// メイン待機中：スペースが押されたらフェードアウト開始
		if (fade_->IsFinished()) {
			phase_ = Phase::kPlay; // 次の状態へ
		}
		break;

	case Phase::kPlay:

		// ゲーム中の処理
		PlayeUpdate();

		break;
	case Phase::kDeath:

		// ゲームオーバ中の処理
		DeathUpdate();

		// 右端の処理：パーティクルもフェードも終わったらシーン終了
		if (deathParticles_->IsFinished() && fade_->IsFinished()) {

			fade_->Start(Fade::Status::FadeOut, 1.0f);
			phase_ = Phase::kFadeOut;
		}

		break;
	case Phase::kFadeOut:
		if (fade_->IsFinished()) {
			finished_ = true;
		}

		break;

	default:
		break;
	}
}

// 描画処理
void GameScene::Draw() {

	// コントローラー側のカメラを取得
	Camera& activeCamera = cameraController_->GetCamera();

	// 描画の開始位置
	Model::PreDraw();

#pragma region 背景描画

	SkyDome_->Draw(&activeCamera);
#pragma endregion

#pragma region キャラクター描画
	if (!player_->isDead_) {

		player_->Draw(); // 内部で保持しているカメラを使用
	}
	for (Enemy* enemy : enemyis_) {
		enemy->Draw();
	}

	for (ShieldEnemy* shieldEnemy : shieldEnemyis_) {
		shieldEnemy->Draw();
	}

	for (HitEffect* hiteffect : hitEffects_) {
		if (!hiteffect->isDead_) { // ← 追加（生きてるやつだけ描画）
			hiteffect->Draw();
		}
	}

	for (GuardEffect* guardEffect : guardEffects_) {
		if (!guardEffect->isDead_) { // ← 追加（生きてるやつだけ描画）
			guardEffect->Draw();
		}
	}



	if (!deathParticles_->isFinished_) {
		deathParticles_->Draw();
	}

#pragma endregion

#pragma region マップ描画
	for (std::vector<WorldTransform*>& worldTransformBlockRow : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockRow) {
			if (worldTransformBlock) {
				model_->Draw(*worldTransformBlock, activeCamera);
			}
		}
	}
#pragma endregion

	fade_->Draw();

	// 描画の終了位置
	Model::PostDraw();
}

#pragma region デストラクタ
// デストラクタ（解放エクササイズ）
GameScene::~GameScene() {
	delete modelMap_; // model_ は modelMap_ の別名なので modelMap_ だけ delete
	delete modelSkydome_;
	delete modelPlayer_;  // 追加
	delete modelAttack_;  // 追加
	delete modelEnemy_;   // 追加
	delete modelParticl_; // 追加
	delete player_;
	delete debugCamera_;
	delete mapChipField_;
	delete deathParticles_;
	delete cameraController_;
	delete SkyDome_;
	delete fade_;
	delete hitEffect_;
	delete guardEffect_;

	for (std::vector<WorldTransform*>& worldTransformBlockRow : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockRow) {
			delete worldTransformBlock;
		}
	}
	for (Enemy* enemy : enemyis_) {
		delete enemy;
	}

	for (ShieldEnemy* shieldEnemy : shieldEnemyis_) {
		delete shieldEnemy;
	}

	worldTransformBlocks_.clear();
}

#pragma endregion
//===================================================
// プライベート内の処理
//===================================================

#pragma region ブロックに関するもの
void GameScene::GenerateBlocks() {
	// 要素数
	uint32_t numBlockVirtical = mapChipField_->GetNumBlockVirtical();
	uint32_t mumBlockHorizontal = mapChipField_->GetNumBlockHorizontal();

	// 要素の変更
	worldTransformBlocks_.resize(numBlockVirtical);
	for (uint32_t i = 0; i < numBlockVirtical; i++) {
		worldTransformBlocks_[i].resize(mumBlockHorizontal); // 各列に「行」を確保する
	}

	// キューブ生成
	// GameScene.cpp 修正後
	for (uint32_t j = 0; j < numBlockVirtical; j++) {
		for (uint32_t i = 0; i < mumBlockHorizontal; i++) {

			// 引数の順番を (i, j) に修正！
			if (mapChipField_->GetMapChipTypeByIndex(i, j) == MapChipType::kBlock) {

				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();

				worldTransformBlocks_[j][i] = worldTransform;
				// ここは (i, j) で合っています
				worldTransformBlocks_[j][i]->translation_ = mapChipField_->GetMapChipPositionByIndex(i, j);
			}
		}
	}
}
#pragma endregion

#pragma region 全体の当たり判定
void GameScene::CheckAllCollisions() {

	AABB aabb1, aabb2;

	aabb1 = player_->GetAABB();
	for (Enemy* enemy : enemyis_) {

		if (enemy->IsCollisionDisabled())
			continue;

		aabb2 = enemy->GetAABB();

		if (IsCollision(aabb1, aabb2)) {
			player_->EnemyOnCollsion(enemy);
			enemy->OnCollsion(player_);
		}
	}

	for (ShieldEnemy* shieldEnemy : shieldEnemyis_) {
		if (shieldEnemy->IsCollisionDisabled())
			continue;

		aabb2 = shieldEnemy->GetAABB();

		if (IsCollision(aabb1, aabb2)) {
			player_->ShieldEnemyOnCollsion(shieldEnemy);
			shieldEnemy->OnCollsion(player_);
		}
	}
}
#pragma endregion

#pragma region ゲームプレ中の処理
// ゲームプレイ中のアップデート
void GameScene::PlayeUpdate() {
#ifdef _DEBUG

	if (Input::GetInstance()->TriggerKey(DIK_R)) {
		isDebugCamera_ = true;
	}

#endif // DEBUG
	if (isDebugCamera_) {
		// デバックカメラの更新処理
		debugCamera_->Update();

		camera_.matView = debugCamera_->GetCamera().matView;
		camera_.matProjection = debugCamera_->GetCamera().matProjection;
		camera_.TransferMatrix();
	} else {
		camera_.UpdateMatrix();
	}

	// 1. プレイヤーの更新（まずプレイヤーが動く）
	player_->Updata();
	for (Enemy* enemy : enemyis_) {
		enemy->Update();
	}

	for (ShieldEnemy* shieldEnemy : shieldEnemyis_) {
		shieldEnemy->Update();
	}

	for (HitEffect* hiteffect : hitEffects_) {
		hiteffect->Update();
	}


	for (GuardEffect* guardEffect : guardEffects_) {
		guardEffect->Update();
	}

	


	// 2. カメラコントローラーの更新（動いたプレイヤーをカメラが追いかける）
	// ★これが抜けているので追加してください
	cameraController_->Update();

	SkyDome_->Update();

	// ブロックの更新
	for (std::vector<WorldTransform*>& worldTransformBlockRow : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockRow) {

			if (!worldTransformBlock) {
				continue;
			}

			// 4. アフィン変換行列の合成 (Scale * Rotate * Translate)
			worldTransformBlock->matWorld_ = MakeAffineMatrix(worldTransformBlock->scale_, worldTransformBlock->rotation_, worldTransformBlock->translation_);

			// 5. 行列を定数バッファに転送
			worldTransformBlock->TransferMatrix();
		}
	}

	CheckAllCollisions();

	if (player_->isDead_) {

		phase_ = Phase::kDeath;

		deathParticles_->Initialize(modelParticl_, &cameraController_->GetCamera(), player_->GetWorldPodition());
	}

	enemyis_.remove_if([](Enemy* enemy) {
		if (enemy->isDead_) {
			delete enemy;
			return true;
		}
		return false;
	});


	shieldEnemyis_.remove_if([](ShieldEnemy* shieldEnemy) {
		if (shieldEnemy->isDead_) {
			delete shieldEnemy;
			return true;
		}
		return false;
	});


	hitEffects_.remove_if([](HitEffect* hiteffect) {
		if (hiteffect->isDead_) {
			delete hiteffect;
			return true;
		}
		return false;
	});


	guardEffects_.remove_if([](GuardEffect* guardEffect) {
		if (guardEffect->isDead_) {
			delete guardEffect;
			return true;
		}
		return false;
	});
}
#pragma endregion

#pragma region プレイヤーの死亡中の処理
// デス演出更新処理
void GameScene::DeathUpdate() {

	for (Enemy* enemy : enemyis_) {
		enemy->Update();
	}

	for (ShieldEnemy* shieldEnemy : shieldEnemyis_) {
		shieldEnemy->Update();
	}

	// スカイドームの更新処理
	SkyDome_->Update();

	// パーティクルの処理
	if (!deathParticles_->isFinished_) {
		deathParticles_->Update();
	}

	// ブロックの更新
	for (std::vector<WorldTransform*>& worldTransformBlockRow : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockRow) {

			if (!worldTransformBlock) {
				continue;
			}

			// 4. アフィン変換行列の合成 (Scale * Rotate * Translate)
			worldTransformBlock->matWorld_ = MakeAffineMatrix(worldTransformBlock->scale_, worldTransformBlock->rotation_, worldTransformBlock->translation_);

			// 5. 行列を定数バッファに転送
			worldTransformBlock->TransferMatrix();
		}
	}
}
#pragma endregion

void GameScene::CreateHitEffect(const Vector3 postion) {

	HitEffect* newHitEffect = HitEffect::Create(postion);
	hitEffects_.push_back(newHitEffect);
}

void GameScene::CreateGuardEffect(const Vector3 postion) {

	GuardEffect* newGuardEffect = GuardEffect::Create(postion);
	guardEffects_.push_back(newGuardEffect);
}