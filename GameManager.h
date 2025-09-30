#pragma once
#include "Game.h"

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

	int kk[100];

};

