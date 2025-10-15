#pragma once
#include "Game.h"
#include "Player/PlayerBullet/PlayerBullet.h"

enum class actionState
{
	none,
	move,
	attack,
};

class Player
{
public:
	Player(int Tex, int model);
	~Player();

	void Update();
	void Draw();

	void SetBlock(Game::RenderData_Model& target) { data.SetBlock(target); }
	PlayerBullet* bullet_ = nullptr;

	Game::RenderData_Model data;
	float 落下高度 = 0.0f;
	float 攻撃終後の速度 = 0.0f;

private:
	int frame;

	bool 攻撃中 = false;

	actionState state = actionState::none;

};

