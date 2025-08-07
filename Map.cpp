#include "Map.h"
#include <fstream>
#include <cassert>
#include <algorithm>


Map::Map()
{
	for (int x = 0; x < MAP_WIDTH; ++x)
	{
		for (int y = 0; y < MAP_HEIGHT; ++y)
		{
			data[y][x].model = uint32_t(TEXTURE::Map_Block);
			data[y][x].texture = uint32_t(TEXTURE::Map_Block);
			data[y][x].transforms.translate = PositionByIndex(Vector2int(x, y));
		}
	}
}

void Map::Initialize()
{
	//data[1][0].color = 0x000000FF;
}

void Map::LoadMap(int stageNum)
{
	if (stageNum >= STAGE_MAX)return;

	// 既に読み込み済だったらスキップ
	if (mapCSV[stageNum].empty())
	{
		// パス作成
		std::ostringstream path;
		path << "resources/csv/map" << stageNum << ".csv";

		// ファイルをひらく
		std::ifstream file(path.str());
		assert(file.is_open());

		// ファイルの内容を丸ごとコピー
		std::ostringstream buffer;
		buffer << file.rdbuf();
		mapCSV[stageNum] = buffer.str();

		// ファイルを閉じる
		file.close();
	}
	// 1行ずつ
	std::string line;
	// ここで毎回新しいstringstreamを作る
	std::istringstream mapStream(mapCSV[stageNum]);


	// ブロックタイプ適用
	int lineNumber = 0;
	int wordNumber = 0;
	while (getline(mapStream, line))
	{
		std::istringstream line_stream(line);
		std::string word;
		wordNumber = 0;

		while (getline(line_stream, word, ','))
		{
			if (lineNumber < MAP_HEIGHT && wordNumber < MAP_WIDTH)
			{
				if (word.find("0") == 0)
				{
					blockType[lineNumber][wordNumber] = BLOCK_TYPE::Empty;
				}
				else if (word.find("1") == 0)
				{
					blockType[lineNumber][wordNumber] = BLOCK_TYPE::Wall;
				}
				else if (word.find("2") == 0)
				{
					blockType[lineNumber][wordNumber] = BLOCK_TYPE::Asid;
				}
				else if (word.find("3") == 0)
				{
					blockType[lineNumber][wordNumber] = BLOCK_TYPE::WarpIn;
				}
				else if (word.find("4") == 0)
				{
					blockType[lineNumber][wordNumber] = BLOCK_TYPE::WarpOut;
				}
			}
			wordNumber++;
		}
		lineNumber++;
	}

	// X軸反転
	FlipXAxis(blockType);

	// ブロックタイプごとのSRTやcolorの適用
	for (int x = 0; x < MAP_WIDTH; ++x)
	{
		for (int y = 0; y < MAP_HEIGHT; ++y)
		{
			ShapeChangeByType(Vector2int(x, y));
		}
	}
}

void Map::Update()
{
	//for (int x = 0; x < MAP_WIDTH; ++x)
	//{
	//	for (int y = 0; y < MAP_HEIGHT; ++y)
	//	{
	//		if (Game::IsCollisionMouseRayAABB(data[y][x].model, data[y][x].transforms))
	//		{
	//			data[y][x].color = 0xFF0000FF;
	//		}
	//		else
	//		{
	//			data[y][x].color = 0xFFFFFFFF;
	//		}
	//	}
	//}
}

void Map::Draw()
{
	data[0][0].color = 0xFF0000FF;
	data[0][1].color = 0xFFFF00FF;
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
	if (index.y >= MAP_HEIGHT || index.y < 0 || index.x >= MAP_WIDTH || index.x < 0)
	{
		return;
	}

	if (blockType[index.y][index.x] == BLOCK_TYPE::Empty)
	{
		data[index.y][index.x].transforms.scale = Vector3{ 1.0f,1.0f,1.0f };
		data[index.y][index.x].transforms.rotate = Vector3{ 0.0f,0.0f,0.0f };
		data[index.y][index.x].transforms.translate = PositionByIndex(index);
		data[index.y][index.x].color = 0xFFFFFFFF;
	}
	else if (blockType[index.y][index.x] == BLOCK_TYPE::Wall)
	{
		data[index.y][index.x].transforms.scale = Vector3{ 1.0f,5.0f,1.0f };
		data[index.y][index.x].transforms.rotate = Vector3{ 0.0f,0.0f,0.0f };
		data[index.y][index.x].transforms.translate = PositionByIndex(index);
		data[index.y][index.x].transforms.translate.y += 0.4f;
		data[index.y][index.x].color = 0xFFFFFFFF;
	}
	else if (blockType[index.y][index.x] == BLOCK_TYPE::Asid)
	{
		data[index.y][index.x].transforms.scale = Vector3{ 1.0f,1.0f,1.0f };
		data[index.y][index.x].transforms.rotate = Vector3{ 0.0f,0.0f,0.0f };
		data[index.y][index.x].transforms.translate = PositionByIndex(index);
		data[index.y][index.x].color = 0xFF0000FF;
	}
}



BLOCK_TYPE Map::BlockTypeByIndex(Vector2int index)
{
	return blockType[index.y][index.x];
}
