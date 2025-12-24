#include "MapManager.h"
#include <fstream>
#include <sstream>
#include "Block/Block.h"
#include "Player.h"
#include "PerlinNoise.h"
#include "Engine.h"
#include "Itemslot.h"
#include "Block/BlockDurability.h"
#include "Chunk.h"

MapManager::MapManager(Player* player)
{
	// プレイヤー参照保存
	player_ = player;
}

MapManager::~MapManager()
{
	// チャンク解放
	for (auto& pair : chunks)
	{
		delete pair.second;
		pair.second = nullptr;
	}

	for (auto& item : dropItems_)
	{
		delete item;
		item = nullptr;
	}
}

void MapManager::Initialize()
{
	CreateNewMap(12345);
}

void MapManager::CreateNewMap(uint32_t seed)
{
	// ノイズパラメータ設定
	noiseParam_.seed = seed;			// 俗に言うシード値
	noiseParam_.scale = 32.0f;			// 地形の粗さ（大きくすると緩やか）
	noiseParam_.octaves = 4;			// 反復回数 (大きくすると細かい起伏が増える)
	noiseParam_.persistence = 0.5f;		// 各オクターブの振幅減衰 (大きくすると細かい起伏が増える)
	noiseParam_.pn = PerlinNoise(seed);	// PerlinNoise インスタンス生成

	//Vector2int playerIndex = ChunkIndexByPosition(player_->GetRenderData().translate.value);
	//// プレイヤー周辺のチャンクを生成スケジュールに登録
	//for (int dx = -20; dx <= 20; ++dx)
	//{
	//	for (int dz = -20; dz <= 20; ++dz)
	//	{
	//		Vector2int chunkPos = { playerIndex.x + dx, playerIndex.y + dz };
	//		EnsureChunkScheduled(chunkPos);
	//	}
	//}
}

void MapManager::LoadMap(const std::string& mapFilePath)
{
	// chunks 全解放&clear
	for (auto& pair : chunks)
	{
		delete pair.second;
		pair.second = nullptr;
	}
	// chunkGenQueue_ を空に
	while (!chunkGenQueue_.empty())
	{
		chunkGenQueue_.pop();
	}
	// chunkScheduled_/chunkCreated_ をclear
	chunkScheduled_.clear();
	chunkCreated_.clear();


	mapFilePath_ = mapFilePath;

	JsonManager json;
	json.LoadFromJson(*this, mapFilePath);

	// ロード後プレイヤーが乗ってるチャンクを生成
	Vector2int playerIndex = ChunkIndexByPosition(player_->GetRenderData().translate.value);
	if (!(chunkCreated_.find(playerIndex) != chunkCreated_.end()))
	{
		EnsureChunkScheduled(playerIndex);
		ProcessChunkGeneration();
	}

	player_->data_.translate.value.y = 500.0f;
	player_->data_.translate.velocity.y = 0.0f;
	player_->data_.translate.acceleration.y = 0.0f;
}

void MapManager::SaveMap(const std::string& mapFilePath)
{
	JsonManager json;
	json.SaveToJson(*this, mapFilePath);
}

// チャンク有無確認
bool MapManager::HasChunk(const Vector2int& chunkPos) const
{
	return chunks.find(chunkPos) != chunks.end();
}

// チャンク取得、なくても生成はしない
Chunk* MapManager::TryGetChunk(const Vector2int& chunkPos) const
{
	auto it = chunks.find(chunkPos);
	if (it != chunks.end()) return it->second;
	return nullptr;
}

// チャンク取得、なければスケジュールに登録して生成
Chunk* MapManager::GetOrCreateChunk(const Vector2int& chunkPos)
{
	// chunkCreated_から既に生成済みか確認
	if (chunkCreated_.find(chunkPos) != chunkCreated_.end())
	{
		return TryGetChunk(chunkPos);
	}


	// 欲しいチャンクが存在しなければスケジュールに登録
	EnsureChunkScheduled(chunkPos);
	// スケジュールに登録されたチャンクを1つ生成
	ProcessChunkGeneration();

	return TryGetChunk(chunkPos);
}

