#include "MapManager.h"
#include <fstream>
#include <sstream>
#include "Block/Block.h"
#include "Player.h"
#include "PerlinNoise.h"
#include "Engine.h"
#include "Itemslot.h"
#include "Block/BlockDurability.h"

MapManager::MapManager(Player* player)
{
	// プレイヤー参照保存
	player_ = player;

	// ブロック初期化
	for (int x = 0; x < MAX_BLOCK_X; x++)
	{
		for (int y = 0; y < MAX_BLOCK_Y; y++)
		{
			for (int z = 0; z < MAX_BLOCK_Z; z++)
			{
				block_[x][y][z] = new Block();
				block_[x][y][z]->Initialize();
				block_[x][y][z]->data_.name = "Block_" + std::to_string(x) + "_" + std::to_string(y) + "_" + std::to_string(z);
			}
		}
	}

	blockInfoMap_[BlockID::Stone] = { BlockID::Stone, 60 };
	blockInfoMap_[BlockID::Dirt] = { BlockID::Dirt, 30 };
	blockInfoMap_[BlockID::lawn] = { BlockID::lawn, 30 };
	blockInfoMap_[BlockID::Glass] = { BlockID::Glass, 10 };
	blockInfoMap_[BlockID::wood] = { BlockID::wood, 40 };
	blockInfoMap_[BlockID::leaf] = { BlockID::leaf, 20 };
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

	for (auto& item : dropItems_)
	{
		delete item;
		item = nullptr;
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

			Vector3 blockPosition;
			for (int y = 0; y < height - dirtThickness; y++)
			{
				block_[x][y][z]->SetBlockType(blockInfoMap_[BlockID::Stone]);
				blockPosition = Vector3(
					x * BLOCK_SIZE - (MAX_BLOCK_X - 1),
					y * BLOCK_SIZE - (MAX_BLOCK_Y - 1),
					z * BLOCK_SIZE - (MAX_BLOCK_Z - 1));
				block_[x][y][z]->SetBlockPosition(blockPosition);
			}
			for (int y = height - dirtThickness; y < height - 1; y++)
			{
				block_[x][y][z]->SetBlockType(blockInfoMap_[BlockID::Dirt]);
				blockPosition = Vector3(
					x * BLOCK_SIZE - (MAX_BLOCK_X - 1),
					y * BLOCK_SIZE - (MAX_BLOCK_Y - 1),
					z * BLOCK_SIZE - (MAX_BLOCK_Z - 1));
				block_[x][y][z]->SetBlockPosition(blockPosition);
			}
			for (int y = height - 1; y < height; y++)
			{
				block_[x][y][z]->SetBlockType(blockInfoMap_[BlockID::lawn]);
				blockPosition = Vector3(
					x * BLOCK_SIZE - (MAX_BLOCK_X - 1),
					y * BLOCK_SIZE - (MAX_BLOCK_Y - 1),
					z * BLOCK_SIZE - (MAX_BLOCK_Z - 1));
				block_[x][y][z]->SetBlockPosition(blockPosition);
			}
			for (int y = height; y < MAX_BLOCK_Y; y++)
			{
				block_[x][y][z]->SetBlockType(blockInfoMap_[BlockID::None]);
				blockPosition = Vector3(
					x * BLOCK_SIZE - (MAX_BLOCK_X - 1),
					y * BLOCK_SIZE - (MAX_BLOCK_Y - 1),
					z * BLOCK_SIZE - (MAX_BLOCK_Z - 1));
				block_[x][y][z]->SetBlockPosition(blockPosition);
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
	UpDataPlayerRayCollision();

	// マウス右ボタンが押されているフラグ
	bool isMouseRightHeld = Game::Input::Mouse::IsJustPressed(1);

	for (int x = 0; x < MAX_BLOCK_X; x++)
	{
		for (int y = 0; y < MAX_BLOCK_Y; y++)
		{
			for (int z = 0; z < MAX_BLOCK_Z; z++)
			{
				block_[x][y][z]->Update();
			}
		}
	}

	UpdatePlayerCollisionY();
	//UpdatePlayerCollisionXZ();
}

void MapManager::UpDataPlayerRayCollision()
{
	// モデルと衝突までの距離セット構造体
	struct HitInfo { Block* block; float distance; DirectionXYZ direction; };
	// のリスト
	std::vector<HitInfo> hits;
	// のリサイズ(リサイズではない)
	hits.reserve(size_t(MAX_BLOCK_X * MAX_BLOCK_Y * MAX_BLOCK_Z));

	for (int x = 0; x < MAX_BLOCK_X; x++)
	{
		for (int y = 0; y < MAX_BLOCK_Y; y++)
		{
			for (int z = 0; z < MAX_BLOCK_Z; z++)
			{
				block_[x][y][z]->isCollisionRay = -1;
				// 描画範囲内なら判定
				if (block_[x][y][z]->data_.inPicture)
				{
					// 最近接衝突点を取得
					std::optional<Vector3> colPos = IntersectRayModel(
						player_->viewRay_,
						Engine::Instance().GetAllObject3D()[block_[x][y][z]->data_.model].modelData.vertices,
						&block_[x][y][z]->data_
					);
					// 衝突していたらリストに登録
					if (colPos)
					{
						float minDistance = (colPos.value() - player_->viewRay_.origin).Length();
						DirectionXYZ dir = DirectionXYZ::None;
						if (colPos->x >= block_[x][y][z]->data_.translate.value.x + (BLOCK_SIZE / 2.0f) - 0.01f)
						{
							dir = DirectionXYZ::Right;
						}
						else if (colPos->x <= block_[x][y][z]->data_.translate.value.x - (BLOCK_SIZE / 2.0f) + 0.01f)
						{
							dir = DirectionXYZ::Left;
						}
						else if (colPos->y >= block_[x][y][z]->data_.translate.value.y + (BLOCK_SIZE / 2.0f) - 0.01f)
						{
							dir = DirectionXYZ::Up;
						}
						else if (colPos->y <= block_[x][y][z]->data_.translate.value.y - (BLOCK_SIZE / 2.0f) + 0.01f)
						{
							dir = DirectionXYZ::Down;
						}
						else if (colPos->z >= block_[x][y][z]->data_.translate.value.z + (BLOCK_SIZE / 2.0f) - 0.01f)
						{
							dir = DirectionXYZ::Front;
						}
						else if (colPos->z <= block_[x][y][z]->data_.translate.value.z - (BLOCK_SIZE / 2.0f) + 0.01f)
						{
							dir = DirectionXYZ::Back;
						}
						hits.push_back({ block_[x][y][z], minDistance, dir });
					}
				}
			}
		}
	}

	// 距離の昇順でソート
	std::sort(hits.begin(), hits.end(),
		[](auto& a, auto& b) { return a.distance < b.distance; });

	// ソート後に順序を割り当て
	for (int order = 0; order < (int)hits.size(); ++order)
	{
		hits[order].block->isCollisionRay = order;
		hits[order].block->collisionDistance = hits[order].distance;
		hits[order].block->direction = hits[order].direction;
	}
}

void MapManager::UpdatePlayerCollisionY()
{
#pragma region 下方向

	float playerHeight = player_->data_.aabbs[0].max.y - player_->data_.aabbs[0].min.y;

	Vector3 corners[4] = {
		Vector3(player_->data_.aabbs[0].min.x, player_->data_.aabbs[0].min.y - 0.1f, player_->data_.aabbs[0].min.z),
		Vector3(player_->data_.aabbs[0].max.x, player_->data_.aabbs[0].min.y - 0.1f, player_->data_.aabbs[0].min.z),
		Vector3(player_->data_.aabbs[0].min.x, player_->data_.aabbs[0].min.y - 0.1f, player_->data_.aabbs[0].max.z),
		Vector3(player_->data_.aabbs[0].max.x, player_->data_.aabbs[0].min.y - 0.1f, player_->data_.aabbs[0].max.z)
	};

	bool anyCollision = false;
	float bestGroundY = -999.9f;

	for (int i = 0; i < 4; ++i)
	{
		Vector3int idx = IndexByPosition(corners[i]);

		if (block_[idx.x][idx.y][idx.z]->data_.aabbs[0].max.y >= corners[i].y && !block_[idx.x][idx.y][idx.z]->durability_->GetIsDestroy())
		{
			anyCollision = true;
			float groundY = block_[idx.x][idx.y][idx.z]->data_.aabbs[0].max.y;

			if (groundY > bestGroundY) bestGroundY = groundY;
		}
	}

	if (anyCollision)
	{
		// translate.value.y を地面上に設定する（translate が中心なら center.y = groundY + height/2）
		player_->data_.translate.value.y = bestGroundY + playerHeight * 0.5f;

		// AABB を center から再計算するか、min/max を更新する
		player_->data_.aabbs[0].min.y = player_->data_.translate.value.y - playerHeight * 0.5f;
		player_->data_.aabbs[0].max.y = player_->data_.translate.value.y + playerHeight * 0.5f;

		player_->data_.translate.velocity.y = 0.0f;
		player_->data_.translate.acceleration.y = 0.0f;
	}
	else
	{
		player_->data_.translate.acceleration.y = GRAVITY;
	}

#pragma endregion
}

void MapManager::UpdatePlayerCollisionZ()
{
#pragma region 前方向

	float playerWidth = player_->data_.aabbs[0].max.z - player_->data_.aabbs[0].min.z;

	Vector3 corners[4] = {
		Vector3(player_->data_.aabbs[0].min.x, player_->data_.aabbs[0].min.y + 0.1f, player_->data_.aabbs[0].min.z + 0.1f),
		Vector3(player_->data_.aabbs[0].max.x, player_->data_.aabbs[0].min.y + 0.1f, player_->data_.aabbs[0].min.z + 0.1f),
		Vector3(player_->data_.aabbs[0].min.x, player_->data_.aabbs[0].max.y - 0.1f, player_->data_.aabbs[0].min.z + 0.1f),
		Vector3(player_->data_.aabbs[0].max.x, player_->data_.aabbs[0].max.y - 0.1f, player_->data_.aabbs[0].min.z + 0.1f)
	};

	bool anyCollision = false;
	float bestFrontZ = 9999.9f;

	for (int i = 0; i < 4; ++i)
	{
		Vector3int idx = IndexByPosition(corners[i]);
		if (block_[idx.x][idx.y][idx.z]->data_.aabbs[0].min.z <= corners[i].z && !block_[idx.x][idx.y][idx.z]->durability_->GetIsDestroy())
		{
			anyCollision = true;
			float frontZ = block_[idx.x][idx.y][idx.z]->data_.aabbs[0].min.z;
			if (frontZ < bestFrontZ) bestFrontZ = frontZ;
		}
	}

	if (anyCollision)
	{
		// translate.value.z を地面上に設定する（translate が中心なら center.z = frontZ - width/2）
		player_->data_.translate.value.z = bestFrontZ - playerWidth * 0.5f;
		// AABB を center から再計算するか、min/max を更新する
		player_->data_.aabbs[0].min.z = player_->data_.translate.value.z - playerWidth * 0.5f;
		player_->data_.aabbs[0].max.z = player_->data_.translate.value.z + playerWidth * 0.5f;
		if (player_->data_.translate.velocity.z > 0)
		{
			player_->data_.translate.velocity.z = 0.0f;
		}
	}

#pragma endregion
}

void MapManager::UpdatePlayerCollisionX()
{
#pragma region 右方向

	float playerWidth = player_->data_.aabbs[0].max.x - player_->data_.aabbs[0].min.x;

	Vector3 corners[4] = {
		Vector3(player_->data_.aabbs[0].max.x + 0.1f, player_->data_.aabbs[0].min.y + 0.1f, player_->data_.aabbs[0].min.z),
		Vector3(player_->data_.aabbs[0].max.x + 0.1f, player_->data_.aabbs[0].min.y + 0.1f, player_->data_.aabbs[0].max.z),
		Vector3(player_->data_.aabbs[0].max.x + 0.1f, player_->data_.aabbs[0].max.y - 0.1f, player_->data_.aabbs[0].min.z),
		Vector3(player_->data_.aabbs[0].max.x + 0.1f, player_->data_.aabbs[0].max.y - 0.1f, player_->data_.aabbs[0].max.z)
	};

	bool anyCollision = false;
	float bestFrontX = 9999.9f;

	for (int i = 0; i < 4; ++i)
	{
		Vector3int idx = IndexByPosition(corners[i]);
		if (block_[idx.x][idx.y][idx.z]->data_.aabbs[0].min.z <= corners[i].z && !block_[idx.x][idx.y][idx.z]->durability_->GetIsDestroy())
		{
			anyCollision = true;
			float frontX = block_[idx.x][idx.y][idx.z]->data_.aabbs[0].min.z;
			if (frontX < bestFrontX) bestFrontX = frontX;
		}
	}

	if (anyCollision)
	{
		// translate.value.z を地面上に設定する（translate が中心なら center.z = frontZ - width/2）
		player_->data_.translate.value.x = bestFrontX - playerWidth * 0.5f;
		// AABB を center から再計算するか、min/max を更新する
		player_->data_.aabbs[0].min.x = player_->data_.translate.value.x - playerWidth * 0.5f;
		player_->data_.aabbs[0].max.x = player_->data_.translate.value.x + playerWidth * 0.5f;
		if (player_->data_.translate.velocity.x > 0)
		{
			player_->data_.translate.velocity.x = 0.0f;
		}
	}

#pragma endregion

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

	for (auto& item : dropItems_)
	{
		item->Draw();
	}
}

Vector3int MapManager::IndexByPosition(const Vector3& position)
{
	Vector3int index;
	index.x = static_cast<int>(((position.x + (MAX_BLOCK_X - 1)) / BLOCK_SIZE) + (BLOCK_SIZE / 2.0f));
	index.y = static_cast<int>(((position.y + (MAX_BLOCK_Y - 1)) / BLOCK_SIZE));
	index.z = static_cast<int>(((position.z + (MAX_BLOCK_Z - 1)) / BLOCK_SIZE) + (BLOCK_SIZE / 2.0f));

	if (index.x < 0)index.x = 0;
	else if (index.x > MAX_BLOCK_X - 1)index.x = MAX_BLOCK_X - 1;
	if (index.y < 0)index.y = 0;
	else if (index.y > MAX_BLOCK_Y - 1)index.y = MAX_BLOCK_Y - 1;
	if (index.z < 0)index.z = 0;
	else if (index.z > MAX_BLOCK_Z - 1)index.z = MAX_BLOCK_Z - 1;

	return index;
}
