#include "Chunk.h"
#include "Block/Block.h"
#include "Utilities/JsonManager.h"

Chunk::Chunk()
{
	for (int x = 0; x < CHUNK_X; x++)
	{
		for (int z = 0; z < CHUNK_Z; z++)
		{
			for (int y = 0; y < CHUNK_Y; y++)
			{
				blocks[x][z][y] = std::make_unique<Block>();
				blocks[x][z][y]->Initialize();
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
}

void Chunk::SetExposedBlocks()
{
	for (int x = 0; x < CHUNK_X; x++)
	{
		for (int y = 0; y < CHUNK_Z; y++)
		{
			for (int z = 0; z < CHUNK_Y; z++)
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
				if (blocks[x][z][y] != nullptr)
				{
					blocks[x][z][y]->Update();

					if (blocks[x][y][z]->isExposed_ && blocks[x][y][z]->GetBlockID() != BlockID::Air)
					{
						blockData_[blocks[x][y][z]->GetBlockID()]->colorData_[blocks[x][y][z]->instanceIndex_]
							= blocks[x][y][z]->color_;
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

void Chunk::SaveChunkData(const std::string& mapFilePath)
{
	// チャンク座標とマップデータを書き込む

	JsonManager json;
	std::string chunkKey = "Chunk_" + std::to_string(chunkPos.x) + "_" + std::to_string(chunkPos.y);
	for (int x = 0; x < CHUNK_X; x++)
	{
		for (int z = 0; z < CHUNK_Z; z++)
		{
			for (int y = 0; y < CHUNK_Y; y++)
			{
				std::string blockKey = "Block_" + std::to_string(x) + "_" + std::to_string(y) + "_" + std::to_string(z);
				json.SaveToJson(mapFilePath, chunkKey + "_" + blockKey, static_cast<int>(blocks[x][z][y]->GetBlockID()));
			}
		}
	}
}
