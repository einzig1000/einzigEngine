#include "Chunk.h"
#include "Block/Block.h"
#include "Utilities/JsonManager.h"
#include "Block/BlockConfig.h"
#include "Block/BlockDurability.h"

namespace
{
	// chunkごとに決定論的に乱数を出す（seed + chunk座標で固定化）
	static uint32_t Hash32(uint32_t x)
	{
		x ^= x >> 16;
		x *= 0x7feb352d;
		x ^= x >> 15;
		x *= 0x846ca68b;
		x ^= x >> 16;
		return x;
	}

	static uint32_t MakeChunkSeed(uint32_t seed, const Vector2int& chunkPos)
	{
		uint32_t h = seed;
		h ^= Hash32(static_cast<uint32_t>(chunkPos.x));
		h ^= Hash32(static_cast<uint32_t>(chunkPos.y) + 0x9e3779b9u);
		return Hash32(h);
	}

	static int RandRange(std::mt19937& rng, int minV, int maxV)
	{
		std::uniform_int_distribution<int> dist(minV, maxV);
		return dist(rng);
	}

	static float Rand01(std::mt19937& rng)
	{
		std::uniform_real_distribution<float> dist(0.0f, 1.0f);
		return dist(rng);
	}
}

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
	if (loadResult)CreateChunkDataFromJson();
	// 新規生成の場合
	else CreateChunkDataNewly(param, chunkPos);

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

// Jsonから読み込まれたデータを元にチャンクデータを生成
void Chunk::CreateChunkDataFromJson()
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

// 新規生成されたチャンクデータを作成
void Chunk::CreateChunkDataNewly(const NoiseParameter & param, const Vector2int & chunkPos)
{
	// 既存データをクリア(Json読んでないからあるはずない)
	blockPositions.clear();

	// 事前に定数を計算
	const float invScale = 1.0f / param.scale;
	const int maxY = param.height - 1;

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
			std::mt19937 rng(MakeChunkSeed(param.seed, chunkPos));
			int dirtThickness = RandRange(rng, 2, 5);
			if (height - dirtThickness < 0) dirtThickness = height;

			// 境界を計算（yの区間でブロックIDが決まる）
			const int stoneEnd = my_max(0, height - dirtThickness); // [0, stoneEnd)
			const int dirtEnd = my_max(0, height - 1);              // [stoneEnd, dirtEnd)
			const int lawnY = height - 1;                           // y==lawnY が Lawn（height>0のとき）

			for (int y = 0; y < CHUNK_Y; ++y)
			{
				// ブロックID決定
				BlockID id;
				
				if (y == 0)							 id = BlockID::Bedrock;
				else if (y < height - dirtThickness) id = BlockID::Stone;
				else if (y < height - 1)			 id = BlockID::Dirt;
				else if (y < height)				 id = BlockID::Lawn;
				else								 id = BlockID::Air;

				SetBlockLocal(Vector3int(x, y, z), id);
			}
		}
	}

	GenerateOres(param);
	GenerateTrees(param);
}

void Chunk::GenerateOres(const NoiseParameter& param)
{
	// chunkごと固定の乱数（同じseed＆chunkPosなら必ず同じ鉱脈）
	std::mt19937 rng(MakeChunkSeed(param.seed, chunkPos));

	auto ClampY = [&](int& minY, int& maxY)
		{
			minY = my_max(0, minY);
			maxY = my_min(CHUNK_Y - 1, maxY);
			if (minY > maxY) std::swap(minY, maxY);
		};

	// 6近傍ランダムウォーク鉱脈
	auto CarveVeins = [&](BlockID oreId, int veinsPerChunk, int sizeMean, int sizeRand, int minY, int maxY)
		{
			if (veinsPerChunk <= 0) return;
			if (sizeMean <= 0) return;

			ClampY(minY, maxY);

			static const int dx[6] = { -1, 1, 0, 0, 0, 0 };
			static const int dy[6] = { 0, 0, -1, 1, 0, 0 };
			static const int dz[6] = { 0, 0, 0, 0, -1, 1 };

			for (int v = 0; v < veinsPerChunk; ++v)
			{
				int x = RandRange(rng, 0, CHUNK_X - 1);
				int y = RandRange(rng, minY, maxY);
				int z = RandRange(rng, 0, CHUNK_Z - 1);

				int veinSize = sizeMean + RandRange(rng, -sizeRand, sizeRand);
				if (veinSize < 1) veinSize = 1;

				for (int i = 0; i < veinSize; ++i)
				{
					// Stone のみ置換（Bedrock/Dirt/Lawnは壊さない）
					if (blocks[x][y][z]->GetBlockID() == BlockID::Stone)
					{
						SetBlockLocal(Vector3int(x, y, z), oreId);
					}

					// 次へ（ランダムウォーク）
					const int dir = RandRange(rng, 0, 5);
					x = my_min(CHUNK_X - 1, my_max(0, x + dx[dir]));
					y = my_min(CHUNK_Y - 1, my_max(0, y + dy[dir]));
					z = my_min(CHUNK_Z - 1, my_max(0, z + dz[dir]));

					// 高さ帯から外れたら戻す（分布を安定させる）
					if (y < minY) y = minY;
					if (y > maxY) y = maxY;
				}
			}
		};

	CarveVeins(BlockID::Iron,
		param.ironVeinsPerChunk,
		param.ironVeinSizeMean,
		param.ironVeinSizeRand,
		param.ironMinY,
		param.ironMaxY);

	CarveVeins(BlockID::Diamond,
		param.diamondVeinsPerChunk,
		param.diamondVeinSizeMean,
		param.diamondVeinSizeRand,
		param.diamondMinY,
		param.diamondMaxY);
}

