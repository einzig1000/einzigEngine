#include "Map.h"
#include <fstream>
#include <cassert>
#include <algorithm>
#include <queue>
#include <limits>

Map::Map()
{
	for (int x = 0; x < MAP_WIDTH; ++x)
	{
		for (int y = 0; y < MAP_HEIGHT; ++y)
		{
			data[y][x].model = uint32_t(TEXTURE::Map_Block);
			data[y][x].texture = uint32_t(TEXTURE::Map_Block);
			data[y][x].transforms.translate = PositionByIndex(Vector2int(x, y));
			blockType[y][x] = BLOCK_TYPE::Empty;
		}
	}
}

void Map::Initialize(){}

void Map::LoadMap(int stageNum)
{
	if (stageNum >= STAGE_MAX)return;

	// 既に読み込み済だったらスキップ
	if (mapCSV[stageNum].empty())
	{
#pragma region mapBlock
		// パス作成
		std::ostringstream path_Block;
		path_Block << "resources/csv/map/map" << stageNum << ".csv";

		// ファイルをひらく
		std::ifstream file_Block(path_Block.str());
		assert(file_Block.is_open());

		// ファイルの内容を丸ごとコピー
		std::ostringstream buffer_Block;
		buffer_Block << file_Block.rdbuf();
		mapCSV[stageNum] = buffer_Block.str();

		// ファイルを閉じる
		file_Block.close();
#pragma endregion

#pragma region mapEffect
		// パス作成
		std::ostringstream path_Effect;
		path_Effect << "resources/csv/map/map" << stageNum << "-Initialize.csv";

		// ファイルをひらく
		std::ifstream file_Effect(path_Effect.str());
		assert(file_Effect.is_open());

		// ファイルの内容を丸ごとコピー
		std::ostringstream buffer_Effect;
		buffer_Effect << file_Effect.rdbuf();
		mapEffectCSV[stageNum] = buffer_Effect.str();

		// ファイルを閉じる
		file_Effect.close();
#pragma endregion

	}

#pragma region mapBlock
	// 1行ずつ
	std::string line_Block;
	// ここで毎回新しいstringstreamを作る
	std::istringstream mapStream_Block(mapCSV[stageNum]);


	// ブロックタイプ適用
	int lineNumber_Block = 0;
	int wordNumber_Block = 0;
	while (getline(mapStream_Block, line_Block))
	{
		std::istringstream line_stream(line_Block);
		std::string word_Block;
		wordNumber_Block = 0;

		while (getline(line_stream, word_Block, ','))
		{
			if (lineNumber_Block < MAP_HEIGHT && wordNumber_Block < MAP_WIDTH)
			{
				if (word_Block.find("0") == 0)
				{
					blockType[lineNumber_Block][wordNumber_Block] = BLOCK_TYPE::Empty;
				}
				else if (word_Block.find("1") == 0)
				{
					blockType[lineNumber_Block][wordNumber_Block] = BLOCK_TYPE::Wall;
				}
				else if (word_Block.find("2") == 0)
				{
					blockType[lineNumber_Block][wordNumber_Block] = BLOCK_TYPE::stairs;
				}
			}
			wordNumber_Block++;
		}
		lineNumber_Block++;
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
#pragma endregion

#pragma region mapEffect
	// 1行ずつ
	std::string line_Effect;
	// ここで毎回新しいstringstreamを作る
	std::istringstream mapStream_Effect(mapEffectCSV[stageNum]);


	// ブロックタイプ適用
	int lineNumber_Effect = 0;
	int wordNumber_Effect = 0;
	while (getline(mapStream_Effect, line_Effect))
	{
		std::istringstream line_stream_Effect(line_Effect);
		std::string word_Effect;
		wordNumber_Effect = 0;

		while (getline(line_stream_Effect, word_Effect, ','))
		{
			if (lineNumber_Effect < MAP_HEIGHT && wordNumber_Effect < MAP_WIDTH)
			{
				if (word_Effect.find("0") == 0)
				{
					EffectType[lineNumber_Effect][wordNumber_Effect] = BLOCK_EFFECT_TYPE::Empty;
				}
				else if (word_Effect.find("1") == 0)
				{
					EffectType[lineNumber_Effect][wordNumber_Effect] = BLOCK_EFFECT_TYPE::AbleCharactorSet;
				}
			}
			wordNumber_Effect++;
		}
		lineNumber_Effect++;
	}

	// X軸反転
	FlipXAxis(EffectType);

#pragma endregion
}

void Map::Update()
{
	//for (int x = 0; x < MAP_WIDTH; ++x)
	//{
	//	for (int y = 0; y < MAP_HEIGHT; ++y)
	//	{
	//		if (EffectType[y][x] == BLOCK_EFFECT_TYPE::AbleCharactorSet)
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
		data[index.y][index.x].LookAtFront();
		data[index.y][index.x].color = 0xFFFFFFFF;
	}
	else if (blockType[index.y][index.x] == BLOCK_TYPE::Wall)
	{
		data[index.y][index.x].transforms.scale = Vector3{ 1.0f,5.0f,1.0f };
		data[index.y][index.x].transforms.rotate = Vector3{ 0.0f,0.0f,0.0f };
		data[index.y][index.x].transforms.translate = PositionByIndex(index);
		data[index.y][index.x].transforms.translate.y += 0.4f;
		data[index.y][index.x].LookAtFront();
		data[index.y][index.x].color = 0xFFFFFFFF;
	}
	else if (blockType[index.y][index.x] == BLOCK_TYPE::stairs)
	{
		data[index.y][index.x].transforms.scale = Vector3{ 1.0f,2.5f,1.0f };
		data[index.y][index.x].transforms.rotate = Vector3{ 0.0f,0.0f,0.0f };
		data[index.y][index.x].transforms.translate = PositionByIndex(index);
		data[index.y][index.x].transforms.translate.y += 0.2f;
		data[index.y][index.x].LookAtFront();
		data[index.y][index.x].color = 0xFF0000FF;
	}
}


