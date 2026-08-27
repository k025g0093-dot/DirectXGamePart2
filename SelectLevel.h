#pragma once
#include "Fade.h"
#include "KamataEngine.h"
#include <vector>
#include "GameScene.h"

class SelectLevel {

public:
	enum class GamePhase { kFadeIn, kPlay, kFadeOut };

	GamePhase gamePhase_ = GamePhase::kFadeIn;
	void Initialize();
	void Update();
	void Draw();

	~SelectLevel();

	bool finished_ = false;
	bool IsFinished() const { return finished_; }

	Fade* fade_ = nullptr;

public:
	DifficultyLevel GetSelectedDifficulty() const { return selectedDifficulty_; }

private:
	DifficultyLevel selectedDifficulty_ = DifficultyLevel::kNormal;

	// ===== 背景の3D（天球と地面）=====
	KamataEngine::Camera camera_;
	SkyDome* skyDome_ = nullptr;
	Plane* plane_ = nullptr;
	KamataEngine::Model* skyDomeModel_ = nullptr;
	KamataEngine::Model* groundModel_ = nullptr;
	// 背景をゆっくり流すための回転角
	float skyRotation_ = 0.0f;

	// 難易度選択用スプライト
	uint32_t sampleTexture_ = 0;
	uint32_t whiteTexture_ = 0;

	// 難易度説明のUI画像（1280x720の全画面オーバーレイ）
	uint32_t easyTexture_ = 0;
	uint32_t normalTexture_ = 0;
	uint32_t hardTexture_ = 0;
	uint32_t tutorialDescTexture_ = 0;
	uint32_t selectLevelTexture_ = 0;
	// チュートリアル画面（タイトル・本文・戻る案内まで1枚に入った画像）
	uint32_t tutorialTexture_ = 0;

	// 難易度の並びを描いた画像
	KamataEngine::Sprite* selectLevelSprite_ = nullptr;

	// 選択カーソル
	KamataEngine::Sprite* cursor_ = nullptr;
	// 選択中を強調する下線と左右マーカー
	KamataEngine::Sprite* cursorBar_ = nullptr;
	KamataEngine::Sprite* markerL_ = nullptr;
	KamataEngine::Sprite* markerR_ = nullptr;
	float selectPulse_ = 0.0f;
	int32_t selectIndex_ = 0; // 0=Tutorial, 1=Easy, 2=Normal, 3=Hard
	bool prevDpadLeft_ = false;
	bool prevDpadRight_ = false;
	bool prevConfirm_ = false;
	bool prevBack_ = false;

	// 難易度説明テキスト（白色四角で表現）
	KamataEngine::Sprite* descBg_ = nullptr;
	KamataEngine::Sprite* descTextEasy_ = nullptr;
	KamataEngine::Sprite* descTextNormal_ = nullptr;
	KamataEngine::Sprite* descTextHard_ = nullptr;
	KamataEngine::Sprite* descTextTutorial_ = nullptr;

	// チュートリアルパネル
	bool showTutorial_ = false;
	// 背景を暗くするオーバーレイ
	KamataEngine::Sprite* tutorialBg_ = nullptr;
	// 説明の中身。文字は画像に描かれているので1枚で足りる
	KamataEngine::Sprite* tutorialImage_ = nullptr;

	// タイトルテクスチャ
	KamataEngine::Sprite* titleSprite_ = nullptr;

};