// 欲しいチャンクが存在しなければスケジュールに登録
void MapManager::EnsureChunkScheduled(const Vector2int& chunkPos)
{
	// チャンクが既に作成されているならreturn
	if (chunkCreated_.find(chunkPos) != chunkCreated_.end()) return;
	// 既にスケジュール済みならreturn
	if (chunkScheduled_.find(chunkPos) != chunkScheduled_.end()) return;
	// スケジュール登録
	chunkGenQueue_.push(chunkPos);
	// スケジュール済み集合にも登録
	chunkScheduled_.insert(chunkPos);

	// chunkGenQueue_をプレイヤー位置から近い順にソートする
	std::vector<Vector2int> tempQueue;
	while (!chunkGenQueue_.empty())
	{
		tempQueue.push_back(chunkGenQueue_.front());
		chunkGenQueue_.pop();
	}
	Vector2int playerIndex = ChunkIndexByPosition(player_->GetRenderData().translate.value);
	std::sort(tempQueue.begin(), tempQueue.end(),
		[playerIndex](const Vector2int& a, const Vector2int& b)
		{
			int distA = (a.x - playerIndex.x) * (a.x - playerIndex.x) + (a.y - playerIndex.y) * (a.y - playerIndex.y);
			int distB = (b.x - playerIndex.x) * (b.x - playerIndex.x) + (b.y - playerIndex.y) * (b.y - playerIndex.y);
			return distA < distB;
		});
	for (const auto& pos : tempQueue)
	{
		chunkGenQueue_.push(pos);
	}
}

// スケジュールに登録されたチャンクを1つ生成
void MapManager::ProcessChunkGeneration()
{
	// スケジュールキューが空ではないなら作成
	if (!chunkGenQueue_.empty())
	{
		// キューから取り出し
		Vector2int pos = chunkGenQueue_.front();
		chunkGenQueue_.pop();
		chunkScheduled_.erase(pos);

		// Jsonから読み取り座標だけ設定されていた場合(chunksに存在しているがデータがない場合)はデータを生成
		if (HasChunk(pos))
		{
			Chunk* chunk = TryGetChunk(pos);
			if (chunk && chunk->loadResult)
			{
				// データ生成
				chunk->CreateChunkData(noiseParam_, pos);
			}
			else
			{
				assert(false && "既に存在しているチャンクにデータが存在しません。");
			}
		}
		// 完全に新規の場合
		else
		{
			// チャンク生成
			Chunk* chunk = new Chunk();
			chunk->CreateChunkData(noiseParam_, pos);
			chunks[pos] = chunk;
		}

		// 生成済み集合に登録
		chunkCreated_.insert(pos);
	}
}


void MapManager::Update()
{
	//UpDataPlayerRayCollision();

	if (Game::Input::Key::IsJustPressed(DIK_0))
	{
		LoadMap(mapFilePath_);
	}

	if (Game::Input::Key::IsJustPressed(DIK_1))
	{
		SaveMap(mapFilePath_);
	}

	// 生成を段階的に実行
	ProcessChunkGeneration();

	// プレイヤー視点のインデックス
	Vector2int playerIndex = ChunkIndexByPosition(player_->GetRenderData().translate.value);

	// 既存チャンクのみ更新
	for (int32_t dx = -updateRadius_; dx <= updateRadius_; ++dx)
	{
		for (int32_t dz = -updateRadius_; dz <= updateRadius_; ++dz)
		{
			Vector2int pos(playerIndex.x + dx, playerIndex.y + dz);
			Chunk* chunk = TryGetChunk(pos);
			if (chunk) { chunk->Update(); }
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

	// プレイヤーから周囲のみ描画（既存チャンクのみ）
	for (int32_t dx = -drawRadius_; dx <= drawRadius_; ++dx)
	{
		for (int32_t dz = -drawRadius_; dz <= drawRadius_; ++dz)
		{
			Vector2int chunkPos = Vector2int(playerIndex.x + dx, playerIndex.y + dz);
			EnsureChunkScheduled(chunkPos);
			Chunk* chunk = TryGetChunk(chunkPos);
			if (chunk) { chunk->Draw(); }
		}
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
	Chunk* chunk = TryGetChunk(chunkPos);
	if (chunk)
	{
		if (chunk->blocks[index.x][index.y][index.z])
		{
			return chunk->blocks[index.x][index.y][index.z]->isActive_;
		}
	}
	return false;
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
