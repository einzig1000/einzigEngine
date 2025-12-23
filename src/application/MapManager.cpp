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
#include "Chunk.h"

MapManager::MapManager(Player* player)
{
	// プレイヤー参照保存
	player_ = player;

	blockConfig_ = new BlockConfig();
}

MapManager::~MapManager()
{
	delete blockConfig_;
	blockConfig_ = nullptr;


	for (auto& item : dropItems_)
	{
		delete item;
		item = nullptr;
	}
}

void MapManager::LoadMap(const std::string& mapFilePath)
{
	mapFilePath_ = mapFilePath;

	std::ifstream file(mapFilePath_);

	if (!file.is_open())
	{

	}
}

void MapManager::SaveMap(const std::string& mapFilePath)
{
	for (const auto& [chunkPos, chunk] : chunks)
	{
		chunk->SaveChunkData(mapFilePath_);
	}
}

void MapManager::CreateNewMap()
{

}

// チャンク生成
Chunk* MapManager::CreateChunk(const Vector2int& chunkPos)
{
	Chunk* chunk = new Chunk();
	chunk->chunkPos = chunkPos;

	const int maxHeight = CHUNK_Z;
	const float scale = 32.0f;         // 地形の粗さ（大きくすると緩やか）
	const int octaves = 4;             // 反復回数 (大きくすると細かい起伏が増える)
	const float persistence = 0.5f;    // 各オクターブの振幅減衰 (大きくすると細かい起伏が増える)
	static const unsigned int seed = 12345;	// 俗に言うシード値
	static PerlinNoise pn(seed);


	// チャンク内すべてのブロック生成
	for (int x = 0; x < CHUNK_X; ++x)
	{
		for (int z = 0; z < CHUNK_Y; ++z)
		{
			// ワールド座標でのブロックインデックス
			const int worldX = chunkPos.x * CHUNK_X + x;
			const int worldZ = chunkPos.y * CHUNK_Y + z;

			// ワールド座標をノイズサンプル空間へスケールダウン（連続性が鍵）
			const float sampleX = static_cast<float>(worldX) / scale;
			const float sampleZ = static_cast<float>(worldZ) / scale;

			// フラクタルノイズ（0..1）
			float n = fractalPerlin(pn, sampleX, sampleZ, octaves, persistence);

			// 高さへ変換（0..maxHeight-1）
			int height = static_cast<int>(std::floor(n * float(maxHeight - 1) + 0.5f));
			if (height < 0) height = 0;
			if (height > maxHeight - 1) height = maxHeight - 1;

			// 素材の割り当て（例）
			int dirtThickness = 3;
			if (height - dirtThickness < 0) dirtThickness = height;

			for (int y = 0; y < CHUNK_Z; ++y)
			{
				// ブロックID決定
				BlockID id;
				if (y < height - dirtThickness) id = BlockID::Stone;
				else if (y < height - 1)		id = BlockID::Dirt;
				else if (y < height)			id = BlockID::Lawn;
				else                            id = BlockID::Air;
				// ブロックのAABB取得
				AABB aabb = GetAABB(chunkPos, Vector3int(x, y, z));
				// ブロックの中心座標取得
				Vector3 center = aabb.center();

				chunk->blocks[x][y][z]->SetBlockType(blockConfig_->GetBlockInfo(id));
				chunk->blocks[x][y][z]->aabb_ = aabb;
				chunk->blocks[x][y][z]->SetBlockPosition(center);
			}
		}
	}

	chunk->SetExposedBlocks();

	return chunk;
}

// チャンク取得、なければCreateChunk
Chunk* MapManager::GetOrCreateChunk(const Vector2int& chunkPos)
{
	auto it = chunks.find(chunkPos);
	if (it != chunks.end())
		return it->second;

	// なければ作る
	Chunk* newChunk = CreateChunk(chunkPos);
	chunks[chunkPos] = newChunk;
	return newChunk;
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

	Vector2int playerIndex = ChunkIndexByPosition(player_->GetRenderData().translate.value);

	// プレイヤーから周囲2チャンクは更新する
	for (int32_t dx = -1; dx <= 1; ++dx)
	{
		for (int32_t dz = -1; dz <= 1; ++dz)
		{
			Vector2int chunkPos = Vector2int(playerIndex.x + dx, playerIndex.y + dz);
			Chunk* chunk = GetOrCreateChunk(chunkPos);
			chunk->Update();
		}
	}
}

