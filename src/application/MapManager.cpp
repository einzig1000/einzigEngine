#include "MapManager.h"
#include <fstream>
#include <sstream>
#include "Block.h"
#include "Player.h"
#include "PerlinNoise.h"
#include "ResourceID.h"
#include "Engine.h"
#include "Itemslot.h"

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
				block_[x][y][z]->model_.isCheckMouseRay = true;
				//player->data_.SetBlock(block_[x][y][z]->model_);
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
				if (block_[x][y][z]->model_.inPicture)
				{
					// 最近接衝突点を取得
					std::optional<Vector3> colPos = IntersectRayModel(
						player_->viewRay_,
						Engine::Instance().GetAllObject3D()[block_[x][y][z]->model_.model].modelData.vertices,
						&block_[x][y][z]->model_
					);
					// 衝突していたらリストに登録
					if (colPos)
					{
						float minDistance = (colPos.value() - player_->viewRay_.origin).Length();
						DirectionXYZ dir = DirectionXYZ::None;
						if (colPos->x >= block_[x][y][z]->model_.translate.value.x + (BLOCK_SIZE / 2.0f) - 0.01f)
						{
							dir = DirectionXYZ::Right;
						}
						else if (colPos->x <= block_[x][y][z]->model_.translate.value.x - (BLOCK_SIZE / 2.0f) + 0.01f)
						{
							dir = DirectionXYZ::Left;
						}
						else if (colPos->y >= block_[x][y][z]->model_.translate.value.y + (BLOCK_SIZE / 2.0f) - 0.01f)
						{
							dir = DirectionXYZ::Up;
						}
						else if (colPos->y <= block_[x][y][z]->model_.translate.value.y - (BLOCK_SIZE / 2.0f) + 0.01f)
						{
							dir = DirectionXYZ::Down;
						}
						else if (colPos->z >= block_[x][y][z]->model_.translate.value.z + (BLOCK_SIZE / 2.0f) - 0.01f)
						{
							dir = DirectionXYZ::Front;
						}
						else if (colPos->z <= block_[x][y][z]->model_.translate.value.z - (BLOCK_SIZE / 2.0f) + 0.01f)
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
		hits[order].block->direction = hits[order].direction;
	}

	// マウス右ボタンが押されているフラグ
	bool isMouseRightHeld = Game::Input::Mouse::IsJustPressed(1);

	isBeingDestroyed_ = false;
	for (int x = 0; x < MAX_BLOCK_X; x++)
	{
		for (int y = 0; y < MAX_BLOCK_Y; y++)
		{
			for (int z = 0; z < MAX_BLOCK_Z; z++)
			{
				block_[x][y][z]->Update();
				// 破壊中
				if (block_[x][y][z]->isBeingDestroyed_ && block_[x][y][z]->isCollisionRay == 0)
				{
					// 破壊中ブロックの周りに破壊テクスチャを配置
					blockTriangleTransform_ = Transforms(
						Vector3(1.0f, 1.0f, 1.0f),
						block_[x][y][z]->model_.rotate.value,
						block_[x][y][z]->model_.translate.value);

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

					Vector3 ItemPos = block_[x][y][z]->model_.translate.value;
					ItemPos.x += Game::Math::RandFloat(-0.4f, 0.4f, 1);
					ItemPos.z += Game::Math::RandFloat(-0.4f, 0.4f, 1);
					ItemPos.y -= (BLOCK_SIZE / 2.0f) - 0.4f;
					DropItem* dropItem = new DropItem(player_, Vector3int(x,y,z), ItemPos, block_[x][y][z]->model_.model, block_[x][y][z]->model_.texture);
					dropItems_.push_back(dropItem);
				}
				// 破壊されていない
				// マウス右ボタンが押されている
				if (isMouseRightHeld && block_[x][y][z]->isCollisionRay == 0 && block_[x][y][z]->isDestroy_)
				{
					Vector3int targetIndex = Vector3int(x, y, z);
					switch (block_[x][y][z]->direction)
					{
					case DirectionXYZ::None:
						break;
					case DirectionXYZ::Left:
						targetIndex.x -= 1;
						break;
					case DirectionXYZ::Right:
						targetIndex.x += 1;
						break;
					case DirectionXYZ::Back:
						targetIndex.z -= 1;
						break;
					case DirectionXYZ::Front:
						targetIndex.z += 1;
						break;
					case DirectionXYZ::Down:
						targetIndex.y -= 1;
						break;
					case DirectionXYZ::Up:
						targetIndex.y += 1;
						break;
					default:
						break;
					}
					
						 
					block_[targetIndex.x][targetIndex.y][targetIndex.z]->model_.scale.value = Vector3(1.0f, 1.0f, 1.0f);
					block_[targetIndex.x][targetIndex.y][targetIndex.z]->model_.texture = player_->Itemslot_->getSelectedItemID();
					block_[targetIndex.x][targetIndex.y][targetIndex.z]->isDestroy_ = false;
					block_[targetIndex.x][targetIndex.y][targetIndex.z]->nowDurability_ = 0;
					block_[targetIndex.x][targetIndex.y][targetIndex.z]->maxDurability_ = 60;
					block_[targetIndex.x][targetIndex.y][targetIndex.z]->destroyFrame_ = 0;
					block_[targetIndex.x][targetIndex.y][targetIndex.z]->isBeingDestroyed_ = false;
					block_[targetIndex.x][targetIndex.y][targetIndex.z]->isJustDestroyed_ = false;
					block_[targetIndex.x][targetIndex.y][targetIndex.z]->isExposed_ = true;
					player_->Itemslot_->useSelectedItem();
				}
			}
		}
	}

	for (int i = 0; i < 6; i++)
	{
		blockRect_[i].transforms = blockTriangleTransform_;
	}

	for (auto& item : dropItems_)
	{
		item->Update(block_[item->index.x][item->index.y][item->index.z]->isDestroy_);
	}

	UpdatePlayerCollisionY();
	//UpdatePlayerCollisionXZ();
}

