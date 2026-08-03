#pragma once
#include "Fade.h"
#include "KamataEngine.h"
#include <vector>
#include "GameScene.h"

class SelectLevel {

public:
	enum class GamePhase { kFadeIn, kPlay, kFadeOut };

	GamePhase gamePhase_ = GamePhase::kFadeIn;
	// 初期化関数
	void Initialize();
	// 更新処理
	void Update();

	void Draw();

	~SelectLevel();

	bool finished_ = false;

	bool IsFinished() const { return finished_; }

	Fade* fade_ = nullptr;

public:
	//レベルの選択関係
	DifficultyLevel GetSelectedDifficulty() const { return selectedDifficulty_; }

private:
	DifficultyLevel selectedDifficulty_ = DifficultyLevel::kNormal;
		

private:
	// ワールドトランスフォーム
	KamataEngine::WorldTransform worldTransform_;
	// ビュープロジェクション（カメラ）
	KamataEngine::Camera camera_;

	// タイトル用のモデルなどが必要な場合はここに追加
	KamataEngine::Model* modelTitle_ = nullptr;

	// ★これを追加
	KamataEngine::ObjectColor objectColor_;
};
