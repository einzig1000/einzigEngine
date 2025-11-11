#pragma once
#include "Phase/PhaseParent/PhaseParent.h"
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

private:

	RenderData_Model block_[MAX_BLOCK_X][MAX_BLOCK_Y][MAX_BLOCK_Z];

	Vector2 mousePos = { 0.0f,0.0f };
	Vector2 preMousePos = { 0.0f,0.0f };
	Vector2 mouseGap = { 0.0f,0.0f };
	void UpdateMousePos();

	Vector3 cameraPos = { 0.0f,0.0f,0.0f };
	Vector3 cameraRot = { 0.0f,0.0f,0.0f };
	void UpdateCamera();
};

