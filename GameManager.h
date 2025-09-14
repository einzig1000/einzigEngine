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

	Game::RenderData_Model test;
	Game::RenderData_Model test1;

};

