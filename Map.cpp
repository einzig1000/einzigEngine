#include "Map.h"
#include <fstream>
#include <cassert>

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