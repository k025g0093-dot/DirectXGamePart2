#pragma once
#include "CameraController.h"
#include "Fade.h"
#include "KamataEngine.h"
#include <vector>


class TitleScene {
public:

	enum class Phese { 
		kFadeIn,
		kMain,
		kFadeOut,
	};
	Phese phase_ = Phese::kFadeIn;
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
	// ワールドトランスフォーム
	KamataEngine::WorldTransform worldTransform_;
	// ビュープロジェクション（カメラ）
	KamataEngine::Camera camera_;

	// タイトル用のモデルなどが必要な場合はここに追加
	KamataEngine::Model* modelTitle_ = nullptr;

	// ★これを追加
	KamataEngine::ObjectColor objectColor_;
};
