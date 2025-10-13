#pragma once
#include "include/engine/Engine/Game.h"
#include "Enemy.h"
#include "Player.h"

#define blockW 1
#define blockY 200
#define enemyN 10

class GameManager
{
public:
	GameManager();
	~GameManager();

	void Update();
	void Draw();

	void FrameTwoVerticalPoints(const Vector3& p1, const Vector3& p2, float padding, int spendFrame, EaseType easing);


private:


	int frame;

	Player* player_;
	Enemy* enemy_[enemyN];
	Game::RenderData_Model block_[blockW][blockY];

	bool spaceLog[30];
};