void MapManager::UpdatePlayerCollisionY()
{
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

		if (block_[idx.x][idx.y][idx.z]->model_.aabbs[0].max.y >= corners[i].y && !block_[idx.x][idx.y][idx.z]->isDestroy_)
		{
			//block_[idx.x][idx.y][idx.z]->model_.color = Vector4(0x00, 0xFF, 0x00, 0xFF);
			anyCollision = true;
			float groundY = block_[idx.x][idx.y][idx.z]->model_.aabbs[0].max.y;

			// 同一ブロックに複数点で当たっても最高の地面 Y を採用
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
}

void MapManager::UpdatePlayerCollisionXZ()
{
	Vector3 currentPosition = player_->data_.translate.value;
	AABB currentAABB = player_->data_.aabbs[0];
	Vector3 velocityXZ = Vector3(player_->data_.translate.velocity.x, 0.0f, player_->data_.translate.velocity.z);
	Vector3 movedPosition = currentPosition + velocityXZ;
	AABB movedAABB = AABB{ currentAABB.min + velocityXZ, currentAABB.max + velocityXZ };
	movedAABB.min.y -= -0.3f;

	Vector3 corners[8] = {
		Vector3(movedAABB.min.x, movedAABB.min.y, movedAABB.min.z),
		Vector3(movedAABB.max.x, movedAABB.min.y, movedAABB.min.z),
		Vector3(movedAABB.min.x, movedAABB.min.y, movedAABB.max.z),
		Vector3(movedAABB.max.x, movedAABB.min.y, movedAABB.max.z),
		Vector3(movedAABB.min.x, movedAABB.max.y, movedAABB.min.z),
		Vector3(movedAABB.max.x, movedAABB.max.y, movedAABB.min.z),
		Vector3(movedAABB.min.x, movedAABB.max.y, movedAABB.max.z),
		Vector3(movedAABB.max.x, movedAABB.max.y, movedAABB.max.z),
	};

	bool anyCollision = false;
	Vector2 bestOffset = { 0.0f,0.0f };

	for (int i = 0; i < 8; ++i)
	{
		Vector3int idx = IndexByPosition(corners[i]);

		if (!block_[idx.x][idx.y][idx.z]->isDestroy_)
		{
			anyCollision = true;
			block_[idx.x][idx.y][idx.z]->model_.color = Vector4(0x00, 0xFF, 0x00, 0xFF);

			// 衝突している場合、プレイヤーをブロックの外に押し出す
			AABB blockAABB = block_[idx.x][idx.y][idx.z]->model_.aabbs[0];
			Vector2 offset = { 0.0f, 0.0f };
			// X方向の押し出し量を計算
			if (movedAABB.max.x > blockAABB.min.x && currentAABB.max.x <= blockAABB.min.x)
			{
				offset.x = blockAABB.min.x - movedAABB.max.x - eps;
			}
			else if (movedAABB.min.x < blockAABB.max.x && currentAABB.min.x >= blockAABB.max.x)
			{
				offset.x = blockAABB.max.x - movedAABB.min.x + eps;
			}
			// Z方向の押し出し量を計算
			if (movedAABB.max.z > blockAABB.min.z && currentAABB.max.z <= blockAABB.min.z)
			{
				offset.y = blockAABB.min.z - movedAABB.max.z - eps;
			}
			else if (movedAABB.min.z < blockAABB.max.z && currentAABB.min.z >= blockAABB.max.z)
			{
				offset.y = blockAABB.max.z - movedAABB.min.z + eps;
			}
			// 最も大きな押し出し量を採用
			if (std::abs(offset.x) > std::abs(bestOffset.x))
			{
				bestOffset.x = offset.x;
			}
			if (std::abs(offset.y) > std::abs(bestOffset.y))
			{
				bestOffset.y = offset.y;
			}
		}
	}

	if (anyCollision)
	{
		// プレイヤー位置を押し出し分だけ修正
		player_->data_.translate.value.x += bestOffset.x;
		player_->data_.translate.value.z += bestOffset.y;
		// AABB も更新
		player_->data_.aabbs[0].min.x += bestOffset.x;
		player_->data_.aabbs[0].max.x += bestOffset.x;
		player_->data_.aabbs[0].min.z += bestOffset.y;
		player_->data_.aabbs[0].max.z += bestOffset.y;
		// XZ速度を0にする
		player_->data_.translate.velocity.x = 0.0f;
		player_->data_.translate.velocity.z = 0.0f;
		player_->data_.translate.acceleration.x = 0.0f;
		player_->data_.translate.acceleration.z = 0.0f;
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

	for (auto& item : dropItems_)
	{
		item->Draw();
	}

	if (isBeingDestroyed_)
	{
		for (int i = 0; i < 6; i++)
		{
			blockRect_[i].Draw();
		}
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
