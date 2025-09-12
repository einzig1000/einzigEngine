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

	Game::RenderData_Model test;

};

