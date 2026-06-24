#pragma once
#include "KamataEngine.h"
#include "Player.h"
class GameScene 

{
public:

	GameScene();
	~GameScene();

	//大枠の初期化、アップデート、描画処理
	void Initialize();
	void Update();
	void Draw();

	//この先に小分けにしていく関数などを書く
private:


};
