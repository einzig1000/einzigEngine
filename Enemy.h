#pragma once
#include "Game.h"
#include "EnemyBullet.h"

class Player;

class Enemy
{
public:
	Enemy(int Tex, int model);
	~Enemy();

	void Update(Player& player);
	void Draw();
	Game::RenderData_Model data;
	std::vector<std::unique_ptr<EnemyBullet>> bulletList;

private:

	int frame;



};

