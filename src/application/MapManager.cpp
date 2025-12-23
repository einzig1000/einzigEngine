#include "MapManager.h"
#include <fstream>
#include <sstream>
#include "Block/Block.h"
#include "Player.h"
#include "PerlinNoise.h"
#include "Engine.h"
#include "Itemslot.h"
#include "Block/BlockDurability.h"
#include "Block/BlockConfig.h"

MapManager::MapManager(Player* player)
{
	// プレイヤー参照保存
	player_ = player;

	for (int x = 0; x < CHUNK_X; x++)
	{
		for (int y = 0; y < CHUNK_Z; y++)
		{
			for (int z = 0; z < CHUNK_Y; z++)
			{
				block_[x][y][z] = new Block();
				block_[x][y][z]->Initialize();
				block_[x][y][z]->aabb_ = AABBByIndex(Vector3int(x, y, z));
			}
		}
	}

	for (int32_t i = 0; i < int32_t(BlockID::MAX); ++i)
	{
		blockData_[BlockID(i)] = std::make_unique<RenderData_Block>(BlockID(i));
		blockData_[BlockID(i)]->texture = ResourceID::GetTextureID(BlockID(i));
		blockData_[BlockID(i)]->additionalTexture = ResourceID::GetTextureID(TextureID::BreakBlock_1);
		blockData_[BlockID(i)]->model = ResourceID::GetModelID(ModelID::Cube);

		blockData_[BlockID(i)]->currentDrawSum = 0;
		blockData_[BlockID(i)]->currentSum = 0;
	}

	blockConfig_ = new BlockConfig();
}

