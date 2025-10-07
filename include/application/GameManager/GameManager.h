#pragma once
#include "include/engine/Engine/Game.h"

class GameManager
{
public:
	GameManager();
	~GameManager();

	void Update();
	void Draw();


private:

	int frame;

	Game::RenderData_Model player;
	Game::RenderData_Model ground;
	Game::RenderData_Sprite	sprite;

	float roll = 0.0f;
};

