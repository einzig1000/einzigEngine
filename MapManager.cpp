#include "MapManager.h"
#include <fstream>
#include <sstream>
#include "Block.h"
#include "Player.h"
#include "PerlinNoise.h"
#include "ResourceID.h"

MapManager::MapManager(Player* player)
{
	// プレイヤー参照保存
	player_ = player;

	// ブロック破壊テクスチャ初期化
	for (int i = 0; i < 6; i++)
	{
		blockRect_[i].texture = ResourceID::blockTextureIDs_[int(BlockTextureID::BreakBlock_0)];
	}

	// ブロック初期化
	for (int x = 0; x < MAX_BLOCK_X; x++)
	{
		for (int y = 0; y < MAX_BLOCK_Y; y++)
		{
			for (int z = 0; z < MAX_BLOCK_Z; z++)
			{
				block_[x][y][z] = new Block();
				block_[x][y][z]->Initialize(
					Vector3(
						x * BLOCK_SIZE - (MAX_BLOCK_X - 1),
						y * BLOCK_SIZE - (MAX_BLOCK_Y - 1),
						z * BLOCK_SIZE - (MAX_BLOCK_Z - 1))
				);
				block_[x][y][z]->model_.name = "Block_" + std::to_string(x) + "_" + std::to_string(y) + "_" + std::to_string(z);
				block_[x][y][z]->model_.mass = 100.0f;
				player->data_.SetBlock(block_[x][y][z]->model_);
			}
		}
	}

	// 破壊テクスチャ用矩形配置
	AABB blockAABB;
	blockAABB.min = Vector3(-BLOCK_SIZE / 2.0f - (BLOCK_SIZE / 100.0f), -BLOCK_SIZE / 2.0f - (BLOCK_SIZE / 100.0f), -BLOCK_SIZE / 2.0f - (BLOCK_SIZE / 100.0f));
	blockAABB.max = Vector3(BLOCK_SIZE / 2.0f + (BLOCK_SIZE / 100.0f), BLOCK_SIZE / 2.0f + (BLOCK_SIZE / 100.0f), BLOCK_SIZE / 2.0f + (BLOCK_SIZE / 100.0f));

	blockRect_[0].pos1 = Vector3(blockAABB.max.x, blockAABB.max.y, blockAABB.min.z);
	blockRect_[0].pos2 = Vector3(blockAABB.max.x, blockAABB.min.y, blockAABB.min.z);
	blockRect_[0].pos3 = Vector3(blockAABB.min.x, blockAABB.max.y, blockAABB.min.z);
	blockRect_[0].pos4 = Vector3(blockAABB.min.x, blockAABB.min.y, blockAABB.min.z);

	blockRect_[1].pos1 = Vector3(blockAABB.min.x, blockAABB.max.y, blockAABB.max.z);
	blockRect_[1].pos2 = Vector3(blockAABB.min.x, blockAABB.min.y, blockAABB.max.z);
	blockRect_[1].pos3 = Vector3(blockAABB.max.x, blockAABB.max.y, blockAABB.max.z);
	blockRect_[1].pos4 = Vector3(blockAABB.max.x, blockAABB.min.y, blockAABB.max.z);

	blockRect_[2].pos1 = Vector3(blockAABB.max.x, blockAABB.max.y, blockAABB.max.z);
	blockRect_[2].pos2 = Vector3(blockAABB.max.x, blockAABB.min.y, blockAABB.max.z);
	blockRect_[2].pos3 = Vector3(blockAABB.max.x, blockAABB.max.y, blockAABB.min.z);
	blockRect_[2].pos4 = Vector3(blockAABB.max.x, blockAABB.min.y, blockAABB.min.z);

	blockRect_[3].pos1 = Vector3(blockAABB.min.x, blockAABB.max.y, blockAABB.min.z);
	blockRect_[3].pos2 = Vector3(blockAABB.min.x, blockAABB.min.y, blockAABB.min.z);
	blockRect_[3].pos3 = Vector3(blockAABB.min.x, blockAABB.max.y, blockAABB.max.z);
	blockRect_[3].pos4 = Vector3(blockAABB.min.x, blockAABB.min.y, blockAABB.max.z);

	blockRect_[4].pos1 = Vector3(blockAABB.min.x, blockAABB.max.y, blockAABB.max.z);
	blockRect_[4].pos2 = Vector3(blockAABB.min.x, blockAABB.max.y, blockAABB.min.z);
	blockRect_[4].pos3 = Vector3(blockAABB.max.x, blockAABB.max.y, blockAABB.max.z);
	blockRect_[4].pos4 = Vector3(blockAABB.max.x, blockAABB.max.y, blockAABB.min.z);

	blockRect_[5].pos1 = Vector3(blockAABB.max.x, blockAABB.min.y, blockAABB.min.z);
	blockRect_[5].pos2 = Vector3(blockAABB.max.x, blockAABB.min.y, blockAABB.max.z);
	blockRect_[5].pos3 = Vector3(blockAABB.min.x, blockAABB.min.y, blockAABB.min.z);
	blockRect_[5].pos4 = Vector3(blockAABB.min.x, blockAABB.min.y, blockAABB.max.z);
}

