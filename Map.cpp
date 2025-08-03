#include "Map.h"
#include <fstream>
#include <cassert>

Map::Map()
{


}

void Map::Initialize(int stageNum)
{
	if (mapCSV[0].str().empty())
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


}

void Map::Update()
{

}


void Map::Draw()
{




}