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
			IsCollisionMouseRay_[y][x] = false;
			CharactorType[y][x] = BLOCK_CHAR::Empty;
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

#pragma region mapEnemy
		// パス作成
		std::ostringstream path_Enemy;
		path_Enemy << "resources/csv/map/map" << stageNum << "-Enemy.csv";

		// ファイルをひらく
		std::ifstream file_Enemy(path_Enemy.str());
		assert(file_Enemy.is_open());

		// ファイルの内容を丸ごとコピー
		std::ostringstream buffer_Enemy;
		buffer_Enemy << file_Enemy.rdbuf();
		mapEnemyCSV[stageNum] = buffer_Enemy.str();

		// ファイルを閉じる
		file_Enemy.close();

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
					EffectType[lineNumber_Effect][wordNumber_Effect] = BLOCK_EFFECT_TYPE::移動可能;
				}
			}
			wordNumber_Effect++;
		}
		lineNumber_Effect++;
	}

	// X軸反転
	FlipXAxis(EffectType);

#pragma endregion

#pragma region mapEnemy
	for (int x = 0; x < MAP_WIDTH; ++x)
	{
		for (int y = 0; y < MAP_HEIGHT; ++y)
		{
			InitializeEnemy[y][x] = 0;
		}
	}
	// 1行ずつ
	std::string line_Enemy;
	// ここで毎回新しいstringstreamを作る
	std::istringstream mapStream_Enemy(mapEnemyCSV[stageNum]);


	// ブロックタイプ適用
	int lineNumber_Enemy = 0;
	int wordNumber_Enemy = 0;
	while (getline(mapStream_Enemy, line_Enemy))
	{
		std::istringstream line_stream_Enemy(line_Enemy);
		std::string word_Enemy;
		wordNumber_Enemy = 0;

		while (getline(line_stream_Enemy, word_Enemy, ','))
		{
			if (lineNumber_Enemy < MAP_HEIGHT && wordNumber_Enemy < MAP_WIDTH)
			{
				InitializeEnemy[lineNumber_Enemy][wordNumber_Enemy] = std::stoi(word_Enemy);
			}
			wordNumber_Enemy++;
		}
		lineNumber_Enemy++;
	}

	// X軸反転
	FlipXAxis(InitializeEnemy);

#pragma endregion

}

void Map::Update()
{
	for (int x = 0; x < MAP_WIDTH; ++x)
	{
		for (int y = 0; y < MAP_HEIGHT; ++y)
		{
			if (EffectType[y][x] == BLOCK_EFFECT_TYPE::移動可能)
			{
				data[y][x].color = 0x69ff5dFF;
			}
			else if (EffectType[y][x] == BLOCK_EFFECT_TYPE::攻撃範囲)
			{
				// data[y][x].color = 0xFF6A6AFF;
				data[y][x].color = 0xFF0000FF;
			}
			else if (EffectType[y][x] == BLOCK_EFFECT_TYPE::回復範囲)
			{
				data[y][x].color = 0xFF69B4FF;
			}
			else
			{
				data[y][x].color = 0xFFFFFFFF;
			}

			// マウスに当たっていたら
			if (IsCollisionMouseRay_[y][x] == true)
			{
				data[y][x].color = 0xFF0000FF;
			}
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
	{
		for (int x = 0; x < MAP_WIDTH; ++x)
		{
			cost[y][x] = INF;
		}
	}

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
			{
				continue;
			}

			// 移動可能かチェック
			if (!A_to_B(current, next))
			{
				continue;
			}

			// 未訪問なら更新
			if (cost[next.y][next.x] == INF)
			{
				cost[next.y][next.x] = cost[current.y][current.x] + 1;
				q.push(next);

				// ゴールに到達したら即返す
				if (next == end)
				{
					return cost[next.y][next.x];
				}
			}
		}
	}

	// 到達不能
	return -1;
}