void MapManager::UpDataPlayerRayCollision()
{
//	// プレイヤー視点のインデックス
//	Vector3 rayStart = player_->viewLine_.origin;
//	Vector3 rayEnd = player_->viewLine_.end;
//	Vector3 dir = (rayEnd - rayStart).Normalized();
//
//	// 現在のインデックス
//	Vector3int currentIdx = IndexByPosition(rayStart);
//	// 最終目的地のインデックス
//	Vector3int endIdx = IndexByPosition(rayEnd);
//
//	// 各軸に進む方向 (1 または -1)
//	int stepX = (dir.x > 0) ? 1 : -1;
//	int stepY = (dir.y > 0) ? 1 : -1;
//	int stepZ = (dir.z > 0) ? 1 : -1;
//
//	// 次の境界線までの距離を算出するための準備
//	// deltaDist: その軸方向にブロック1つ分進むのに必要なレイの長さ
//	float deltaDistX = std::abs(BLOCK_SIZE / dir.x);
//	float deltaDistY = std::abs(BLOCK_SIZE / dir.y);
//	float deltaDistZ = std::abs(BLOCK_SIZE / dir.z);
//
//	// sideDist: 現在地から「次の境界線」までのレイの長さ
//	Vector3 blockPos = PositionByIndex(currentIdx); // ブロックの中心（または最小角）
//	float sideDistX = (stepX > 0) ? (blockPos.x + BLOCK_SIZE - rayStart.x) : (rayStart.x - blockPos.x);
//	float sideDistY = (stepY > 0) ? (blockPos.y + BLOCK_SIZE - rayStart.y) : (rayStart.y - blockPos.y);
//	float sideDistZ = (stepZ > 0) ? (blockPos.z + BLOCK_SIZE - rayStart.z) : (rayStart.z - blockPos.z);
//
//	sideDistX = (dir.x != 0) ? sideDistX / std::abs(dir.x) : FLT_MAX;
//	sideDistY = (dir.y != 0) ? sideDistY / std::abs(dir.y) : FLT_MAX;
//	sideDistZ = (dir.z != 0) ? sideDistZ / std::abs(dir.z) : FLT_MAX;
//
//	// 最大ループ回数（安全のため。レイの長さに応じて調整）
//	int maxSteps = 100;
//
//	for (int i = 0; i < maxSteps; i++)
//	{
//		// 衝突判定
//		if (block_[currentIdx.x][currentIdx.y][currentIdx.z]->GetBlockID() != BlockID::Air)
//		{
//			block_[currentIdx.x][currentIdx.y][currentIdx.z]->isCollisionRay = true;
//			return; // 衝突したら終了
//		}
//
//		// ゴールに到達したら終了
//		if (currentIdx == endIdx) break;
//
//		// 最も近い境界線（X, Y, Zのどれか）を跨ぐ
//		if (sideDistX < sideDistY)
//		{
//			if (sideDistX < sideDistZ)
//			{
//				sideDistX += deltaDistX;
//				currentIdx.x += stepX;
//				//block_[currentIdx.x][currentIdx.y][currentIdx.z]->direction =
//			}
//			else
//			{
//				sideDistZ += deltaDistZ;
//				currentIdx.z += stepZ;
//			}
//		}
//		else
//		{
//			if (sideDistY < sideDistZ)
//			{
//				sideDistY += deltaDistY;
//				currentIdx.y += stepY;
//			}
//			else
//			{
//				sideDistZ += deltaDistZ;
//				currentIdx.z += stepZ;
//			}
//		}
//	}
}

void MapManager::Draw()
{
	Vector2int playerIndex = ChunkIndexByPosition(player_->GetRenderData().translate.value);


	// プレイヤーから周囲5チャンクは描画する
	//for (int32_t dx = -2; dx <= 2; ++dx)
	//{
	//	for (int32_t dz = -2; dz <= 2; ++dz)
	//	{
	//		Vector2int chunkPos = Vector2int(playerIndex.x + dx, playerIndex.y + dz);
	//		Chunk* chunk = GetOrCreateChunk(chunkPos);
	//		chunk->Draw();
	//	}
	//}

	// 存在するすべてのチャンクを描画（デバッグ用）
	for (const auto& [chunkPos, chunk] : chunks)
	{
		chunk->Draw();
	}

	for (auto& item : dropItems_)
	{
		item->Draw();
	}
}

void MapManager::DrawImGui()
{

}

AABB MapManager::GetAABB(const Vector2int& chunkPos, const Vector3int& index)
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
AABB MapManager::GetAABB(const Vector3& position)
{
	Vector2int chunkPos = ChunkIndexByPosition(position);
	Vector3int index = BlockIndexByPosition(position);
	return GetAABB(chunkPos, index);
}
bool MapManager::GetIsActive(const Vector2int& chunkPos, const Vector3int& index)
{
	Vector3int clampedIndex = index;
	clampedIndex.x = std::clamp(index.x, 0, CHUNK_X - 1);
	clampedIndex.y = std::clamp(index.y, 0, CHUNK_Z - 1);
	clampedIndex.z = std::clamp(index.z, 0, CHUNK_Y - 1);
	Chunk* chunk = GetOrCreateChunk(chunkPos);
	return chunk->blocks[clampedIndex.x][clampedIndex.y][clampedIndex.z]->isActive_;
}
bool MapManager::GetIsActive(const Vector3& position)
{
	Vector2int chunkPos = ChunkIndexByPosition(position);
	Vector3int index = BlockIndexByPosition(position);
	return GetIsActive(chunkPos, index);
}

// position が今どのチャンクに属しているか  例：position=(34, 0, 50) -> chunkIndex=(1, 2)
Vector2int MapManager::ChunkIndexByPosition(const Vector3& position)
{
	int bx = static_cast<int>(std::floor(position.x / BLOCK_SIZE));
	int bz = static_cast<int>(std::floor(position.z / BLOCK_SIZE));

	Vector2int chunk;
	chunk.x = static_cast<int>(std::floor((float)bx / CHUNK_X));
	chunk.y = static_cast<int>(std::floor((float)bz / CHUNK_Z));
	return chunk;
}

int LocalMod(int a, int n)
{
	return (a % n + n) % n;
}

// position が今チャンク内どのブロックに属しているか(どんな時も0～CHUNK_SIZE-1の範囲に収まる)  例：position=(34, 0, 50) -> localIndex=(2, 0, 2)
Vector3int MapManager::BlockIndexByPosition(const Vector3& position)
{
	// ① ワールド座標 → 世界ブロック座標
	int bx = static_cast<int>(std::floor(position.x / BLOCK_SIZE));
	int by = static_cast<int>(std::floor(position.y / BLOCK_SIZE));
	int bz = static_cast<int>(std::floor(position.z / BLOCK_SIZE));

	// ② チャンク内インデックスへ正規化（数学的 mod）
	Vector3int local;
	local.x = LocalMod(bx, CHUNK_X);
	local.y = LocalMod(by, CHUNK_Y);
	local.z = LocalMod(bz, CHUNK_Z);

	return local;
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
