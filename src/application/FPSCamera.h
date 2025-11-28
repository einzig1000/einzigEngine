#pragma once
#include "Game.h"

class Player;

class FPSCamera
{
public:
	FPSCamera(Player* player);
	void Update();


	// カメラ操作可能か
	bool enableControl;

	// カメラ関係
	Vector3 cameraPos = { 0.0f,0.0f,0.0f };
	Vector3 cameraRot = { 0.0f,0.0f,0.0f };

	// マウス関連
	Vector2 mousePos = { 0.0f,0.0f };
	Vector2 preMousePos = { 0.0f,0.0f };
	Vector2 mouseGap = { 0.0f,0.0f };
	float mouseSensitivity_ = 0.02f;

	// プレイヤー
	Player* player_ = nullptr;
};

