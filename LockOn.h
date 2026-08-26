#pragma once
#include "KamataEngine.h"

class Enemy;
class Player;

class LockOn {

public:
	LockOn() {};
	~LockOn();
	void Initialize();
	void Update(Player*player,std::list<Enemy*>& enemies,const KamataEngine::Camera&camera);
	void Draw();
	Enemy* GetTarget() const { return target_; }

	bool isLockedOn_ = false;
	float alpha_ = 0.60f;

	void SetAlpha(float alpha) { alpha_ = alpha; }

private:
	KamataEngine::Sprite* lockOnSprite_ = nullptr;
	KamataEngine::Vector3 targetScreenPosition_ = {};
	Enemy* target_ = nullptr;

};
