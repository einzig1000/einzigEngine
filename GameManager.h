#pragma once
#include "Map.h"
#include <memory>

class GameManager
{
public:
	GameManager();
	~GameManager();
	void Update();
	void Draw();


private:

	Map* map_;




};

