#pragma once
#include "Game.h"

class FPSCamera;

class Player
{
public:
	Player();
	~Player();

	void Initialize();
	void Update();
	void Draw();

	// 視線レイ
	Ray viewRay_;
	// レティクル
	RenderData_Sprite reticle_;

	// プレイヤー
	RenderData_Model data_;

	// カメラ
	FPSCamera* fpsCamera_;

};

