#pragma once
#include "Fade.h"
#include "KamataEngine.h"
#include "SkyDome.h"
#include "Plane.h"
#include <vector>


class TitleScene {
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

	~TitleScene();

	bool finished_ = false;

	bool IsFinished() const { return finished_; }

	Fade* fade_ = nullptr;

private:
	// ゲームタイトルのロゴ画像
	KamataEngine::Sprite* titleLogoSprite_ = nullptr;
	uint32_t titleLogoTexture_ = 0;

	// 「Ⓐ/SPACE start」の案内画像
	KamataEngine::Sprite* titleImageSprite_ = nullptr;
	uint32_t titleImageTexture_ = 0;

	// 天球と地面
	SkyDome* skyDome_ = nullptr;
	KamataEngine::Model* skyDomeModel_ = nullptr;
	Plane* plane_ = nullptr;
	KamataEngine::Model* planeModel_ = nullptr;

	// 背景専用のカメラ。これを回して天球を流す
	KamataEngine::Camera bgCamera_;
	float skyRotation_ = 0.0f;
};