MapManager::~MapManager()
{
	for (int x = 0; x < MAX_BLOCK_X; x++)
	{
		for (int y = 0; y < MAX_BLOCK_Y; y++)
		{
			for (int z = 0; z < MAX_BLOCK_Z; z++)
			{
				delete block_[x][y][z];
				block_[x][y][z] = nullptr;
			}
		}
	}
}

void MapManager::LoadMap(const std::string& mapFilePath)
{
	std::ifstream file(mapFilePath);

	// ファイルが存在しない場合は新規作成
	if (!file.is_open())
	{
		std::ofstream createFile(mapFilePath);

		const int width = MAX_BLOCK_X;
		const int depth = MAX_BLOCK_Z;
		const int maxHeight = MAX_BLOCK_Y;
		const double scale = 6.0;
		const int octaves = 4;
		const double persistence = 0.5;
		const unsigned int seed = 12345;

		PerlinNoise pn(seed);

		// 高さマップを作る
		std::vector<std::vector<int>> heightmap(width, std::vector<int>(depth, 0));
		for (int x = 0; x < width; ++x)
		{
			for (int z = 0; z < depth; ++z)
			{
				double sampleX = static_cast<double>(x) / scale;
				double sampleZ = static_cast<double>(z) / scale;
				double n = fractalPerlin(pn, sampleX, sampleZ, octaves, persistence); // 0..1
				int h = static_cast<int>(std::floor(n * (maxHeight - 1) + 0.5)); // 0..maxHeight-1
				if (h < 0) h = 0;
				if (h > maxHeight - 1) h = maxHeight - 1;
				heightmap[x][z] = h;
			}
		}

		for (int x = 0; x < width; ++x)
		{
			for (int z = 0; z < depth; ++z)
			{
				createFile << heightmap[x][z] << ",";
			}
			createFile << "\n";
		}

		createFile.close();
		file.open(mapFilePath);
	}

	std::string line;
	int X = 0;
	int Y = 0;

	// [x][z]の高さを読み込み
	while (std::getline(file, line))
	{
		std::istringstream ss(line);
		std::string BlockID;
		while (std::getline(ss, BlockID, ','))
		{
			int id = std::stoi(BlockID);

			// blockHeightMap_に高さを保存
			blockHeightMap_[X][Y] = id;

			X++;
			if (X >= MAX_BLOCK_X)
			{
				X = 0;
				Y++;
				if (Y >= MAX_BLOCK_Z)
				{
					break;
				}
			}
		}
	}

	file.close();

	// [x][z]に高さ分だけブロックを生成
	for (int x = 0; x < MAX_BLOCK_X; x++)
	{
		for (int z = 0; z < MAX_BLOCK_Z; z++)
		{
			// Y軸の高さ
			int height = blockHeightMap_[x][z];
			// 土の厚さ
			int dirtThickness = Game::Math::RandInt(2, 3);
			// 高さより土の厚さが大きい場合は調整
			if (height - dirtThickness < 0)
			{
				dirtThickness = height;
			}

			for (int y = 0; y < height - dirtThickness; y++)
			{
				block_[x][y][z]->isDestroy_ = false;
				block_[x][y][z]->model_.texture = ResourceID::blockTextureIDs_[int(BlockTextureID::Stone)];
				block_[x][y][z]->maxDurability_ = 60;
			}
			for (int y = height - dirtThickness; y < height - 1; y++)
			{
				block_[x][y][z]->isDestroy_ = false;
				block_[x][y][z]->model_.texture = ResourceID::blockTextureIDs_[int(BlockTextureID::Dirt)];
				block_[x][y][z]->maxDurability_ = 30;
			}
			for (int y = height - 1; y < height; y++)
			{
				block_[x][y][z]->isDestroy_ = false;
				block_[x][y][z]->model_.texture = ResourceID::blockTextureIDs_[int(BlockTextureID::lawn)];
				block_[x][y][z]->maxDurability_ = 30;
			}
			for (int y = height; y < MAX_BLOCK_Y; y++)
			{
				block_[x][y][z]->isDestroy_ = true;
				block_[x][y][z]->model_.texture = ResourceID::blockTextureIDs_[int(BlockTextureID::Stone)];
				block_[x][y][z]->model_.scale.value = Vector3(0.001f, 0.001f, 0.001f);
				block_[x][y][z]->maxDurability_ = 60;
			}
		}
	}

	for (int x = 0; x < MAX_BLOCK_X; x++)
	{
		for (int z = 0; z < MAX_BLOCK_Z; z++)
		{
			int height = blockHeightMap_[x][z];
			int neighborHeights[4] = { 0,0,0,0 };
			if (x + 1 < MAX_BLOCK_X)neighborHeights[0] = blockHeightMap_[x + 1][z];
			if (x - 1 >= 0)neighborHeights[1] = blockHeightMap_[x - 1][z];
			if (z + 1 < MAX_BLOCK_Z)neighborHeights[2] = blockHeightMap_[x][z + 1];
			if (z - 1 >= 0)neighborHeights[3] = blockHeightMap_[x][z - 1];

			int maxHeightGap = 0;
			for (int i = 0; i < 4; i++)
			{
				int gap = height - neighborHeights[i];
				if (gap > maxHeightGap)
				{
					maxHeightGap = gap;
				}
			}

			//block_[x][height - 1][z]->isExposed_ = true;
			// blockHeightMap_[x][z]の上からmaxHeightGap分だけ下まで露出している
			for (int i = 0; i <= maxHeightGap; i++)
			{
				if (height - 1 - i < 0) break;
				block_[x][height - 1 - i][z]->isExposed_ = true;
			}
		}
	}
}

