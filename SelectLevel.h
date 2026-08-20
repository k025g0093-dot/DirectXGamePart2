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

	// 難易度選択用スプライト
	uint32_t sampleTexture_ = 0;
	uint32_t whiteTexture_ = 0;

	// 各難易度ボタン
	KamataEngine::Sprite* btnEasy_ = nullptr;
	KamataEngine::Sprite* btnNormal_ = nullptr;
	KamataEngine::Sprite* btnHard_ = nullptr;

	// 選択カーソル
	KamataEngine::Sprite* cursor_ = nullptr;
	int32_t selectIndex_ = 1; // 0=Easy, 1=Normal, 2=Hard
	bool prevDpadLeft_ = false;
	bool prevDpadRight_ = false;
	bool prevConfirm_ = false;

	// タイトルテクスチャ
	KamataEngine::Sprite* titleSprite_ = nullptr;
};
