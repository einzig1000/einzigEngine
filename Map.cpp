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
	if (stageNum >= STAGE_MAX)return;

	// 既に読み込み済だったらスキップ
	if (mapCSV[stageNum].str().empty())
	{
		// パス作成
		std::ostringstream path;
		path << "resources/csv/map" << stageNum << ".csv";

		// ファイルをひらく
		std::ifstream file;
		file.open(path.str());
		assert(file.is_open());

		// ファイルの内容を丸ごとコピー
		mapCSV[0] << file.rdbuf();

		// ファイルを閉じる
		file.close();
	}

	// 1行ずつ
	std::string line;

	// ブロックタイプ適用
	int lineNumber = 0;
	int wordNumber = 0;
	while (getline(mapCSV[0], line))
	{
		std::istringstream line_stream(line);
		std::string word;
		wordNumber = 0;

		while (getline(line_stream, word, ','))
		{
			if (word.find("0") == 0)
			{
				blockType[lineNumber][wordNumber] = BloclType::Empty;
			}
			else if (word.find("1") == 0)
			{
				blockType[lineNumber][wordNumber] = BloclType::Wall;
			}
			else if (word.find("2") == 0)
			{
				blockType[lineNumber][wordNumber] = BloclType::Asid;
			}
			else if (word.find("3") == 0)
			{
				blockType[lineNumber][wordNumber] = BloclType::WarpIn;
			}
			else if (word.find("4") == 0)
			{
				blockType[lineNumber][wordNumber] = BloclType::WarpOut;
			}
			wordNumber++;
		}
		lineNumber++;
	}

	// ブロックタイプごとのSRTやcolorの適用
	for (int x = 0; x < MAP_WIDTH; ++x)
	{
		for (int y = 0; y < MAP_HEIGHT; ++y)
		{
			ShapeChangeByType(Vector2int(y, x));
		}
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

void Map::ShapeChangeByType(Vector2int index)
{
	if (blockType[index.y][index.x] == BloclType::Empty)
	{
		data[index.y][index.x].transforms.scale = Vector3{ 1.0f,1.0f,1.0f };
		data[index.y][index.x].transforms.rotate = Vector3{ 0.0f,0.0f,0.0f };
		data[index.y][index.x].transforms.translate = PositionByIndex(Vector2int(index.y, index.x));
		data[index.y][index.x].color = 0xFFFFFFFF;
	}
	else if (blockType[index.y][index.x] == BloclType::Wall)
	{
		data[index.y][index.x].transforms.scale = Vector3{ 1.0f,5.0f,1.0f };
		data[index.y][index.x].transforms.rotate = Vector3{ 0.0f,0.0f,0.0f };
		data[index.y][index.x].transforms.translate = PositionByIndex(Vector2int(index.y, index.x));
		data[index.y][index.x].transforms.translate.y += 0.4f;
		data[index.y][index.x].color = 0xFFFFFFFF;
	}
	else if (blockType[index.y][index.x] == BloclType::Asid)
	{
		data[index.y][index.x].transforms.scale = Vector3{ 1.0f,1.0f,1.0f };
		data[index.y][index.x].transforms.rotate = Vector3{ 0.0f,0.0f,0.0f };
		data[index.y][index.x].transforms.translate = PositionByIndex(Vector2int(index.y, index.x));
		data[index.y][index.x].color = 0xFF0000FF;
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

BloclType Map::BlockTypeByIndex(Vector2int index)
{
	return blockType[index.y][index.x];
}
