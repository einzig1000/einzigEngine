#pragma once
#include "Game.h"


class PlayerBullet
{
public:
	PlayerBullet();

	bool Update(Vector3 pos, bool action, float radius);
	void Draw();
	Game::RenderData_Model data;


private:
};

