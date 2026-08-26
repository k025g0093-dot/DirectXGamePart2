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
	KamataEngine::Sprite* btnTutorial_ = nullptr;

	// 選択カーソル
	KamataEngine::Sprite* cursor_ = nullptr;
	int32_t selectIndex_ = 0; // 0=Tutorial, 1=Easy, 2=Normal, 3=Hard
	bool prevDpadLeft_ = false;
	bool prevDpadRight_ = false;
	bool prevConfirm_ = false;
	bool prevBack_ = false;

	// 難易度説明テキスト（白色四角で表現）
	KamataEngine::Sprite* descBg_ = nullptr;
	KamataEngine::Sprite* descTextEasy_ = nullptr;
	KamataEngine::Sprite* descTextNormal_ = nullptr;
	KamataEngine::	Sprite* descTextHard_ = nullptr;
	KamataEngine::Sprite* descTextTutorial_ = nullptr;

	// チュートリアルパネル
	bool showTutorial_ = false;
	KamataEngine::Sprite* tutorialBg_ = nullptr;
	KamataEngine::Sprite* tutorialTitle_ = nullptr;
	KamataEngine::Sprite* tutorialText1_ = nullptr;
	KamataEngine::Sprite* tutorialText2_ = nullptr;
	KamataEngine::Sprite* tutorialText3_ = nullptr;
	KamataEngine::Sprite* tutorialText4_ = nullptr;
	KamataEngine::Sprite* tutorialText5_ = nullptr;
	KamataEngine::Sprite* tutorialBack_ = nullptr;

	// タイトルテクスチャ
	KamataEngine::Sprite* titleSprite_ = nullptr;

	// 「PRESS START」表示用スプライト
	KamataEngine::Sprite* pressStartSprite_ = nullptr;
	float blinkTimer_ = 0.0f;
};
