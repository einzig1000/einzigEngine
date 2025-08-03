#pragma once
#include "Game.h"
#include <sstream>

#define MAP_HEIGHT 10
#define MAP_WIDTH 20
#define BLOCK_HEIGHT 1.2f
#define BLOCK_WIDTH 1.2f

enum class BloclType
{
	Empty = 0,		// 空（通行可能）
	Wall = 1,		// 壁（通行不可）
	Asid = 2,		// 毒（歩行コスト倍増）
	WarpIn = 3,		// ワープ入口
	WarpOut = 4,	// ワープ出口
};

class Map
{
public:
	Map();
	void Initialize();

	void LoadMap(int stageNum);

	void Update();
	void Draw();

	/// <summary>
	/// ブロックタイプ合った形状に変化させる
	/// </summary>
	void ShapeChangeByType(Vector2int index);

	Vector2int IndexByPosition(Vector3 pos);
	Vector3 PositionByIndex(Vector2int index);
	BloclType BlockTypeByIndex(Vector2int index);

private:
	Game::RenderData_Model data[MAP_HEIGHT][MAP_WIDTH];
	BloclType blockType[MAP_HEIGHT][MAP_WIDTH];

	std::stringstream mapCSV[STAGE_MAX];
};