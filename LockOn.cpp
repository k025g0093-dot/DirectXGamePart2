#include "LockOn.h"
#include "Enemy.h"
#include "Player.h"

using namespace KamataEngine;

void LockOn::Initialize() {
	uint32_t lockOnSprite = TextureManager::Load("2DReticle.png");
	lockOnSprite_ = Sprite::Create(lockOnSprite, {0, 0}, {1, 1, 1, 1}, {0.5f, 0.5f});

}	

void LockOn::Update(Player* player, std::list<Enemy*>& enemies, const KamataEngine::Camera& camera) { 
	
	//ターゲットのリスト作成
	std::list<std::pair<float, Enemy*>> targets;


	Vector3 playerPosition = player->GetWorldPosition();
	
	Vector3 playerScreen = Project(
		playerPosition, 0.0f, 0.0f,
		WinApp::kWindowWidth,
		WinApp::kWindowHeight,
		camera.matView,
		camera.matProjection
	);
	isLockedOn_ = false;
	for (Enemy* enemy : enemies) {
	
		//敵のワールド座標の取得
		Vector3 positionWorld = enemy->GetWorldPosition();

		if (enemy->GetWorldPosition().z < player->GetWorldPosition().z) {
			continue; // プレイヤーより手前にいる敵は無視
		}

		Vector3 positionScreen = Project(
			positionWorld, 0.0f, 0.0f, 
			WinApp::kWindowWidth, WinApp::kWindowHeight, 
			camera.matView, camera.matProjection
		);

		Vector2 positionScreenV2(positionScreen.x, positionScreen.y);
		float distance = Distance(player->reticlePosition2D, positionScreenV2);

		const float kDistanceLockOn = 100.0f;

		if (distance <= kDistanceLockOn) {
			lockOnSprite_->SetPosition(positionScreenV2);
			targets.emplace_back(std::make_pair(distance, enemy));
			isLockedOn_ = true;
		} 

	

	}

target_ = nullptr;
	if (!targets.empty()) {
		targets.sort();
		target_ = targets.front().second; // Enemy* を保持

		targetScreenPosition_ = Project(
		    target_->GetWorldPosition(), // ワールド座標を渡す
		    0.0f, 0.0f, WinApp::kWindowWidth, WinApp::kWindowHeight, camera.matView, camera.matProjection);
	
		lockOnSprite_->SetPosition(Vector2(targetScreenPosition_.x, targetScreenPosition_.y));
	}

}

void LockOn::Draw() { 
	if (isLockedOn_) {
		lockOnSprite_->SetColor(Vector4(1, 1, 1, alpha_));
		lockOnSprite_->Draw();
	}
}

LockOn::~LockOn(){
	
	delete lockOnSprite_;

}
