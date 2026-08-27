#pragma once
#include "Fade.h"
#include "KamataEngine.h"
#include "SkyDome.h"
#include "Plane.h"
#include <vector>


class GameCreaScene {
public:

	enum class GamePhase {
		kFadeIn,
		kPlay,
		kFadeOut
	};

	GamePhase gamePhase_ = GamePhase::kFadeIn;
	// 初期化関数
	void Initialize();
	// 更新処理
	void Update();

	void Draw();

	~GameCreaScene();

	bool finished_ = false;

	bool IsFinished() const { return finished_; }

	Fade* fade_ = nullptr;

private:
	// 背景（天球と地面）を映すカメラ。ゆっくり回す
	KamataEngine::Camera camera_;
	float skyRotation_ = 0.0f;

	// 結果表示の画像
	KamataEngine::Sprite* resultSprite_ = nullptr;
	uint32_t resultTexture_ = 0;

	// 「BACK TO TITLE」表示用スプライト
	KamataEngine::Sprite* pressStartSprite_ = nullptr;
	uint32_t pressStartTexture_ = 0;
	float blinkTimer_ = 0.0f;
	bool showPressStart_ = true;

	// クリア画面に飾るプレイヤー機体
	KamataEngine::Model* playerModel_ = nullptr;
	KamataEngine::WorldTransform playerTransform_;
	// 機体専用のカメラ。背景カメラは回転するので、それとは別に固定で持つ
	KamataEngine::Camera modelCamera_;

	// 天球と地面
	SkyDome* skyDome_ = nullptr;
	KamataEngine::Model* skyDomeModel_ = nullptr;
	Plane* plane_ = nullptr;
	KamataEngine::Model* planeModel_ = nullptr;
};