void Chunk::GenerateTrees(const NoiseParameter& param)
{
	std::mt19937 rng(MakeChunkSeed(param.seed ^ 0xA53A9C1Du, chunkPos));

	auto FindSurfaceY_Lawn = [&](int x, int z) -> int
		{
			for (int y = CHUNK_Y - 1; y >= 0; --y)
			{
				if (blocks[x][y][z]->GetBlockID() == BlockID::Lawn) return y;
			}
			return -1;
		};

	auto CanPlaceTrunk = [&](int x, int y0, int z, int height) -> bool
		{
			if (y0 < 0 || y0 + height >= CHUNK_Y) return false;
			for (int y = y0; y < y0 + height; ++y)
			{
				if (blocks[x][y][z]->GetBlockID() != BlockID::Air) return false;
			}
			return true;
		};

	for (int x = 0; x < CHUNK_X; ++x)
	{
		for (int z = 0; z < CHUNK_Z; ++z)
		{
			if (Rand01(rng) > param.treeChancePerColumn) continue;

			const int groundY = FindSurfaceY_Lawn(x, z);
			if (groundY < 0) continue;

			const int trunkY0 = groundY + 1;
			const int trunkH = RandRange(rng, param.treeHeightMin, param.treeHeightMax);

			// チャンク内に収まる木だけ作る（跨ぎは後回し）
			if (!CanPlaceTrunk(x, trunkY0, z, trunkH)) continue;

			// 幹
			for (int y = trunkY0; y < trunkY0 + trunkH; ++y)
			{
				SetBlockLocal(Vector3int(x, y, z), BlockID::Wood);
			}

			// 葉（幹先端に球っぽく）
			const int leafRadius = RandRange(rng, param.leafRadiusMin, param.leafRadiusMax);
			const int leafCenterY = trunkY0 + trunkH - 1;

			for (int ly = leafCenterY - leafRadius; ly <= leafCenterY + leafRadius; ++ly)
			{
				if (ly < 0 || ly >= CHUNK_Y) continue;

				for (int lx = x - leafRadius; lx <= x + leafRadius; ++lx)
				{
					if (lx < 0 || lx >= CHUNK_X) continue;

					for (int lz = z - leafRadius; lz <= z + leafRadius; ++lz)
					{
						if (lz < 0 || lz >= CHUNK_Z) continue;

						const int dx0 = lx - x;
						const int dy0 = ly - leafCenterY;
						const int dz0 = lz - z;

						// 球っぽい形
						if (dx0 * dx0 + dy0 * dy0 + dz0 * dz0 > leafRadius * leafRadius + 1) continue;

						// 空気だけ葉にする（地形と幹を潰さない）
						if (blocks[lx][ly][lz]->GetBlockID() == BlockID::Air)
						{
							SetBlockLocal(Vector3int(lx, ly, lz), BlockID::Leaf);
						}
					}
				}
			}
		}
	}
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
				static const int dx[6] = { -1, 1, 0, 0, 0, 0 };
				static const int dz[6] = { 0, 0, 0, 0, -1, 1 };
				static const int dy[6] = { 0, 0, -1, 1, 0, 0 };

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

Vector3 Chunk::LocalCenter(const Vector3int& index) const
{
	const float half = BLOCK_SIZE * 0.5f;
	const float baseX = chunkPos.x * CHUNK_X * BLOCK_SIZE + half;
	const float baseZ = chunkPos.y * CHUNK_Z * BLOCK_SIZE + half;

	const float cx = baseX + index.x * BLOCK_SIZE;
	const float cy = index.y * BLOCK_SIZE + half;
	const float cz = baseZ + index.z * BLOCK_SIZE;

	return Vector3(cx, cy, cz);
}

void Chunk::SetBlockLocal(const Vector3int& index, const BlockID id)
{
	const BlockID oldId = blocks[index.x][index.y][index.z]->GetBlockID();
	if (oldId == id) return;

	// blockPositions更新
	if (oldId != BlockID::Air)
	{
		auto& v = blockPositions[oldId];
		v.erase(std::remove(v.begin(), v.end(), index), v.end());
	}

	// ブロック更新
	blocks[index.x][index.y][index.z]->SetBlockType(blockConfig_->GetBlockInfo(id));
	blocks[index.x][index.y][index.z]->SetBlockPosition(LocalCenter(index));

	// blockPositions 更新（Airは記録しない）
	if (id != BlockID::Air)
	{
		blockPositions[id].emplace_back(index);
	}
}

void Chunk::DestroyBlock(const Vector3int& localIndex)
{
	Block* block = blocks[localIndex.x][localIndex.y][localIndex.z].get();
	if (!block) return;

	const BlockID preBlockID = block->GetBlockID();
	if (!block->isActive_ || preBlockID == BlockID::Air) return;

	// ブロックをAirに置換（blockPositionsの整合もここで取る）
	SetBlockLocal(localIndex, BlockID::Air);

	// ブロック側の状態フラグ
	block->isExposed_ = false;

	// 描画データから削除
	blockData_[preBlockID]->RemoveBlock(localIndex);

	// 周囲の露出判定更新
	UpdateExposedAround(localIndex);
}

void Chunk::RebuildBlockPositions()
{}

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