MapManager::~MapManager()
{
	delete blockConfig_;
	blockConfig_ = nullptr;

	for (int x = 0; x < CHUNK_X; x++)
	{
		for (int y = 0; y < CHUNK_Z; y++)
		{
			for (int z = 0; z < CHUNK_Y; z++)
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

		const int width = CHUNK_X;
		const int depth = CHUNK_Y;
		const int maxHeight = CHUNK_Z;
		const float scale = 6.0f;
		const int octaves = 4;
		const float persistence = 0.5f;
		const unsigned int seed = 12345;

		PerlinNoise pn(seed);

		// 高さマップを作る
		std::vector<std::vector<int>> heightmap(width, std::vector<int>(depth, 0));
		for (int x = 0; x < width; ++x)
		{
			for (int z = 0; z < depth; ++z)
			{
				float sampleX = static_cast<float>(x) / scale;
				float sampleZ = static_cast<float>(z) / scale;
				float n = fractalPerlin(pn, sampleX, sampleZ, octaves, persistence); // 0..1
				int h = static_cast<int>(std::floor(n * float(maxHeight - 1) + 0.5f)); // 0..maxHeight-1
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

	int blockHeightMap_[CHUNK_X][CHUNK_Y];

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
			if (X >= CHUNK_X)
			{
				X = 0;
				Y++;
				if (Y >= CHUNK_Y)
				{
					break;
				}
			}
		}
	}

	file.close();

	// [x][z]に高さ分だけブロックを生成
	for (int x = 0; x < CHUNK_X; x++)
	{
		for (int z = 0; z < CHUNK_Y; z++)
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

			//Vector3 blockPosition;
			//BlockID blockID = BlockID::Air;
			//for (int y = 0; y < height - dirtThickness; y++)
			//{
			//	blockID = BlockID::Stone;
			//	block_[x][y][z]->SetBlockType(blockConfig_->GetBlockInfo(blockID));
			//	blockPosition = PositionByIndex(Vector3int(x, y, z));
			//	block_[x][y][z]->SetBlockPosition(blockPosition);
			//	block_[x][y][z]->instanceIndex_ = blockData_[blockID]->currentSum;
			//	blockData_[blockID]->currentSum++;
			//}
			//for (int y = height - dirtThickness; y < height - 1; y++)
			//{
			//	blockID = BlockID::Dirt;
			//	block_[x][y][z]->SetBlockType(blockConfig_->GetBlockInfo(blockID));
			//	blockPosition = PositionByIndex(Vector3int(x, y, z));
			//	block_[x][y][z]->SetBlockPosition(blockPosition);
			//	block_[x][y][z]->instanceIndex_ = blockData_[blockID]->currentSum;
			//	blockData_[blockID]->currentSum++;
			//}
			//for (int y = height - 1; y < height; y++)
			//{
			//	blockID = BlockID::Lawn;
			//	block_[x][y][z]->SetBlockType(blockConfig_->GetBlockInfo(blockID));
			//	blockPosition = PositionByIndex(Vector3int(x, y, z));
			//	block_[x][y][z]->SetBlockPosition(blockPosition);
			//	block_[x][y][z]->instanceIndex_ = blockData_[blockID]->currentSum;
			//	blockData_[blockID]->currentSum++;
			//}
			//for (int y = height; y < CHUNK_Z; y++)
			//{
			//	blockID = BlockID::Air;
			//	block_[x][y][z]->SetBlockType(blockConfig_->GetBlockInfo(blockID));
			//	blockPosition = PositionByIndex(Vector3int(x, y, z));
			//	block_[x][y][z]->SetBlockPosition(blockPosition);
			//	block_[x][y][z]->instanceIndex_ = 0;
			//}

			for (int y = 0; y < CHUNK_Z; y++)
			{
				BlockID blockID;

				if (y < height - dirtThickness)
				{
					blockID = BlockID::Stone;
				}
				else if (y < height - 1)
				{
					blockID = BlockID::Dirt;
				}
				else if (y < height)
				{
					blockID = BlockID::Lawn;
				}
				else
				{
					blockID = BlockID::Air;
				}

				block_[x][y][z]->SetBlockType(blockConfig_->GetBlockInfo(blockID));
				block_[x][y][z]->SetBlockPosition(PositionByIndex(Vector3int(x, y, z)));
				blockData_[blockID]->currentSum++;
			}
		}
	}

	SetExposedBlocks();
}

void MapManager::CreateNewMap()
{

}

void MapManager::SetExposedBlocks()
{
	for (int x = 0; x < CHUNK_X; x++)
	{
		for (int y = 0; y < CHUNK_Z; y++)
		{
			for (int z = 0; z < CHUNK_Y; z++)
			{
				// ブロックのIDを取得
				BlockID id = block_[x][y][z]->GetBlockID();


				// Air は露出していても描画しない
				if (id == BlockID::Air)
				{
					block_[x][y][z]->isExposed_ = false;
					continue;
				}

				bool exposed = false;

				// 6方向のオフセット
				int dx[6] = { -1, 1, 0, 0, 0, 0 };
				int dy[6] = { 0, 0, -1, 1, 0, 0 };
				int dz[6] = { 0, 0, 0, 0, -1, 1 };

				for (int i = 0; i < 6; i++)
				{
					int nx = x + dx[i];
					int ny = y + dy[i];
					int nz = z + dz[i];

					if (nx < 0 || nx >= CHUNK_X) continue;
					if (ny < 0 || ny >= CHUNK_Y) continue;
					if (nz < 0 || nz >= CHUNK_Z) continue;

					if (block_[nx][ny][nz]->GetBlockID() == BlockID::Air)
					{
						exposed = true;
						break; // 早期終了
					}
				}

				block_[x][y][z]->isExposed_ = exposed;

				if (block_[x][y][z]->isExposed_)
				{
					// ブロックの座標を順番に設定
					block_[x][y][z]->instanceIndex_ = 
					blockData_[id]->AddNewBlock(
						block_[x][y][z]->position_, 
						Vector3int(x, y, z)
					);
				}
			}
		}
	}
}

void MapManager::Initialize()
{

}

void MapManager::Update()
{
	//UpDataPlayerRayCollision();

	if (Game::Input::Key::IsJustPressed(DIK_0))
	{
		LoadMap("Resources/Map/map.csv");
	}

	
	for (int x = 0; x < CHUNK_X; x++)
	{
		for (int y = 0; y < CHUNK_Z; y++)
		{
			for (int z = 0; z < CHUNK_Y; z++)
			{
				block_[x][y][z]->Update();

				if (block_[x][y][z]->isExposed_ && block_[x][y][z]->GetBlockID() != BlockID::Air)
				{
					blockData_[block_[x][y][z]->GetBlockID()]->colorData_[block_[x][y][z]->instanceIndex_]
						= block_[x][y][z]->color_;
				}
			}
		}
	}

	

	/// 
	//UpdatePlayerCollisionY();
	//UpdatePlayerCollisionXZ();
}

//void MapManager::UpDataPlayerRayCollision()
//{
//	// プレイヤー視点のインデックス
//	Vector3int lineStartIdx = IndexByPosition(player_->viewLine_.origin);
//	Vector3int lineEndIdx = IndexByPosition(player_->viewLine_.end);
//	Vector3int currentIdx = lineStartIdx;
//
//	// 始点インデックスのブロックから順番に衝突判定していき、衝突したらそこで終了する
//	while (currentIdx != lineEndIdx)
//	{
//		// もし調査インデックスのブロックが表面に出ている(存在している && 見えるところにある)ブロックなら
//		if (block_[currentIdx.x][currentIdx.y][currentIdx.z]->isExposed_)
//		{
//			block_[currentIdx.x][currentIdx.y][currentIdx.z]->isCollisionRay = true;
//		}
//
//		// インデックスを１ステップ進める
//		Vector3 direction = (player_->viewLine_.end - player_->viewLine_.origin).Normalized();
//		Vector3 nextPosition = PositionByIndex(currentIdx) + direction * BLOCK_SIZE;
//		currentIdx = IndexByPosition(nextPosition);
//	}
//}

void MapManager::UpDataPlayerRayCollision()
{
	// プレイヤー視点のインデックス
	Vector3 rayStart = player_->viewLine_.origin;
	Vector3 rayEnd = player_->viewLine_.end;
	Vector3 dir = (rayEnd - rayStart).Normalized();

	// 現在のインデックス
	Vector3int currentIdx = IndexByPosition(rayStart);
	// 最終目的地のインデックス
	Vector3int endIdx = IndexByPosition(rayEnd);

	// 各軸に進む方向 (1 または -1)
	int stepX = (dir.x > 0) ? 1 : -1;
	int stepY = (dir.y > 0) ? 1 : -1;
	int stepZ = (dir.z > 0) ? 1 : -1;

	// 次の境界線までの距離を算出するための準備
	// deltaDist: その軸方向にブロック1つ分進むのに必要なレイの長さ
	float deltaDistX = std::abs(BLOCK_SIZE / dir.x);
	float deltaDistY = std::abs(BLOCK_SIZE / dir.y);
	float deltaDistZ = std::abs(BLOCK_SIZE / dir.z);

	// sideDist: 現在地から「次の境界線」までのレイの長さ
	Vector3 blockPos = PositionByIndex(currentIdx); // ブロックの中心（または最小角）
	float sideDistX = (stepX > 0) ? (blockPos.x + BLOCK_SIZE - rayStart.x) : (rayStart.x - blockPos.x);
	float sideDistY = (stepY > 0) ? (blockPos.y + BLOCK_SIZE - rayStart.y) : (rayStart.y - blockPos.y);
	float sideDistZ = (stepZ > 0) ? (blockPos.z + BLOCK_SIZE - rayStart.z) : (rayStart.z - blockPos.z);

	sideDistX = (dir.x != 0) ? sideDistX / std::abs(dir.x) : FLT_MAX;
	sideDistY = (dir.y != 0) ? sideDistY / std::abs(dir.y) : FLT_MAX;
	sideDistZ = (dir.z != 0) ? sideDistZ / std::abs(dir.z) : FLT_MAX;

	// 最大ループ回数（安全のため。レイの長さに応じて調整）
	int maxSteps = 100;

	for (int i = 0; i < maxSteps; i++)
	{
		// 衝突判定
		if (block_[currentIdx.x][currentIdx.y][currentIdx.z]->GetBlockID() != BlockID::Air)
		{
			block_[currentIdx.x][currentIdx.y][currentIdx.z]->isCollisionRay = true;
			return; // 衝突したら終了
		}

		// ゴールに到達したら終了
		if (currentIdx == endIdx) break;

		// 最も近い境界線（X, Y, Zのどれか）を跨ぐ
		if (sideDistX < sideDistY)
		{
			if (sideDistX < sideDistZ)
			{
				sideDistX += deltaDistX;
				currentIdx.x += stepX;
				//block_[currentIdx.x][currentIdx.y][currentIdx.z]->direction =
			}
			else
			{
				sideDistZ += deltaDistZ;
				currentIdx.z += stepZ;
			}
		}
		else
		{
			if (sideDistY < sideDistZ)
			{
				sideDistY += deltaDistY;
				currentIdx.y += stepY;
			}
			else
			{
				sideDistZ += deltaDistZ;
				currentIdx.z += stepZ;
			}
		}
	}
}

//void MapManager::UpdatePlayerCollisionY()
//{
//#pragma region 下方向
//
//	float playerHeight = player_->data_.aabbs[0].max.y - player_->data_.aabbs[0].min.y;
//
//	Vector3 corners[4] = {
//		Vector3(player_->data_.aabbs[0].min.x, player_->data_.aabbs[0].min.y - 0.1f, player_->data_.aabbs[0].min.z),
//		Vector3(player_->data_.aabbs[0].max.x, player_->data_.aabbs[0].min.y - 0.1f, player_->data_.aabbs[0].min.z),
//		Vector3(player_->data_.aabbs[0].min.x, player_->data_.aabbs[0].min.y - 0.1f, player_->data_.aabbs[0].max.z),
//		Vector3(player_->data_.aabbs[0].max.x, player_->data_.aabbs[0].min.y - 0.1f, player_->data_.aabbs[0].max.z)
//	};
//
//	bool anyCollision = false;
//	float bestGroundY = -999.9f;
//
//	for (int i = 0; i < 4; ++i)
//	{
//		Vector3int idx = IndexByPosition(corners[i]);
//
//		if (block_[idx.x][idx.y][idx.z]->aabb_.max.y >= corners[i].y && !block_[idx.x][idx.y][idx.z]->durability_->GetIsDestroy())
//		{
//			anyCollision = true;
//			float groundY = block_[idx.x][idx.y][idx.z]->aabb_.max.y;
//
//			if (groundY > bestGroundY) bestGroundY = groundY;
//		}
//	}
//
//	if (anyCollision)
//	{
//		// translate.value.y を地面上に設定する（translate が中心なら center.y = groundY + height/2）
//		player_->data_.translate.value.y = bestGroundY + playerHeight * 0.5f;
//
//		// AABB を center から再計算するか、min/max を更新する
//		player_->data_.aabbs[0].min.y = player_->data_.translate.value.y - playerHeight * 0.5f;
//		player_->data_.aabbs[0].max.y = player_->data_.translate.value.y + playerHeight * 0.5f;
//
//		player_->data_.translate.velocity.y = 0.0f;
//		player_->data_.translate.acceleration.y = 0.0f;
//	}
//	else
//	{
//		player_->data_.translate.acceleration.y = GRAVITY;
//	}
//
//#pragma endregion
//}
//
//void MapManager::UpdatePlayerCollisionZ()
//{
//#pragma region 前方向
//
//	float playerWidth = player_->data_.aabbs[0].max.z - player_->data_.aabbs[0].min.z;
//
//	Vector3 corners[4] = {
//		Vector3(player_->data_.aabbs[0].min.x, player_->data_.aabbs[0].min.y + 0.1f, player_->data_.aabbs[0].min.z + 0.1f),
//		Vector3(player_->data_.aabbs[0].max.x, player_->data_.aabbs[0].min.y + 0.1f, player_->data_.aabbs[0].min.z + 0.1f),
//		Vector3(player_->data_.aabbs[0].min.x, player_->data_.aabbs[0].max.y - 0.1f, player_->data_.aabbs[0].min.z + 0.1f),
//		Vector3(player_->data_.aabbs[0].max.x, player_->data_.aabbs[0].max.y - 0.1f, player_->data_.aabbs[0].min.z + 0.1f)
//	};
//
//	bool anyCollision = false;
//	float bestFrontZ = 9999.9f;
//
//	for (int i = 0; i < 4; ++i)
//	{
//		Vector3int idx = IndexByPosition(corners[i]);
//		if (block_[idx.x][idx.y][idx.z]->aabb_.min.z <= corners[i].z && !block_[idx.x][idx.y][idx.z]->durability_->GetIsDestroy())
//		{
//			anyCollision = true;
//			float frontZ = block_[idx.x][idx.y][idx.z]->aabb_.min.z;
//			if (frontZ < bestFrontZ) bestFrontZ = frontZ;
//		}
//	}
//
//	if (anyCollision)
//	{
//		// translate.value.z を地面上に設定する（translate が中心なら center.z = frontZ - width/2）
//		player_->data_.translate.value.z = bestFrontZ - playerWidth * 0.5f;
//		// AABB を center から再計算するか、min/max を更新する
//		player_->data_.aabbs[0].min.z = player_->data_.translate.value.z - playerWidth * 0.5f;
//		player_->data_.aabbs[0].max.z = player_->data_.translate.value.z + playerWidth * 0.5f;
//		if (player_->data_.translate.velocity.z > 0)
//		{
//			player_->data_.translate.velocity.z = 0.0f;
//		}
//	}
//
//#pragma endregion
//}
//
//void MapManager::UpdatePlayerCollisionX()
//{
//#pragma region 右方向
//
//	float playerWidth = player_->data_.aabbs[0].max.x - player_->data_.aabbs[0].min.x;
//
//	Vector3 corners[4] = {
//		Vector3(player_->data_.aabbs[0].max.x + 0.1f, player_->data_.aabbs[0].min.y + 0.1f, player_->data_.aabbs[0].min.z),
//		Vector3(player_->data_.aabbs[0].max.x + 0.1f, player_->data_.aabbs[0].min.y + 0.1f, player_->data_.aabbs[0].max.z),
//		Vector3(player_->data_.aabbs[0].max.x + 0.1f, player_->data_.aabbs[0].max.y - 0.1f, player_->data_.aabbs[0].min.z),
//		Vector3(player_->data_.aabbs[0].max.x + 0.1f, player_->data_.aabbs[0].max.y - 0.1f, player_->data_.aabbs[0].max.z)
//	};
//
//	bool anyCollision = false;
//	float bestFrontX = 9999.9f;
//
//	for (int i = 0; i < 4; ++i)
//	{
//		Vector3int idx = IndexByPosition(corners[i]);
//		if (block_[idx.x][idx.y][idx.z]->aabb_.min.z <= corners[i].z && !block_[idx.x][idx.y][idx.z]->durability_->GetIsDestroy())
//		{
//			anyCollision = true;
//			float frontX = block_[idx.x][idx.y][idx.z]->aabb_.min.z;
//			if (frontX < bestFrontX) bestFrontX = frontX;
//		}
//	}
//
//	if (anyCollision)
//	{
//		// translate.value.z を地面上に設定する（translate が中心なら center.z = frontZ - width/2）
//		player_->data_.translate.value.x = bestFrontX - playerWidth * 0.5f;
//		// AABB を center から再計算するか、min/max を更新する
//		player_->data_.aabbs[0].min.x = player_->data_.translate.value.x - playerWidth * 0.5f;
//		player_->data_.aabbs[0].max.x = player_->data_.translate.value.x + playerWidth * 0.5f;
//		if (player_->data_.translate.velocity.x > 0)
//		{
//			player_->data_.translate.velocity.x = 0.0f;
//		}
//	}
//
//#pragma endregion
//
//}

void MapManager::Draw()
{
	// 0 はAirなので描画しない
	for (int32_t i = 1; i < int32_t(BlockID::MAX); ++i)
	{
		blockData_[BlockID(i)]->Draw();
	}

	for (auto& item : dropItems_)
	{
		item->Draw();
	}
}

void MapManager::DrawImGui()
{
	for (int32_t i = 0; i < int32_t(BlockID::MAX); ++i)
	{
		blockData_[BlockID(i)]->DrawImGui();
	}
}

AABB MapManager::GetAABB(const Vector3int& index)
{
	Vector3int clampedIndex = index;
	clampedIndex.x = std::clamp(index.x, 0, CHUNK_X - 1);
	clampedIndex.y = std::clamp(index.y, 0, CHUNK_Z - 1);
	clampedIndex.z = std::clamp(index.z, 0, CHUNK_Y - 1);

	return block_[clampedIndex.x][clampedIndex.y][clampedIndex.z]->aabb_;
}

bool MapManager::GetIsActive(const Vector3int& index)
{
	Vector3int clampedIndex = index;
	clampedIndex.x = std::clamp(index.x, 0, CHUNK_X - 1);
	clampedIndex.y = std::clamp(index.y, 0, CHUNK_Z - 1);
	clampedIndex.z = std::clamp(index.z, 0, CHUNK_Y - 1);

	return block_[clampedIndex.x][clampedIndex.y][clampedIndex.z]->isActive_;
}

Vector3int MapManager::IndexByPosition(const Vector3& position)
{
	Vector3int index;
	index.x = static_cast<int>(((position.x + (CHUNK_X - 1)) / BLOCK_SIZE) + (BLOCK_SIZE / 2.0f));
	index.y = static_cast<int>(((position.y + (CHUNK_Z - 1)) / BLOCK_SIZE));
	index.z = static_cast<int>(((position.z + (CHUNK_Y - 1)) / BLOCK_SIZE) + (BLOCK_SIZE / 2.0f));

	if (index.x < 0)index.x = 0;
	else if (index.x > CHUNK_X - 1)index.x = CHUNK_X - 1;
	if (index.y < 0)index.y = 0;
	else if (index.y > CHUNK_Z - 1)index.y = CHUNK_Z - 1;
	if (index.z < 0)index.z = 0;
	else if (index.z > CHUNK_Y - 1)index.z = CHUNK_Y - 1;

	return index;
}

Vector3 MapManager::PositionByIndex(const Vector3int& index)
{
	Vector3 position;
	position.x = (index.x * BLOCK_SIZE) - (CHUNK_X - 1);
	position.y = (index.y * BLOCK_SIZE) - (CHUNK_Z - 1);
	position.z = (index.z * BLOCK_SIZE) - (CHUNK_Y - 1);
	return position;
}

AABB MapManager::AABBByIndex(const Vector3int& index)
{
	AABB aabb;
	Vector3 center = PositionByIndex(index);
	aabb.min = Vector3(
		center.x - (BLOCK_SIZE / 2.0f),
		center.y - (BLOCK_SIZE / 2.0f),
		center.z - (BLOCK_SIZE / 2.0f)
	);
	aabb.max = Vector3(
		center.x + (BLOCK_SIZE / 2.0f),
		center.y + (BLOCK_SIZE / 2.0f),
		center.z + (BLOCK_SIZE / 2.0f)
	);
	return aabb;
}

std::optional<Vector3> MapManager::IntersectRayBlock(const Ray& ray, const std::vector<VertexData>& vertices, const AABB& aabb)
{
	// まずAABBで大まかに判定
	if (!IsCollision(ray, aabb))
	{
		return std::nullopt;
	}


	// AABBに当たっていた場合のみ、三角形ごとに詳細判定 最近衝突点を返す
	std::optional<Vector3> closestPoint = std::nullopt;
	float closestDist = std::numeric_limits<float>::infinity();
	for (size_t i = 0; i + 2 < vertices.size(); i += 3)
	{
		Triangle t;
		// 三角形の頂点をワールド座標に変換
		t.vertices[0] = Vector3(
			vertices[i].position.x,
			vertices[i].position.y,
			vertices[i].position.z
		);
		t.vertices[1] = Vector3(
			vertices[i + 1].position.x,
			vertices[i + 1].position.y,
			vertices[i + 1].position.z
		);
		t.vertices[2] = Vector3(
			vertices[i + 2].position.x,
			vertices[i + 2].position.y,
			vertices[i + 2].position.z
		);

		std::optional<Vector3> pos = IntersectRayTriangle(ray, t);
		if (pos != std::nullopt)
		{
			// 衝突点までの距離を計算
			float dist = (pos.value() - ray.origin).Length();
			if (dist < closestDist)
			{
				closestDist = dist;
				closestPoint = pos;
			}
		}
	}
	return closestPoint;
}