int Map::shotestCost(Vector2int start, Vector2int end)
{
	if (start == end) return 0;

	const int INF = 1000;
	int cost[MAP_HEIGHT][MAP_WIDTH];
	for (int y = 0; y < MAP_HEIGHT; ++y)
		for (int x = 0; x < MAP_WIDTH; ++x)
			cost[y][x] = INF;

	std::queue<Vector2int> q;
	q.push(start);
	cost[start.y][start.x] = 0;

	const Vector2int directions[4] = {
		{ 0, -1 }, // 上
		{ 0,  1 }, // 下
		{ -1, 0 }, // 左
		{ 1,  0 }  // 右
	};

	while (!q.empty())
	{
		Vector2int current = q.front();
		q.pop();

		for (const auto& dir : directions)
		{
			Vector2int next = current + dir;

			// 範囲外チェック
			if (next.x < 0 || next.x >= MAP_WIDTH || next.y < 0 || next.y >= MAP_HEIGHT)
				continue;

			// 移動可能かチェック
			if (!A_to_B(current, next))
				continue;

			// 未訪問なら更新
			if (cost[next.y][next.x] == INF)
			{
				cost[next.y][next.x] = cost[current.y][current.x] + 1;
				q.push(next);

				// ゴールに到達したら即返す
				if (next == end)
					return cost[next.y][next.x];
			}
		}
	}

	// 到達不能
	return -1;

}

BLOCK_TYPE Map::BlockTypeByIndex(Vector2int index)
{
	return blockType[index.y][index.x];
}

BLOCK_EFFECT_TYPE Map::BlockEffectByIndex(Vector2int index)
{
	return EffectType[index.y][index.x];
}

bool Map::A_to_B(Vector2int start, Vector2int target)
{
	if (blockType[start.y][start.x] == BLOCK_TYPE::Empty && blockType[target.y][target.x] == BLOCK_TYPE::Wall)
	{
		return false;
	}
	if (blockType[start.y][start.x] == BLOCK_TYPE::Wall && blockType[target.y][target.x] == BLOCK_TYPE::Empty)
	{
		return false;
	}

	return true;
}
