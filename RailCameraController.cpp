#define NOMINMAX
#include "RailCameraController.h"
#include "KamataEngine.h"
#include "MyMath.h"

#include "Player.h"
#include <algorithm>
#include <numbers>

//カマタエンジンの名前の省略のために宣言
using namespace KamataEngine;


void RailCameraController::Initialize() {
	camera_.Initialize();
	worldTransform_.Initialize();

	// オフセット設定
	targetOffset_ = {0, 0, 0.0f}; // 高さも調整可能に
	worldTransform_.translation_ = {0, 0.0f, -100};
	// 移動可能範囲
	movableArea_ = {-50.0f, 50.0f, 0.0f, 50.0f};

	// 初期カメラ位置
	endCameraPosition_ = worldTransform_.translation_;


	//camera_.farZ = 100.0f;
	camera_.UpdateMatrix();
}

void RailCameraController::Update() {
	if (target_) {
		// プレイヤーの位置を取得
		Vector3 targetPos = target_->GetWorldPosition();

		// カメラの目標位置（プレイヤー位置 + オフセット）
		Vector3 targetCameraPos = targetPos + targetOffset_;

		// 移動可能範囲内に制限
		targetCameraPos.x = std::clamp(targetCameraPos.x, movableArea_.left, movableArea_.right);
		targetCameraPos.y = std::clamp(targetCameraPos.y, movableArea_.bottom, movableArea_.top);

		// カメラとプレイヤーの距離差を計算
		Vector3 diff = targetCameraPos - worldTransform_.translation_;

		// deadZone外なら補間で移動
		if (std::abs(diff.x) > cameraDeadZone_.right || std::abs(diff.y) > cameraDeadZone_.top) {
			endCameraPosition_ = Lerp(worldTransform_.translation_, targetCameraPos, kInterpolationRate);
		}
	}
#ifdef _DEBUG
	ImGui::Begin("CameraPos");
	ImGui::DragFloat3("position", &worldTransform_.translation_.x, 0.01f, -100.0f, 100.0f);

	ImGui::End();
#endif

	//worldTransform_.translation_ = endCameraPosition_;
	worldTransform_.translation_.z += kVelocityBias * 0.016f; // 毎フレームの更新でZ軸方向に移動

	UpdateWorldTransform(worldTransform_); 
	camera_.translation_ = worldTransform_.translation_;
	camera_.UpdateMatrix();  
}


KamataEngine::Vector3 RailCameraController::Lerp(const KamataEngine::Vector3& v1, const KamataEngine::Vector3& v2, float t) { return v1 + (v2 - v1) * t; }