#define NOMINMAX
#include "Player.h"
#include "Enemy.h"
#include "LockOn.h"
#include "MyMath.h"
#include <algorithm>
#include <cassert>
#include <numbers>

#ifdef _DEBUG
#include <imgui.h>
#endif

using namespace KamataEngine;

Player::Player() {}

void Player::Initialize(
    KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position

) {

	worldTransform_.scale_ = {1.0f, 1.0f, 1.0f};
	worldTransform_.rotation_ = {0.0f, 0.0f, 0.0f};
	worldTransform_.translation_ = {0.0f, 0.0f, 0.0f};
	objectColor_.Initialize();
	objectColor_.SetColor({1.0f, 1.0f, 1.0f, alfaColor});

	velocity_ = {0.0f, 0.0f, 0.0f};

	assert(model);
	model_ = model;

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = 0.0f;

	// 3Dレティクルのワールドトランスフォームの初期化
	worldTransform3DReticle_.Initialize();

	// ２Dレティクル初期化
	uint32_t textureReticle = TextureManager::Load("2DReticle.png");
	sprite2DReticle_ = Sprite::Create(textureReticle, {0, 0}, {1, 1, 1, 0.6f}, {0.5f, 0.5f});

	camera_ = camera;

	// キー入力の初期化
	input_ = Input::GetInstance();
}

void Player::Updata() {

	MovePlayer();
	Rotate();
	Attack();

	bullets_.remove_if([](playerBullet* bullet) {
		if (bullet->IsDead()) {
			delete bullet;
			return true;
		}
		return false;
	});

	for (playerBullet* bullet : bullets_) {
		bullet->Update();
	}

	if (incvincibleTimer_ > 0) {
		incvincibleTimer_--;
	}

	if (incvincibleTimer_ <= 0) {
		alfaColor += 1.0f;
		objectColor_.SetColor({1.0f, 1.0f, 1.0f, alfaColor});
	}

	UpdateWorldTransform(worldTransform_);
	Update3DReticlePosition();
	Update2DReticlePosition();
}

void Player::Draw() {
	for (playerBullet* bullet : bullets_) {
		bullet->Draw(camera_);
	}
	model_->Draw(worldTransform_, *camera_, &objectColor_);
	model3DReticle_->Draw(worldTransform3DReticle_, *camera_);
}

void Player::DrawUI() {
	// 2Dレティクルの描画
	sprite2DReticle_->Draw();
}

Player::~Player() {
	for (playerBullet* bullet : bullets_) {
		delete bullet;
	}
	delete sprite2DReticle_;
}

void Player::MovePlayer() {

	Vector3 move = {0, 0, 0};
	// 移動速度
	const float kCharacteaSpeed = 0.2f;

	// 移動制限座標
	const float kMoveLimitX = 13;
	const float kMoveLimitY = 7;

	worldTransform_.translation_.x = std::max(worldTransform_.translation_.x, -kMoveLimitX);
	worldTransform_.translation_.x = std::min(worldTransform_.translation_.x, +kMoveLimitX);

	worldTransform_.translation_.y = std::max(worldTransform_.translation_.y, -kMoveLimitY);
	worldTransform_.translation_.y = std::min(worldTransform_.translation_.y, +kMoveLimitY);

#ifdef _DEBUG
	ImGui::Begin("PlayerPostion");
	ImGui::DragFloat3("position", &worldTransform_.translation_.x, 0.01f, -100.0f, 100.0f);
	ImGui::DragFloat3("rotation_", &worldTransform_.rotation_.x, 0.01f, -100.0f, 100.0f);

	ImGui::End();
#endif

	if (input_->PushKey(DIK_LEFT) || input_->PushKey(DIK_A)) {
		move.x -= kCharacteaSpeed;
	} else if (input_->PushKey(DIK_RIGHT) || input_->PushKey(DIK_D)) {
		move.x += kCharacteaSpeed;
	}

	// 上下移動
	if (input_->PushKey(DIK_UP) || input_->PushKey(DIK_W)) {
		move.y += kCharacteaSpeed;
	} else if (input_->PushKey(DIK_DOWN) || input_->PushKey(DIK_S)) {
		move.y -= kCharacteaSpeed;
	}

	// ゲームパッドの状態取得
	XINPUT_STATE joyState{};
	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		move.x += (float)joyState.Gamepad.sThumbLX / SHRT_MAX * kCharacteaSpeed;
		move.y += (float)joyState.Gamepad.sThumbLY / SHRT_MAX * kCharacteaSpeed;
	}

	worldTransform_.translation_ += move;
}

// 回転
void Player::Rotate() {

	const float kRotSpeed = 0.02f;
	const float kMaxRotY = 0.8f; // 左右の回転制限（ラジアン）

	if (input_->PushKey(DIK_Q)) {
		worldTransform_.rotation_.y += kRotSpeed;
	} else if (input_->PushKey(DIK_E)) {
		worldTransform_.rotation_.y -= kRotSpeed;
	}

	// ゲームパッドの状態取得
	XINPUT_STATE joyState{};
	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		worldTransform_.rotation_.y -= (float)joyState.Gamepad.sThumbRX / SHRT_MAX * kRotSpeed;
		// 右スティック上下でレティクルを上下移動
		reticleOffsetY_ += (float)joyState.Gamepad.sThumbRY / SHRT_MAX * 0.15f;
		reticleOffsetY_ = std::max(reticleOffsetY_, -5.0f);
		reticleOffsetY_ = std::min(reticleOffsetY_, 5.0f);
	}

	// 回転制限をクランプ
	if (worldTransform_.rotation_.y > kMaxRotY) {
		worldTransform_.rotation_.y = kMaxRotY;
	}
	if (worldTransform_.rotation_.y < -kMaxRotY) {
		worldTransform_.rotation_.y = -kMaxRotY;
	}
}

