#pragma once
#include "Phase/PhaseParent/PhaseParent.h"
#include "Block.h"
#define MAX_BLOCK_X 10
#define MAX_BLOCK_Y 10
#define MAX_BLOCK_Z 10
#define BLOCK_SIZE 1.0f
#define PLAYER_SPEED 0.1f

class GameScenePhase :
	public PhaseParent
{
public:
	GameScenePhase();
	~GameScenePhase();
	void Initialize();
	void Update();
	void Draw();

	void LoadMap(const std::string& mapFilePath);

	Vector3 DirectionFromYawPitch(float yaw, float pitch);
	Vector3 YawPitchFromDirection(const Vector3& dir);


private:

	// レティクル
	RenderData_Sprite reticle_;

	// ブロック
	Block* block_[MAX_BLOCK_X][MAX_BLOCK_Y][MAX_BLOCK_Z];

	// ブロック破壊テクスチャ
	bool isDestroy_ = false;
	RenderData_Rect blockRect_[6];
	Transforms blockTriangleTransform_;
	void UpdateBlockTriangleTransform();

	// マウス関連
	Vector2 mousePos = { 0.0f,0.0f };
	Vector2 preMousePos = { 0.0f,0.0f };
	Vector2 mouseGap = { 0.0f,0.0f };
	float mouseSensitivity_ = 0.02f;
	void UpdateMousePos();

	// カメラ関連
	Vector3 cameraPos = { 0.0f,0.0f,0.0f };
	Vector3 cameraRot = { 0.0f,0.0f,0.0f };
	void UpdateCamera();

	// 衝突判定関連
	void UpdateCollisionCenterRay();
};