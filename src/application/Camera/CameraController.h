#pragma once
#include "Game.h"

class Player;

class CameraController
{
public:
	CameraController(Player* player);
	void Update();

	CameraMode_FirstPerson_ThirdPerson cameraMode_;

	// カメラ操作可能か
	bool enableControl;

	// カメラ関係
	Vector3 cameraPos = { 0.0f,0.0f,0.0f };
	Vector3 cameraRot = { 0.0f,0.0f,0.0f };

	// マウス関連
	Vector2 mousedelta = { 0.0f,0.0f };
	float mouseSensitivity_ = 0.02f;

	// プレイヤー
	Player* player_ = nullptr;
};