std::vector<Vector2int> Map::FindPath(Vector2int start, Vector2int end)
{
	if (start == end) return { start };

	const int INF = 1000;
	int cost[MAP_HEIGHT][MAP_WIDTH]{};
	Vector2int cameFrom[MAP_HEIGHT][MAP_WIDTH]{};

	for (int y = 0; y < MAP_HEIGHT; ++y)
	{
		for (int x = 0; x < MAP_WIDTH; ++x)
		{
			cost[y][x] = INF;
			cameFrom[y][x] = Vector2int(-1, -1); // 未訪問
		}
	}

	std::queue<Vector2int> q;
	q.push(start);
	cost[start.y][start.x] = 0;

	const Vector2int directions[4] = {
		{ 0, -1 }, { 0, 1 }, { -1, 0 }, { 1, 0 }
	};

	while (!q.empty())
	{
		Vector2int current = q.front();
		q.pop();

		for (const auto& dir : directions)
		{
			Vector2int next = current + dir;

			if (next.x < 0 || next.x >= MAP_WIDTH || next.y < 0 || next.y >= MAP_HEIGHT)
			{
				continue;
			}

			if (!A_to_B(current, next))
			{
				continue;
			}

			if (cost[next.y][next.x] == INF)
			{
				cost[next.y][next.x] = cost[current.y][current.x] + 1;
				cameFrom[next.y][next.x] = current;
				q.push(next);

				if (next == end)
				{
					goto reconstruct;
				}
			}
		}
	}

	// 到達不能
	return {};

reconstruct:
	std::vector<Vector2int> path;
	Vector2int current = end;
	while (current != start)
	{
		path.push_back(current);
		current = cameFrom[current.y][current.x];
	}
	path.push_back(start);
	std::reverse(path.begin(), path.end());
	return path;
}


BLOCK_TYPE Map::BlockTypeByIndex(Vector2int index)
{
	return blockType[index.y][index.x];
}

BLOCK_EFFECT_TYPE Map::BlockEffectByIndex(Vector2int index)
{
	return EffectType[index.y][index.x];
}

void Map::CheckAblemovement(Vector2int index, int idouhanni)
{
	for (int y = index.y - idouhanni; y <= index.y + idouhanni; ++y)
	{
		for (int x = index.x - idouhanni; x <= index.x + idouhanni; ++x)
		{
			// マップ内であれ
			if (x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT)
			{
				EffectType[y][x] = BLOCK_EFFECT_TYPE::Empty;
				// ターゲットまでの最短経路を求める
				int cost = shotestCost(index, Vector2int{ x,y });
				// 最短ルートが移動可能範囲を超えていなかったため移動可能
				if ((cost <= idouhanni && cost != -1))//|| (index == Vector2int{ x,y })
				{
					EffectType[y][x] = BLOCK_EFFECT_TYPE::移動可能;
				}
			}
		}
	}
}

