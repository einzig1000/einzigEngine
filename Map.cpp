#include "Map.h"
#include <fstream>
#include <cassert>
#include <algorithm>

Map::Map()
{
	int model = Game::LoadOBJ("resources/block", "map.obj");
	int texture = Game::LoadTexture("resources/block/map.png");

	for (int x = 0; x < MAP_WIDTH; ++x)
	{
		for (int y = 0; y < MAP_HEIGHT; ++y)
		{
			data[y][x].model = model;
			data[y][x].texture = texture;
			data[y][x].transforms.translate = PositionByIndex(Vector2int(y, x));
		}
	}
}

void Map::Initialize()
{
}

void Map::LoadMap(int stageNum)
{
	if (mapCSV[stageNum].str().empty())
	{
		// ファイルをひらく
		std::ifstream file;
		file.open("resources/csv/map1.csv");
		assert(file.is_open());

		// ファイルの内容を丸ごとコピー
		mapCSV[0] << file.rdbuf();

		// ファイルを閉じる
		file.close();
	}

	// 1行文
	std::string line;

	// コマンド実行
	while (getline(mapCSV[0], line))
	{
		std::istringstream line_stream(line);
		std::string word;
		int lineNumber = 0;
		int wordNumber = 0;

		while (getline(line_stream, word, ','))
		{
			if (word.find("0") == 0)
			{
				blockType[lineNumber][wordNumber];
			}
			else if (word.find("1") == 0)
			{
				blockType[lineNumber][wordNumber];
			}
			wordNumber++;
		}
		lineNumber++;
	}
}

void Map::Update()
{
	for (int x = 0; x < MAP_WIDTH; ++x)
	{
		for (int y = 0; y < MAP_HEIGHT; ++y)
		{
	
		}
	}
}

void Map::Draw()
{
	for (int x = 0; x < MAP_WIDTH; ++x)
	{
		for (int y = 0; y < MAP_HEIGHT; ++y)
		{
			data[y][x].Draw();
		}
	}
}

Vector2int Map::IndexByPosition(Vector3 pos)
{
	Vector2int index;
	// ブロック中心座標からインデックスを計算
	index.x = static_cast<int>(std::round(-pos.x / BLOCK_WIDTH));
	index.y = static_cast<int>(std::round(-pos.z / BLOCK_HEIGHT));
	// 範囲外の値を制限
	index.x = std::clamp(index.x, 0, MAP_WIDTH - 1);
	index.y = std::clamp(index.y, 0, MAP_HEIGHT - 1);
	return index;
}

Vector3 Map::PositionByIndex(Vector2int index)
{
	Vector3 pos;

	pos.x = -static_cast<float>(index.x) * BLOCK_WIDTH;
	pos.y = 0.0f;
	pos.z = -static_cast<float>(index.y) * BLOCK_HEIGHT;

	return pos;
}

int Map::BlockTypeByIndex(Vector2int index)
{
	return blockType[index.y][index.x];
}
