#pragma once
#include "Phase/PhaseParent/PhaseParent.h"
#include "Enemy/Enemy.h"
#include "Player/Player.h"

#define blockW 1
#define blockY 200
#define enemyN 10




class TestPhase :
	public PhaseParent
{
public:
	TestPhase();
	~TestPhase();

	void Initialize();
	void Update();
	void Draw();

	void FrameTwoVerticalPoints(const Vector3& p1, const Vector3& p2, float padding, int spendFrame, EaseType easing);


private:


	int frame;

	Player* player_;
	Enemy* enemy_[enemyN];
	Game::RenderData_Model block_[blockW][blockY];
};