void MapManager::Initialize()
{

}

void MapManager::Update()
{
	isBeingDestroyed_ = false;
	for (int x = 0; x < MAX_BLOCK_X; x++)
	{
		for (int y = 0; y < MAX_BLOCK_Y; y++)
		{
			for (int z = 0; z < MAX_BLOCK_Z; z++)
			{
				block_[x][y][z]->Update();
				// 破壊中
				if (block_[x][y][z]->isBeingDestroyed_ && block_[x][y][z]->model_.isCollisionMouseRay == 1)
				{
					// 破壊中ブロックの周りに破壊テクスチャを配置
					blockTriangleTransform_ = Transforms(
						Vector3(1.0f, 1.0f, 1.0f),
						block_[x][y][z]->model_.rotate.value,
						block_[x][y][z]->model_.translate.value);

					破壊中ブロックのVector3 = block_[x][y][z]->model_.translate.value;
					破壊エフェクトのVector3 = blockTriangleTransform_.translate;

					// 破壊割合に応じたテクスチャ更新
					for (int i = 0; i < 6; i++)
					{
						// 0.0f ~ 1.0f
						float t = float(block_[x][y][z]->nowDurability_) / float(block_[x][y][z]->maxDurability_);
						// 0.0f ~ 10.0f
						float percent = t * 10.0f;
						// 0.0f ~ 5.0f
						float frameF = percent / 2.0f;
						// int変換
						int frame = int(5 - frameF);
						if (frame > 4)frame = 4;

						int tagetID = int(BlockTextureID::BreakBlock_0) + frame;
						blockRect_[i].texture = ResourceID::blockTextureIDs_[tagetID];
					}

					// 破壊中フラグ乱立
					isBeingDestroyed_ = true;
				}
				// 破壊された瞬間
				if (block_[x][y][z]->isJustDestroyed_)
				{
					if (x < MAX_BLOCK_X - 1)
					{
						block_[x + 1][y][z]->isExposed_ = true;
					}
					if (x > 0)
					{
						block_[x - 1][y][z]->isExposed_ = true;
					}
					if (y < MAX_BLOCK_Y - 1)
					{
						block_[x][y + 1][z]->isExposed_ = true;
					}
					if (y > 0)
					{
						block_[x][y - 1][z]->isExposed_ = true;
					}
					if (z < MAX_BLOCK_Z - 1)
					{
						block_[x][y][z + 1]->isExposed_ = true;
					}
					if (z > 0)
					{
						block_[x][y][z - 1]->isExposed_ = true;
					}
				}
			}
		}
	}

	for (int i = 0; i < 6; i++)
	{
		blockRect_[i].transforms = blockTriangleTransform_;
	}
}

void MapManager::Draw()
{
	for (int x = 0; x < MAX_BLOCK_X; x++)
	{
		for (int y = 0; y < MAX_BLOCK_Y; y++)
		{
			for (int z = 0; z < MAX_BLOCK_Z; z++)
			{
				block_[x][y][z]->Draw();
			}
		}
	}

	//if (isBeingDestroyed_)
	{
		for (int i = 0; i < 6; i++)
		{
			blockRect_[i].Draw();
		}
	}
	ImGui::Text("破壊中ブロックの座標: (%.2f, %.2f, %.2f)", 破壊中ブロックのVector3.x, 破壊中ブロックのVector3.y, 破壊中ブロックのVector3.z);
	ImGui::Text("破壊エフェクトの座標: (%.2f, %.2f, %.2f)", 破壊エフェクトのVector3.x, 破壊エフェクトのVector3.y, 破壊エフェクトのVector3.z);
}