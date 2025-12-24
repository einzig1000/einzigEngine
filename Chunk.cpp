#include "Chunk.h"
#include "Block/Block.h"
#include "Utilities/JsonManager.h"
#include "Block/BlockConfig.h"
#include "Block/BlockDurability.h"

Chunk::Chunk()
{
	blockConfig_ = new BlockConfig();

	// ブロックデータの初期化
	for (int32_t i = 0; i < int32_t(BlockID::MAX); ++i)
	{
		blockData_[BlockID(i)] = std::make_unique<RenderData_Block>(BlockID(i));
		blockData_[BlockID(i)]->texture = ResourceID::GetTextureID(BlockID(i));
		blockData_[BlockID(i)]->breakTexture = ResourceID::GetTextureID(TextureID::BreakBlock_Array);
		blockData_[BlockID(i)]->model = ResourceID::GetModelID(ModelID::Cube);

		blockData_[BlockID(i)]->currentDrawSum = 0;
		blockData_[BlockID(i)]->currentSum = 0;
	}
}

Chunk::~Chunk()
{
	delete blockConfig_;
	blockConfig_ = nullptr;
}

void Chunk::CreateChunkData(const NoiseParameter& param, const Vector2int & chunkPos)
{
	this->chunkPos = chunkPos;
	CreateInstance();

	// 既にセーブデータが存在している場合
	if (loadResult)
	{
		// blockPositions に基づいてブロックを生成
		for (const auto& [blockID, positions] : blockPositions)
		{
			for (const auto& pos : positions)
			{
				// ブロックのAABB取得
				AABB aabb = GetAABB(pos);
				// ブロックの中心座標取得
				Vector3 center = aabb.center();

				blocks[pos.x][pos.y][pos.z]->SetBlockType(blockConfig_->GetBlockInfo(blockID));
				blocks[pos.x][pos.y][pos.z]->SetBlockPosition(center);
			}
		}
	}
	// 新規生成の場合
	else
	{
		// チャンク内すべてのブロック生成
		for (int x = 0; x < CHUNK_X; ++x)
		{
			for (int z = 0; z < CHUNK_Z; ++z)
			{
				// ワールド座標でのブロックインデックス
				const int worldX = chunkPos.x * CHUNK_X + x;
				const int worldZ = chunkPos.y * CHUNK_Z + z;

				// ワールド座標をノイズサンプル空間へスケールダウン（連続性が鍵）
				const float sampleX = static_cast<float>(worldX) / param.scale;
				const float sampleZ = static_cast<float>(worldZ) / param.scale;

				// フラクタルノイズ（0..1）
				float n = fractalPerlin(param.pn, sampleX, sampleZ, param.octaves, param.persistence);

				// 高さへ変換（0..maxHeight-1）
				int height = static_cast<int>(std::floor(n * float(param.height - 1) + 0.5f));
				if (height < 0) height = 0;
				if (height > param.height - 1) height = param.height - 1;

				// 素材の割り当て（例）
				int dirtThickness = 3;
				if (height - dirtThickness < 0) dirtThickness = height;

				for (int y = 0; y < CHUNK_Y; ++y)
				{
					// ブロックID決定
					BlockID id;
					if (y < height - dirtThickness) id = BlockID::Stone;
					else if (y < height - 1)       id = BlockID::Dirt;
					else if (y < height)           id = BlockID::Lawn;
					else                            id = BlockID::Air;
					// ブロックのAABB取得
					AABB aabb = GetAABB(Vector3int(x, y, z));
					// ブロックの中心座標取得
					Vector3 center = aabb.center();

					blocks[x][y][z]->SetBlockType(blockConfig_->GetBlockInfo(id));
					blocks[x][y][z]->SetBlockPosition(center);
					blockPositions[id].emplace_back(x, y, z);
				}
			}
		}
	}

	SetExposedBlocks();
}

void Chunk::SetExposedBlocks()
{
	for (int x = 0; x < CHUNK_X; x++)
	{
		for (int y = 0; y < CHUNK_Y; y++)
		{
			for (int z = 0; z < CHUNK_Z; z++)
			{
				// ブロックのIDを取得
				BlockID id = blocks[x][y][z]->GetBlockID();


				// Air は露出していても描画しない
				if (id == BlockID::Air)
				{
					blocks[x][y][z]->isExposed_ = false;
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

					if (blocks[nx][ny][nz]->GetBlockID() == BlockID::Air)
					{
						exposed = true;
						break;
					}
				}

				blocks[x][y][z]->isExposed_ = exposed;

				if (blocks[x][y][z]->isExposed_)
				{
					// ブロックの座標を順番に設定
					blocks[x][y][z]->instanceIndex_ =
						blockData_[id]->AddNewBlock(
							blocks[x][y][z]->position_,
							Vector3int(x, y, z)
						);
				}
			}
		}
	}
}

void Chunk::Update()
{
	for (int x = 0; x < CHUNK_X; x++)
	{
		for (int z = 0; z < CHUNK_Z; z++)
		{
			for (int y = 0; y < CHUNK_Y; y++)
			{
				if (blocks[x][y][z] != nullptr)
				{
					blocks[x][y][z]->Update();

					if (blocks[x][y][z]->isExposed_ && blocks[x][y][z]->GetBlockID() != BlockID::Air)
					{
						blockData_[blocks[x][y][z]->GetBlockID()]->colorData_[blocks[x][y][z]->instanceIndex_]
							= blocks[x][y][z]->color_;

						blockData_[blocks[x][y][z]->GetBlockID()]->breakLayerData_[blocks[x][y][z]->instanceIndex_]
							= blocks[x][y][z]->durability_->GetBreakStage();
					}
				}
			}
		}
	}
}

void Chunk::Draw()
{
	// 0 はAirなので描画しない
	for (int32_t i = 1; i < int32_t(BlockID::MAX); ++i)
	{
		blockData_[BlockID(i)]->Draw();
	}
}

AABB Chunk::GetAABB(const Vector3int& index)
{
	// チャンクのワールド原点
	float chunkWorldX = chunkPos.x * CHUNK_X * BLOCK_SIZE;
	float chunkWorldZ = chunkPos.y * CHUNK_Z * BLOCK_SIZE;

	// ブロックのワールド座標
	float worldX = chunkWorldX + index.x * BLOCK_SIZE;
	float worldY = index.y * BLOCK_SIZE;
	float worldZ = chunkWorldZ + index.z * BLOCK_SIZE;

	Vector3 mint(worldX, worldY, worldZ);
	Vector3 maxt(worldX + BLOCK_SIZE, worldY + BLOCK_SIZE, worldZ + BLOCK_SIZE);

	return AABB(mint, maxt);
}

// ブロックのインスタンス生成
void Chunk::CreateInstance()
{
	for (int x = 0; x < CHUNK_X; x++)
	{
		for (int z = 0; z < CHUNK_Z; z++)
		{
			for (int y = 0; y < CHUNK_Y; y++)
			{
				blocks[x][y][z] = std::make_unique<Block>();
				blocks[x][y][z]->Initialize();
			}
		}
	}
}