void Map::CheckAbleAttack(Vector2int index, Skill skill, Direction direction)
{
	for (int x = 0; x < MAP_WIDTH; ++x)
	{
		for (int y = 0; y < MAP_HEIGHT; ++y)
		{
			EffectType[y][x] = BLOCK_EFFECT_TYPE::Empty;
		}
	}

	switch (skill.areaShape)
	{
	case SkillAreaShape::円:
	{
		for (int y = index.y - skill.range; y <= index.y + skill.range; ++y)
		{
			if (y >= 0 && y < MAP_HEIGHT)
			{
				EffectType[y][index.x] = BLOCK_EFFECT_TYPE::攻撃範囲;
			}
		}
		for (int x = index.x - skill.range; x <= index.x + skill.range; ++x)
		{
			if (x >= 0 && x < MAP_WIDTH)
			{
				EffectType[index.y][x] = BLOCK_EFFECT_TYPE::攻撃範囲;
			}
		}

		break;
	}

	case SkillAreaShape::直線:
	{
		Vector2int start = index;
		Vector2int end = index;

		switch (direction)
		{
		case Direction::None:
			break;
		case Direction::Left:
			end.x += skill.range;
			for (int x = start.x; x <= end.x; ++x)
			{
				if (x >= 0 && x < MAP_WIDTH)
				{
					if (blockType[start.y][start.x] == BLOCK_TYPE::Empty)
					{
						if (blockType[end.y][x] == BLOCK_TYPE::Empty)
						{
							EffectType[end.y][x] = BLOCK_EFFECT_TYPE::攻撃範囲;
						}
						else
						{
							break;
						}
					}
					else
					{
						if (blockType[end.y][x] == BLOCK_TYPE::Wall)
						{
							EffectType[end.y][x] = BLOCK_EFFECT_TYPE::攻撃範囲;
						}
					}
				}
			}
			break;
		case Direction::Right:
			end.x -= skill.range;
			for (int x = start.x; x >= end.x; --x)
			{
				if (x >= 0 && x < MAP_WIDTH)
				{
					if (blockType[start.y][start.x] == BLOCK_TYPE::Empty)
					{
						if (blockType[end.y][x] == BLOCK_TYPE::Empty)
						{
							EffectType[end.y][x] = BLOCK_EFFECT_TYPE::攻撃範囲;
						}
						else
						{
							break;
						}
					}
					else
					{
						if (blockType[end.y][x] == BLOCK_TYPE::Wall)
						{
							EffectType[end.y][x] = BLOCK_EFFECT_TYPE::攻撃範囲;
						}
					}
				}
			}
			break;
		case Direction::Down:
			end.y += skill.range;
			for (int y = start.y; y <= end.y; ++y)
			{
				if (y >= 0 && y < MAP_HEIGHT)
				{
					if (blockType[start.y][start.x] == BLOCK_TYPE::Empty)
					{
						if (blockType[y][end.x] == BLOCK_TYPE::Empty)
						{
							EffectType[y][end.x] = BLOCK_EFFECT_TYPE::攻撃範囲;
						}
						else
						{
							break;
						}
					}
					else
					{
						if (blockType[y][end.x] == BLOCK_TYPE::Wall)
						{
							EffectType[y][end.x] = BLOCK_EFFECT_TYPE::攻撃範囲;
						}
					}
				}
			}
			break;
		case Direction::Up:
			end.y -= skill.range;
			for (int y = start.y; y >= end.y; --y)
			{
				if (y >= 0 && y < MAP_HEIGHT)
				{
					if (blockType[start.y][start.x] == BLOCK_TYPE::Empty)
					{
						if (blockType[y][end.x] == BLOCK_TYPE::Empty)
						{
							EffectType[y][end.x] = BLOCK_EFFECT_TYPE::攻撃範囲;
						}
						else
						{
							break;
						}
					}
					else
					{
						if (blockType[y][end.x] == BLOCK_TYPE::Wall)
						{
							EffectType[y][end.x] = BLOCK_EFFECT_TYPE::攻撃範囲;
						}
					}
				}
			}
			break;
		default:
			break;
		}
		break;

	}

	case SkillAreaShape::十字:
	{
		Vector2int start = index;
		Vector2int end = index;

		end = index;
		end.x += skill.range;
		for (int x = start.x; x <= end.x; ++x)
		{
			if (x >= 0 && x < MAP_WIDTH)
			{
				if (blockType[start.y][start.x] == BLOCK_TYPE::Empty)
				{
					if (blockType[end.y][x] == BLOCK_TYPE::Empty)
					{
						EffectType[end.y][x] = BLOCK_EFFECT_TYPE::攻撃範囲;
					}
					else
					{
						break;
					}
				}
				else
				{
					if (blockType[end.y][x] == BLOCK_TYPE::Wall)
					{
						EffectType[end.y][x] = BLOCK_EFFECT_TYPE::攻撃範囲;
					}
				}
			}
		}

		end = index;
		end.x -= skill.range;
		for (int x = start.x; x >= end.x; --x)
		{
			if (x >= 0 && x < MAP_WIDTH)
			{
				if (blockType[start.y][start.x] == BLOCK_TYPE::Empty)
				{
					if (blockType[end.y][x] == BLOCK_TYPE::Empty)
					{
						EffectType[end.y][x] = BLOCK_EFFECT_TYPE::攻撃範囲;
					}
					else
					{
						break;
					}
				}
				else
				{
					if (blockType[end.y][x] == BLOCK_TYPE::Wall)
					{
						EffectType[end.y][x] = BLOCK_EFFECT_TYPE::攻撃範囲;
					}
				}
			}
		}

		end = index;
		end.y += skill.range;
		for (int y = start.y; y <= end.y; ++y)
		{
			if (y >= 0 && y < MAP_HEIGHT)
			{
				if (blockType[start.y][start.x] == BLOCK_TYPE::Empty)
				{
					if (blockType[y][end.x] == BLOCK_TYPE::Empty)
					{
						EffectType[y][end.x] = BLOCK_EFFECT_TYPE::攻撃範囲;
					}
					else
					{
						break;
					}
				}
				else
				{
					if (blockType[y][end.x] == BLOCK_TYPE::Wall)
					{
						EffectType[y][end.x] = BLOCK_EFFECT_TYPE::攻撃範囲;
					}
				}
			}
		}

		end = index;
		end.y -= skill.range;
		for (int y = start.y; y >= end.y; --y)
		{
			if (y >= 0 && y < MAP_HEIGHT)
			{
				if (blockType[start.y][start.x] == BLOCK_TYPE::Empty)
				{
					if (blockType[y][end.x] == BLOCK_TYPE::Empty)
					{
						EffectType[y][end.x] = BLOCK_EFFECT_TYPE::攻撃範囲;
					}
					else
					{
						break;
					}
				}
				else
				{
					if (blockType[y][end.x] == BLOCK_TYPE::Wall)
					{
						EffectType[y][end.x] = BLOCK_EFFECT_TYPE::攻撃範囲;
					}
				}
			}
		}

		break;

		//for (int y = index.y - skill.range; y <= index.y + skill.range; ++y)
		//{
		//	if (y >= 0 && y < MAP_HEIGHT)
		//	{
		//		EffectType[y][index.x] = BLOCK_EFFECT_TYPE::攻撃範囲;
		//	}
		//}
		//for (int x = index.x - skill.range; x <= index.x + skill.range; ++x)
		//{
		//	if (x >= 0 && x < MAP_WIDTH)
		//	{
		//		EffectType[index.y][x] = BLOCK_EFFECT_TYPE::攻撃範囲;
		//	}
		//}
		//break;
	}

	case SkillAreaShape::正方形:
	{
		for (int y = index.y - skill.range; y <= index.y + skill.range; ++y)
		{
			for (int x = index.x - skill.range; x <= index.x + skill.range; ++x)
			{
				// マップ内であれ
				if (x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT)
				{
					if (skill.type == SkillType::攻撃)
					{
						if (skill.passHeight)
						{
							EffectType[y][x] = BLOCK_EFFECT_TYPE::攻撃範囲;
						}
						else
						{
							if (blockType[index.y][index.x] == BLOCK_TYPE::Empty)
							{
								if (blockType[y][x] == BLOCK_TYPE::Empty)
								{
									EffectType[y][x] = BLOCK_EFFECT_TYPE::攻撃範囲;
								}
							}
							else if (blockType[index.y][index.x] == BLOCK_TYPE::Wall)
							{
								if (blockType[y][x] == BLOCK_TYPE::Wall)
								{
									EffectType[y][x] = BLOCK_EFFECT_TYPE::攻撃範囲;
								}
							}
							else if (blockType[index.y][index.x] == BLOCK_TYPE::stairs)
							{
								EffectType[y][x] = BLOCK_EFFECT_TYPE::攻撃範囲;
							}
						}
					}
					else if (skill.type == SkillType::回復)
					{
						if (skill.passHeight)
						{
							EffectType[y][x] = BLOCK_EFFECT_TYPE::回復範囲;
						}
						else
						{
							if (blockType[index.y][index.x] == BLOCK_TYPE::Empty)
							{
								if (blockType[y][x] == BLOCK_TYPE::Empty)
								{
									EffectType[y][x] = BLOCK_EFFECT_TYPE::回復範囲;
								}
							}
							else if (blockType[index.y][index.x] == BLOCK_TYPE::Wall)
							{
								if (blockType[y][x] == BLOCK_TYPE::Wall)
								{
									EffectType[y][x] = BLOCK_EFFECT_TYPE::回復範囲;
								}
							}
							else if (blockType[index.y][index.x] == BLOCK_TYPE::stairs)
							{
								EffectType[y][x] = BLOCK_EFFECT_TYPE::回復範囲;
							}
						}
					}
				}
			}
		}
		break;
	}

	case SkillAreaShape::前方正方形:
	{
		int localRange = skill.range;
		if (localRange % 2 == 0)localRange--;
		int dif = (localRange / 2) + 1;
		Vector2int localCenter = index;
		switch (direction)
		{
		case Direction::None:
			break;
		case Direction::Left:
			localCenter.x += dif;
			break;
		case Direction::Right:
			localCenter.x -= dif;
			break;
		case Direction::Down:
			localCenter.y += dif;
			break;
		case Direction::Up:
			localCenter.y -= dif;
			break;
		default:
			break;
		}
		for (int y = localCenter.y - (localRange / 2); y <= localCenter.y + (localRange / 2); ++y)
		{
			for (int x = localCenter.x - (localRange / 2); x <= localCenter.x + (localRange / 2); ++x)
			{
				// マップ内であれ
				if (x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT)
				{
					EffectType[y][x] = BLOCK_EFFECT_TYPE::攻撃範囲;
				}
			}
		}

		break;
	}

	case SkillAreaShape::例外:
	{
		EffectType[index.y][index.x] = BLOCK_EFFECT_TYPE::攻撃範囲;
	}
		break;
	default:
		break;
	}
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

	// 既にtargetマスに誰かいる
	if (CharactorType[target.y][target.x] == BLOCK_CHAR::OnEnemy || CharactorType[target.y][target.x] == BLOCK_CHAR::OnPlayer)
	{
		return false;
	}

	return true;
}
