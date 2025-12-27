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
		// 事前に定数を計算
		const float invScale = 1.0f / param.scale;
		const int maxY = param.height - 1;

		// ブロック中心座標
		const float half = BLOCK_SIZE * 0.5f;
		const float baseX = chunkPos.x * CHUNK_X * BLOCK_SIZE + half;
		const float baseZ = chunkPos.y * CHUNK_Z * BLOCK_SIZE + half;

		// チャンク内すべてのブロック生成
		for (int x = 0; x < CHUNK_X; ++x)
		{
			for (int z = 0; z < CHUNK_Z; ++z)
			{
				// ワールド座標でのブロックインデックス
				const int worldX = chunkPos.x * CHUNK_X + x;
				const int worldZ = chunkPos.y * CHUNK_Z + z;

				// ワールド座標をノイズサンプル空間へスケールダウン
				const float sampleX = static_cast<float>(worldX) * invScale;
				const float sampleZ = static_cast<float>(worldZ) * invScale;

				// フラクタルノイズ（0..1）
				const float n = fractalPerlin(param.pn, sampleX, sampleZ, param.octaves, param.persistence);

				// 高さへ変換（0..maxY）
				int height = static_cast<int>(std::floor(n * float(maxY) + 0.5f));
				if (height < 0) height = 0;
				if (height > maxY) height = maxY;

				// 素材の割り当て
				int dirtThickness = 3;
				if (height - dirtThickness < 0) dirtThickness = height;

				// 境界を計算（yの区間でブロックIDが決まる）
				const int stoneEnd = my_max(0, height - dirtThickness); // [0, stoneEnd)
				const int dirtEnd = my_max(0, height - 1);              // [stoneEnd, dirtEnd)
				const int lawnY = height - 1;                           // y==lawnY が Lawn（height>0のとき）

				// BlockConfig::GetBlockInfo() を毎回呼ばず、必要な分だけ取得
				const Blockinfo stoneInfo = blockConfig_->GetBlockInfo(BlockID::Stone);
				const Blockinfo dirtInfo = blockConfig_->GetBlockInfo(BlockID::Dirt);
				const Blockinfo lawnInfo = blockConfig_->GetBlockInfo(BlockID::Lawn);
				const Blockinfo airInfo = blockConfig_->GetBlockInfo(BlockID::Air);

				for (int y = 0; y < CHUNK_Y; ++y)
				{
					const float cx = baseX + x * BLOCK_SIZE;
					const float cy = y * BLOCK_SIZE + half;
					const float cz = baseZ + z * BLOCK_SIZE;

					// ブロックID決定
					BlockID id;
					if (y < height - dirtThickness) id = BlockID::Stone;
					else if (y < height - 1)       id = BlockID::Dirt;
					else if (y < height)           id = BlockID::Lawn;
					else                            id = BlockID::Air;

					// 中心座標を直計算
					const Vector3 center(cx, cy, cz);

					blocks[x][y][z]->SetBlockType(blockConfig_->GetBlockInfo(id));
					blocks[x][y][z]->SetBlockPosition(center);

					if (id != BlockID::Air)
					{
						blockPositions[id].emplace_back(x, y, z);
					}
				}
			}
		}
	}

	//else
	//{
	//	// チャンク内すべてのブロック生成 
	//	for (int x = 0; x < CHUNK_X; ++x)
	//	{
	//		for (int z = 0; z < CHUNK_Z; ++z)
	//		{
	//			// ワールド座標でのブロックインデックス 
	//			const int worldX = chunkPos.x * CHUNK_X + x; const int worldZ = chunkPos.y * CHUNK_Z + z;
	//			// ワールド座標をノイズサンプル空間へスケールダウン（連続性が鍵）
	//			const float sampleX = static_cast<float>(worldX) / param.scale;
	//			const float sampleZ = static_cast<float>(worldZ) / param.scale;
	//
	//			// フラクタルノイズ（0..1）
	//			float n = fractalPerlin(param.pn, sampleX, sampleZ, param.octaves, param.persistence);
	//
	//			// 高さへ変換（0..maxHeight-1）
	//			int height = static_cast<int>(std::floor(n * float(param.height - 1) + 0.5f));
	//			if (height < 0) height = 0;
	//			if (height > param.height - 1) height = param.height - 1;
	//
	//			// 素材の割り当て
	//			int dirtThickness = 3;
	//			if (height - dirtThickness < 0) dirtThickness = height;
	//
	//			for (int y = 0; y < CHUNK_Y; ++y)
	//			{
	//				// ブロックID決定
	//				BlockID id;
	//				if (y < height - dirtThickness) id = BlockID::Stone;
	//				else if (y < height - 1)       id = BlockID::Dirt;
	//				else if (y < height)           id = BlockID::Lawn;
	//				else                            id = BlockID::Air;
	//				// ブロックのAABB取得
	//				AABB aabb = GetAABB(Vector3int(x, y, z));
	//				// ブロックの中心座標取得
	//				Vector3 center = aabb.center();
	//
	//				blocks[x][y][z]->SetBlockType(blockConfig_->GetBlockInfo(id));
	//				blocks[x][y][z]->SetBlockPosition(center);
	//				blockPositions[id].emplace_back(x, y, z);
	//			}
	//		}
	//	}
	//}


	SetExposedBlocks();
}

