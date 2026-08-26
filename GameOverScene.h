#pragma once
#include "Fade.h"
#include "KamataEngine.h"
#include "SkyDome.h"
#include "Plane.h"
#include <vector>


class GameOverScene {
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

	~GameOverScene();

	bool finished_ = false;

	bool IsFinished() const { return finished_; }

	Fade* fade_ = nullptr;

private:
	// ワールドトランスフォーム
	KamataEngine::WorldTransform worldTransform_;
	// ビュープロジェクション（カメラ）
	KamataEngine::Camera camera_;

	// タイトル用のモデルなどが必要な場合はここに追加
	KamataEngine::Model* modelTitle_ = nullptr;

	// ★これを追加
	KamataEngine::ObjectColor objectColor_;

	// 「BACK TO TITLE」表示用スプライト
	KamataEngine::Sprite* pressStartSprite_ = nullptr;
	uint32_t pressStartTexture_ = 0;
	float blinkTimer_ = 0.0f;
	bool showPressStart_ = true;

	// 天球と地面
	SkyDome* skyDome_ = nullptr;
	KamataEngine::Model* skyDomeModel_ = nullptr;
	Plane* plane_ = nullptr;
	KamataEngine::Model* planeModel_ = nullptr;
};