// 攻撃
void Player::Attack() {

	// クールダウン中は撃てない
	if (fireCooldown_ > 0) {
		fireCooldown_--;
		return;
	}

	if (input_->PushKey(DIK_SPACE)) {
		if (lockOn_->isLockedOn_) {

			Enemy* target = lockOn_->GetTarget();

			if (target) {
				Vector3 targetPos = target->GetWorldPosition();
				Vector3 playerPos = GetWorldPosition();
				Vector3 direction = targetPos - playerPos;
				direction = Normalize(direction);
				const float kBulletSpeed = 1.0f;
				Vector3 velocity = direction * kBulletSpeed;
				playerBullet* newBullet = new playerBullet();
				newBullet->Initialize(bulletModel_, playerPos, velocity);
				bullets_.push_back(newBullet);
				fireCooldown_ = kFireInterval;
			}

		} else {
			const float kBulletSpeed = 1.0f;
			Vector3 velocity(0, 0, kBulletSpeed);
			velocity = worldTransform3DReticle_.translation_ - GetWorldPosition();
			velocity = Normalize(velocity) * kBulletSpeed;

			playerBullet* newBullet = new playerBullet();

			newBullet->Initialize(bulletModel_, GetWorldPosition(), velocity);
			bullets_.push_back(newBullet);
			fireCooldown_ = kFireInterval;
		}
	}

	// ゲームパッドの状態取得
	XINPUT_STATE joyState{};
	// 何も接続されてないなら抜ける
	if (!Input::GetInstance()->GetJoystickState(0, joyState)) {
		return;
	}
	// Rトリガーを押したら発射
	if (joyState.Gamepad.bRightTrigger > 0) {

		if (lockOn_->isLockedOn_) {

			Enemy* target = lockOn_->GetTarget();

			if (target) {
				Vector3 targetPos = target->GetWorldPosition();
				Vector3 playerPos = GetWorldPosition();
				Vector3 direction = targetPos - playerPos;
				direction = Normalize(direction);
				const float kBulletSpeed = 1.0f;
				Vector3 velocity = direction * kBulletSpeed;
				playerBullet* newBullet = new playerBullet();
				newBullet->Initialize(bulletModel_, playerPos, velocity);
				bullets_.push_back(newBullet);
				fireCooldown_ = kFireInterval;
			}

		} else {
			const float kBulletSpeed = 1.0f;
			Vector3 velocity(0, 0, kBulletSpeed);
			velocity = worldTransform3DReticle_.translation_ - GetWorldPosition();
			velocity = Normalize(velocity) * kBulletSpeed;

			playerBullet* newBullet = new playerBullet();

			newBullet->Initialize(bulletModel_, GetWorldPosition(), velocity);
			bullets_.push_back(newBullet);
			fireCooldown_ = kFireInterval;
		}
	}
}

Vector3 Player::GetWorldPosition() {
	Vector3 worldPos{};

	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

void Player::OnCollision() {
	// 無敵時間中はダメージを受けない
	if (incvincibleTimer_ > 0) {

		return;
	}
	hp_--;

	if (hp_ <= 0) {
		isDead_ = true;
	} else {
		incvincibleTimer_ = incvincibleTimerMax_; // 無敵開始
		alfaColor = 0.5f;
		objectColor_.SetColor({1.0f, 1.0f, 1.0f, alfaColor});
	}
}

void Player::Update3DReticlePosition() {

	const float kDistancePlayerTo3DReticle = 10.0f;
	Vector3 offset = {0, 0, 1.0f};
	offset = TransformNolmar(offset, worldTransform_.matWorld_);

	offset = Normalize(offset) * kDistancePlayerTo3DReticle;

	worldTransform3DReticle_.translation_ = GetWorldPosition() + offset;
	worldTransform3DReticle_.translation_.y += reticleOffsetY_;
	UpdateWorldTransform(worldTransform3DReticle_);
}

void Player::Update2DReticlePosition() {

	Vector3 position2DReticle = {worldTransform3DReticle_.matWorld_.m[3][0], worldTransform3DReticle_.matWorld_.m[3][1], worldTransform3DReticle_.matWorld_.m[3][2]};

	Matrix4x4 matViewport = MakeViewportMatrix(0, 0, WinApp::kWindowWidth, WinApp::kWindowHeight);

	Matrix4x4 matViewProjectionViewport = Multiply(Multiply(camera_->matView, camera_->matProjection), matViewport);

	position2DReticle = Transform(position2DReticle, matViewProjectionViewport);

	reticlePosition2D = Vector2(position2DReticle.x, position2DReticle.y);

	sprite2DReticle_->SetPosition(Vector2(position2DReticle.x, position2DReticle.y));
}

void Player::SetParent(const KamataEngine::WorldTransform* parent) { worldTransform_.parent_ = parent; }