void Chunk::SetNeighborChunk(int direction, Chunk* neighbor)
{
	if (direction < 0 || direction >= 4) return;
	neighbors[direction] = neighbor;
}

bool Chunk::IsNeighborExist(int direction)
{
	return neighbors[direction] != nullptr;
}

// チャンク内の全てのブロックの露出状態を更新
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
					Vector3int neighborIndex(x + dx[i], y + dy[i], z + dz[i]);

					Block* neighborBlock = GetBlock(neighborIndex);

					// 隣接ブロックが存在しない(チャンクがまだ生成されていない)なら露出している
					if (!neighborBlock)
					{
						continue;
					}

					// 隣接ブロックがAirかGlassなら露出している
					if (neighborBlock->GetBlockID() == BlockID::Air ||
						neighborBlock->GetBlockID() == BlockID::Glass)
					{
						exposed = true;
						break;
					}
				}

				const bool wasExposed = blocks[x][y][z]->isExposed_;
				blocks[x][y][z]->isExposed_ = exposed;

				if (blocks[x][y][z]->isExposed_ && !wasExposed)
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

// localIndexの周り６ブロックの露出状態を更新
void Chunk::UpdateExposedAround(const Vector3int& localIndex)
{
	// 指定座標がチャンク境界を跨ぐ場合、対象チャンクとローカル座標へ変換する
	auto TryResolveToChunkLocal = [&](int x, int y, int z, Chunk*& outChunk, Vector3int& outLocal) -> bool
		{
			outChunk = this;
			outLocal = Vector3int(x, y, z);

			// yは跨がない想定
			if (y < 0 || y >= CHUNK_Y) return false;

			// 自チャンク内
			if (0 <= x && x < CHUNK_X && 0 <= z && z < CHUNK_Z)
			{
				return true;
			}

			// X方向に跨ぐ
			if (x < 0)
			{
				outChunk = neighbors[1]; // -X
				if (!outChunk) return false;
				outLocal.x = CHUNK_X - 1;
				return (0 <= z && z < CHUNK_Z);
			}
			if (x >= CHUNK_X)
			{
				outChunk = neighbors[0]; // +X
				if (!outChunk) return false;
				outLocal.x = 0;
				return (0 <= z && z < CHUNK_Z);
			}

			// Z方向に跨ぐ
			if (z < 0)
			{
				outChunk = neighbors[3]; // -Z
				if (!outChunk) return false;
				outLocal.z = CHUNK_Z - 1;
				return (0 <= x && x < CHUNK_X);
			}
			if (z >= CHUNK_Z)
			{
				outChunk = neighbors[2]; // +Z
				if (!outChunk) return false;
				outLocal.z = 0;
				return (0 <= x && x < CHUNK_X);
			}

			return false;
		};



	static const int dx[6] = { -1, 1, 0, 0, 0, 0 };
	static const int dy[6] = { 0, 0, -1, 1, 0, 0 };
	static const int dz[6] = { 0, 0, 0, 0, -1, 1 };

	for (int i = 0; i < 6; ++i)
	{
		const int wx = localIndex.x + dx[i];
		const int wy = localIndex.y + dy[i];
		const int wz = localIndex.z + dz[i];

		Chunk* targetChunk = nullptr;
		Vector3int targetLocal;
		if (!TryResolveToChunkLocal(wx, wy, wz, targetChunk, targetLocal)) continue;

		Block* nb = targetChunk->blocks[targetLocal.x][targetLocal.y][targetLocal.z].get();
		if (!nb) continue;
		if (!nb->isActive_) continue;
		if (nb->GetBlockID() == BlockID::Air) continue;

		// 「隣にAirがある」ので露出確定：破壊の場合は基本的に近傍は露出する方向にしか変化しない
		if (!nb->isExposed_)
		{
			nb->isExposed_ = true;

			// 露出した瞬間だけインスタンスを作る（描画データは targetChunk 側の blockData_ を使う）
			const BlockID id = nb->GetBlockID();
			nb->instanceIndex_ = targetChunk->blockData_[id]->AddNewBlock(nb->position_, targetLocal);
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

// チャンクを跨いだブロックも取得できる
Block* Chunk::GetBlock(const Vector3int& localIndex) 
{
	// 存在しないブロック
	if (localIndex.y < 0 || localIndex.y >= CHUNK_Y) return nullptr;

	// チャンク内
	if (0 <= localIndex.x && localIndex.x < CHUNK_X &&
		0 <= localIndex.y && localIndex.y < CHUNK_Y &&
		0 <= localIndex.z && localIndex.z < CHUNK_Z)
	{
		return blocks[localIndex.x][localIndex.y][localIndex.z].get();
	}

	// チャンク外かつX,Z両方方向に跨いでいる場合はめんどくさいから実装しない
	const bool xOut = (localIndex.x < 0 || localIndex.x >= CHUNK_X);
	const bool zOut = (localIndex.z < 0 || localIndex.z >= CHUNK_Z);
	if (xOut && zOut) return nullptr;

	Vector3int neighborLocal = localIndex;

	// X方向に跨ぐ
	if (localIndex.x < 0)
	{
		Chunk* nb = neighbors[1]; // -X
		if (!nb) return nullptr;
		neighborLocal.x = CHUNK_X - 1;
		return nb->blocks[neighborLocal.x][neighborLocal.y][neighborLocal.z].get();
	}
	if (localIndex.x >= CHUNK_X)
	{
		Chunk* nb = neighbors[0]; // +X
		if (!nb) return nullptr;
		neighborLocal.x = 0;
		return nb->blocks[neighborLocal.x][neighborLocal.y][neighborLocal.z].get();
	}

	// Z方向に跨ぐ
	if (localIndex.z < 0)
	{
		Chunk* nb = neighbors[3]; // -Z
		if (!nb) return nullptr;
		neighborLocal.z = CHUNK_Z - 1;
		return nb->blocks[neighborLocal.x][neighborLocal.y][neighborLocal.z].get();
	}
	if (localIndex.z >= CHUNK_Z)
	{
		Chunk* nb = neighbors[2]; // +Z
		if (!nb) return nullptr;
		neighborLocal.z = 0;
		return nb->blocks[neighborLocal.x][neighborLocal.y][neighborLocal.z].get();
	}

	return nullptr;
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

void Chunk::DestroyBlock(const Vector3int& localIndex)
{
	const BlockID preBlockID = blocks[localIndex.x][localIndex.y][localIndex.z]->GetBlockID();

	if (!blocks[localIndex.x][localIndex.y][localIndex.z]) return;
	if (!blocks[localIndex.x][localIndex.y][localIndex.z]->isActive_ || 
		preBlockID == BlockID::Air) return;

	// BlockをAirに置換
	blocks[localIndex.x][localIndex.y][localIndex.z]->isActive_ = false;
	blocks[localIndex.x][localIndex.y][localIndex.z]->blockID = BlockID::Air;
	blocks[localIndex.x][localIndex.y][localIndex.z]->isExposed_ = false;
	// blockPositions[preBlockID]の
	// [localIndex.x][localIndex.y][localIndex.z]の位置を削除
	auto& positions = blockPositions[preBlockID];
	positions.erase(
		std::remove(positions.begin(), positions.end(), localIndex),
		positions.end()
	);

	blockPositions[BlockID::Air].emplace_back(localIndex);

	// 描画データから削除
	blockData_[preBlockID]->RemoveBlock(localIndex);

	// 周囲の露出判定を更新して、露出したら描画登録
	UpdateExposedAround(localIndex);
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
