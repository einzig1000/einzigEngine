#pragma once
#include "Game.h"
#include <sstream>

#define MAP_HEIGHT 10
#define MAP_WIDTH 20

class Map
{
public:
	Map();
	void Initialize();

	void LoadMap(int stageNum);

	void Update();
	void Draw();


private:
	Game::RenderData_Model data[MAP_HEIGHT][MAP_WIDTH];
	int blockType[MAP_HEIGHT][MAP_WIDTH];

	std::stringstream mapCSV[10];
};