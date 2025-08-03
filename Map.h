#pragma once
#include "Game.h"
#include <sstream>

#define MAP_HEIGHT 10
#define MAP_WIDTH 20
#define BLOCK_HEIGHT 1.2f
#define BLOCK_WIDTH 1.2f

class Map
{
public:
	Map();
	void Initialize();

	void LoadMap(int stageNum);

	void Update();
	void Draw();

	Vector2int IndexByPosition(Vector3 pos);
	Vector3 PositionByIndex(Vector2int index);
	int BlockTypeByIndex(Vector2int index);

private:
	Game::RenderData_Model data[MAP_HEIGHT][MAP_WIDTH];
	int blockType[MAP_HEIGHT][MAP_WIDTH];

	std::stringstream mapCSV[10];
};