#include "MapManager/MapManager.h"
#include "MapManager/Chunk/Chunk.h"
#include "MapManager/Chunk/Block/Block.h"
#include "MapManager/Chunk/Block/BlockDurability.h"
#include <fstream>
#include <sstream>
#include "Charactor/Player/Player.h"
#include "Utilities/PerlinNoise.h"
#include "Engine.h"
#include "Itemslot.h"


// AABBの各種補助 いずれstruct AABBに移す
namespace
{
	static float ClampFloat(float v, float a, float b) { return my_max(a, my_min(v, b)); }

	static float GetAABBSizeX(const AABB& a) { return a.max.x - a.min.x; }
	static float GetAABBSizeY(const AABB& a) { return a.max.y - a.min.y; }
	static float GetAABBSizeZ(const AABB& a) { return a.max.z - a.min.z; }

	// 進行方向に応じて、AABBの「面上のサンプル点」を3x3で作る（簡易）
	static void MakeFaceSamplePoints_X(const AABB& aabb, float xFace, Vector3 outPts[9])
	{
		const float inset = 0.02f;
		const float y0 = aabb.min.y + inset;
		const float y1 = (aabb.min.y + aabb.max.y) * 0.5f;
		const float y2 = aabb.max.y - inset;

		const float z0 = aabb.min.z + inset;
		const float z1 = (aabb.min.z + aabb.max.z) * 0.5f;
		const float z2 = aabb.max.z - inset;

		outPts[0] = { xFace, y0, z0 };
		outPts[1] = { xFace, y0, z1 };
		outPts[2] = { xFace, y0, z2 };
		outPts[3] = { xFace, y1, z0 };
		outPts[4] = { xFace, y1, z1 };
		outPts[5] = { xFace, y1, z2 };
		outPts[6] = { xFace, y2, z0 };
		outPts[7] = { xFace, y2, z1 };
		outPts[8] = { xFace, y2, z2 };
	}

	static void MakeFaceSamplePoints_Y(const AABB& aabb, float yFace, Vector3 outPts[9])
	{
		const float inset = 0.02f;
		const float x0 = aabb.min.x + inset;
		const float x1 = (aabb.min.x + aabb.max.x) * 0.5f;
		const float x2 = aabb.max.x - inset;

		const float z0 = aabb.min.z + inset;
		const float z1 = (aabb.min.z + aabb.max.z) * 0.5f;
		const float z2 = aabb.max.z - inset;

		outPts[0] = { x0, yFace, z0 };
		outPts[1] = { x1, yFace, z0 };
		outPts[2] = { x2, yFace, z0 };
		outPts[3] = { x0, yFace, z1 };
		outPts[4] = { x1, yFace, z1 };
		outPts[5] = { x2, yFace, z1 };
		outPts[6] = { x0, yFace, z2 };
		outPts[7] = { x1, yFace, z2 };
		outPts[8] = { x2, yFace, z2 };
	}

	static void MakeFaceSamplePoints_Z(const AABB& aabb, float zFace, Vector3 outPts[9])
	{
		const float inset = 0.02f;
		const float x0 = aabb.min.x + inset;
		const float x1 = (aabb.min.x + aabb.max.x) * 0.5f;
		const float x2 = aabb.max.x - inset;

		const float y0 = aabb.min.y + inset;
		const float y1 = (aabb.min.y + aabb.max.y) * 0.5f;
		const float y2 = aabb.max.y - inset;

		outPts[0] = { x0, y0, zFace };
		outPts[1] = { x1, y0, zFace };
		outPts[2] = { x2, y0, zFace };
		outPts[3] = { x0, y1, zFace };
		outPts[4] = { x1, y1, zFace };
		outPts[5] = { x2, y1, zFace };
		outPts[6] = { x0, y2, zFace };
		outPts[7] = { x1, y2, zFace };
		outPts[8] = { x2, y2, zFace };
	}
}

int LocalMod(int a, int n)
{
	return (a % n + n) % n;
}



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
	CreateNewMap(123456);
}

void MapManager::CreateNewMap(uint32_t seed)
{
	// ノイズパラメータ設定
	noiseParam_.seed = seed;			// 俗に言うシード値
	noiseParam_.scale = 32.0f;			// 地形の粗さ（大きくすると緩やか）
	noiseParam_.octaves = 4;			// 反復回数 (大きくすると細かい起伏が増える)
	noiseParam_.persistence = 0.5f;		// 各オクターブの振幅減衰 (大きくすると細かい起伏が増える)
	noiseParam_.height = CHUNK_Y;		// マップの高さ
	noiseParam_.pn = PerlinNoise(seed);	// PerlinNoise インスタンス生成

	//Vector2int playerIndex = ChunkIndexByPosition(player_->data_.translate.value);
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
	// 相互参照を切る
	for (auto& [pos, chunk] : chunks)
	{
		if (!chunk) continue;
		for (int dir = 0; dir < 4; ++dir)
		{
			chunk->SetNeighborChunk(static_cast<DirectionXZ>(dir), nullptr);
		}
	}
	// chunks 全解放&clear
	for (auto& pair : chunks)
	{
		delete pair.second;
		pair.second = nullptr;
	}
	chunks.clear();
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

	//// ロード後プレイヤーが乗ってるチャンクを生成
	//Vector2int playerIndex = ChunkIndexByPosition(player_->data_.translate.value);
	//if (!(chunkCreated_.find(playerIndex) != chunkCreated_.end()))
	//{
	//	EnsureChunkScheduled(playerIndex);
	//	ProcessChunkGeneration();
	//}

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
	Vector2int playerIndex = ChunkIndexByPosition(player_->data_.translate.value);
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

		Chunk* chunk = HasChunk(pos) ? TryGetChunk(pos) : new Chunk();
		chunk->CreateChunkData(noiseParam_, pos);
		chunks[pos] = chunk;

		// 隣接チャンク設定
		static constexpr Vector2int dirOffsets[4] = 
			{
			Vector2int(-1, 0),	// Left
			Vector2int(1, 0),	// Right
			Vector2int(0, -1),	// Back
			Vector2int(0, 1)	// Front
		};

		static constexpr DirectionXZ opposite[4] =
		{
			DirectionXZ::Right,	// Left  の反対
			DirectionXZ::Left,	// Right の反対
			DirectionXZ::Front,	// Back  の反対
			DirectionXZ::Back	// Front の反対
		};

		for (int dir = 0; dir < 4; ++dir)
		{
			Chunk* neighbor = TryGetChunk(pos + dirOffsets[dir]);

			// 自分→隣（隣が無ければnullptrでOK）
			chunk->SetNeighborChunk(static_cast<DirectionXZ>(dir), neighbor);

			// 隣→自分（隣がある時だけ）
			if (neighbor)
			{
				neighbor->SetNeighborChunk(opposite[dir], chunk);
			}
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
	Vector2int playerIndex = ChunkIndexByPosition(player_->data_.translate.value);

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

void MapManager::Draw()
{
	Vector2int playerIndex = ChunkIndexByPosition(player_->data_.translate.value);

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
	ImGui::Begin("MapManager");
	ImGui::Text("Chunks count: %zu", chunks.size());
	ImGui::Text("Chunk Generation Queue size: %zu", chunkGenQueue_.size());
	ImGui::End();
}

// 指定位置のブロックを破壊
void MapManager::DestroyBlockAt(const Vector2int& chunkPos, const Vector3int& localIndex)
{
	Chunk* chunk = TryGetChunk(chunkPos);
	if (!chunk) return;

	chunk->DestroyBlock(localIndex);
}

// 指定位置にブロックを設置
bool MapManager::SetBlockAt(const lookAtBlock& lab, const BlockID id)
{
	// Air ブロックならreturn
	if (id == BlockID::Air) return false;
	// 向きが不明ならreturn
	if (lab.face == AABBFace::NONE) return false;

	Vector3int offset = Vector3int(0, 0, 0);

	switch (lab.face)
	{
	case AABBFace::LEFT:
		offset.x = -1;
		break;
	case AABBFace::RIGHT:
		offset.x = 1;
		break;
	case AABBFace::BOTTOM:
		offset.y = -1;
		break;
	case AABBFace::TOP:
		offset.y = 1;
		break;
	case AABBFace::BACK:
		offset.z = -1;
		break;
	case AABBFace::FRONT:
		offset.z = 1;
		break;
	default:
		return false;
		break;
	}

	Vector2int chunkPos = lab.chunkIndex;
	Vector3int localIndex = lab.localIndex + offset;

	// チャンク跨ぎ対応
	if (localIndex.x < 0)
	{
		chunkPos.x -= 1;
		localIndex.x += CHUNK_X;
	}
	else if (localIndex.x >= CHUNK_X)
	{
		chunkPos.x += 1;
		localIndex.x -= CHUNK_X;
	}
	if (localIndex.z < 0)
	{
		chunkPos.y -= 1;
		localIndex.z += CHUNK_Z;
	}
	else if (localIndex.z >= CHUNK_Z)
	{
		chunkPos.y += 1;
		localIndex.z -= CHUNK_Z;
	}

	return SetBlockAt(chunkPos, localIndex, id);
}
bool MapManager::SetBlockAt(const Vector2int& chunkPos, const Vector3int& localIndex, const BlockID id)
{
	// Air ブロックは設置できない(おけるわけがない笑)
	if (id == BlockID::Air) return false;

	// 設置するチャンクが存在しないなら設置できない(非存在なわけがない笑)
	Chunk* chunk = TryGetChunk(chunkPos);
	if (!chunk) return false;

	// チャンク外のブロックを指してたら設置できない(指してるわけがない笑)
	if (localIndex.x < 0 || localIndex.x >= CHUNK_X ||
		localIndex.y < 0 || localIndex.y >= CHUNK_Y ||
		localIndex.z < 0 || localIndex.z >= CHUNK_Z)
	{
		return false;
	}

	Block* targetBlock = chunk->blocks[localIndex.x][localIndex.y][localIndex.z].get();
	// 指定位置にブロックインスタンスが存在しないなら設置できない(存在しないわけがない笑)
	if (!targetBlock) return false;
	// 指定位置に既にブロックが存在しているなら設置できない(存在しているわけがない笑)
	if (targetBlock->GetBlockID() != BlockID::Air) return false;

	// ブロック設置
	chunk->SetBlock(localIndex, id);

	// 露出状態更新
	chunk->SetExposedAroundBlocks(localIndex);

	return true;
}
bool MapManager::SetBlockAt(const Vector3& position, const BlockID id)
{
	return SetBlockAt(ChunkIndexByPosition(position), BlockIndexByPosition(position), id);
}

bool MapManager::SweepAABB(const AABB& aabb, const Vector3& delta, Vector3& outCorrectedDelta) const
{
	outCorrectedDelta = delta;

	// めり込み防止のスキン
	const float skin = 0.001f;

	// 与えられたAABBが重なり得るブロックを走査する
	auto AnySolidOverlap = [&](const AABB& test) -> bool
		{
			// AABBの覆うワールドブロック範囲（min/maxは「セルインデックス」）
			const Vector3int minBlockIndex = WorldBlockIndexByPosition(test.min);
			const Vector3int maxBlockIndex = WorldBlockIndexByPosition(test.max - Vector3{ 1e-6f, 1e-6f, 1e-6f });

			// floor_div（負数対応の床除算）
			auto FloorDiv = [](int a, int n) -> int
				{
					// n > 0 前提
					int q = a / n;
					int r = a % n;
					if (r != 0 && ((r > 0) != (n > 0)) != (a > 0)) {} // unused guard
					// C++の / は0方向丸めなので、負数で割り切れないときだけ1引く
					if (r != 0 && ((a < 0) ^ (n < 0))) --q;
					return q;
				};

			for (int by = minBlockIndex.y; by <= maxBlockIndex.y; ++by)
			{
				// Y軸はワールド境界でクリップ
				if (by < 0 || by >= CHUNK_Y) continue;
				const int localY = by;

				for (int bz = minBlockIndex.z; bz <= maxBlockIndex.z; ++bz)
				{
					const int chunkZ = FloorDiv(bz, CHUNK_Z);
					const int localZ = LocalMod(bz, CHUNK_Z);

					for (int bx = minBlockIndex.x; bx <= maxBlockIndex.x; ++bx)
					{
						const int chunkX = FloorDiv(bx, CHUNK_X);
						const int localX = LocalMod(bx, CHUNK_X);

						const Vector2int chunkPos{ chunkX, chunkZ };
						Chunk* c = TryGetChunk(chunkPos);
						if (!c) continue;

						Block* b = c->blocks[localX][localY][localZ].get();
						if (!b) continue;
						if (b->GetBlockID() == BlockID::Air) continue;

						// ブロックAABBと実際に重なっているか
						if (IsOverLap(test, b->aabb_))
						{
							return true;
						}
					}
				}
			}
			return false;
		};

	// 「指定軸だけ」動かして、めり込むならその軸の移動量を詰める
	auto ResolveAxis = [&](float& dAxis, int axis) -> bool
		{
			if (std::abs(dAxis) < 1e-6f) return false;

			const float sign = (dAxis >= 0.0f) ? 1.0f : -1.0f;
			const float dist = std::abs(dAxis);

			bool hit = false;

			// 二分探索で「当たらない最大距離」を探す（0..dist）
			float lo = 0.0f;
			float hi = dist;

			auto MakeMove = [&](float signedAmount) -> Vector3
				{
					Vector3 move{ 0,0,0 };
					if (axis == 0) move.x = signedAmount;
					if (axis == 1) move.y = signedAmount;
					if (axis == 2) move.z = signedAmount;
					return move;
				};

			// dist動かしても当たらないなら終了
			{
				const Vector3 move = MakeMove(sign * dist);
				Vector3 testMove = outCorrectedDelta + move;
				const AABB test = aabb + testMove;
				if (!AnySolidOverlap(test)) return false;
			}

			// 当たるので探索
			for (int i = 0; i < 16; ++i)
			{
				const float mid = (lo + hi) * 0.5f;
				const Vector3 move = MakeMove(sign * mid);
				Vector3 testMove = outCorrectedDelta + move;
				const AABB test = aabb + testMove;

				if (AnySolidOverlap(test))
				{
					hi = mid;
					hit = true;
				}
				else
				{
					lo = mid;
				}
			}

			// skin分だけ手前で止める（距離側で引く）
			float allowedDist = lo - skin;
			if (allowedDist < 0.0f) allowedDist = 0.0f;

			dAxis = sign * allowedDist;
			return hit;
		};

	bool hitAny = false;

	// X
	{
		float dx = outCorrectedDelta.x;
		if (ResolveAxis(dx, 0)) hitAny = true;
		outCorrectedDelta.x = dx;
	}

	// Z
	{
		float dz = outCorrectedDelta.z;
		if (ResolveAxis(dz, 2)) hitAny = true;
		outCorrectedDelta.z = dz;
	}

	// Y
	{
		float dy = outCorrectedDelta.y;
		if (ResolveAxis(dy, 1)) hitAny = true;
		outCorrectedDelta.y = dy;
	}


	return hitAny;
}
bool MapManager::isSolidAt(const Vector3& position) const 
{
	Vector2int chunkPos = ChunkIndexByPosition(position);
	Vector3int index = BlockIndexByPosition(position);
	Chunk* chunk = TryGetChunk(chunkPos);
	if (chunk)
	{
		Block* block = chunk->blocks[index.x][index.y][index.z].get();
		if (block && block->GetBlockID() != BlockID::Air)
		{
			return true;
		}
	}
	return false;
}

AABB MapManager::GetAABB(const Vector2int& chunkPos, const Vector3int& index) const
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
AABB MapManager::GetAABB(const Vector3& position) const
{
	Vector2int chunkPos = ChunkIndexByPosition(position);
	Vector3int index = BlockIndexByPosition(position);
	return GetAABB(chunkPos, index);
}
bool MapManager::GetIsActive(const Vector2int& chunkPos, const Vector3int& index) const
{
	Chunk* chunk = TryGetChunk(chunkPos);
	if (chunk)
	{
		if (chunk->blocks[index.x][index.y][index.z]->blockID != BlockID::Air)
		{
			return true;
		}
	}
	return false;
}
bool MapManager::GetIsActive(const Vector3& position) const
{
	Vector2int chunkPos = ChunkIndexByPosition(position);
	Vector3int index = BlockIndexByPosition(position);
	return GetIsActive(chunkPos, index);
}

// position が今どのチャンクに属しているか  例：position=(34, 0, 50) -> chunkIndex=(1, 2)
Vector2int MapManager::ChunkIndexByPosition(const Vector3& position) const
{
	int bx = static_cast<int>(std::floor(position.x / BLOCK_SIZE));
	int bz = static_cast<int>(std::floor(position.z / BLOCK_SIZE));

	Vector2int chunk;
	chunk.x = static_cast<int>(std::floor((float)bx / CHUNK_X));
	chunk.y = static_cast<int>(std::floor((float)bz / CHUNK_Z));
	return chunk;
}

// position が今チャンク内どのブロックに属しているか(どんな時も0～CHUNK_SIZE-1の範囲に収まる)  例：position=(34, 0, 50) -> localIndex=(2, 0, 2)
Vector3int MapManager::BlockIndexByPosition(const Vector3& position) const
{
	// ① ワールド座標 → 世界ブロック座標
	Vector3int wb = WorldBlockIndexByPosition(position);

	// ② チャンク内インデックスへ正規化（数学的 mod）
	Vector3int local;
	local.x = LocalMod(wb.x, CHUNK_X);
	local.y = LocalMod(wb.y, CHUNK_Y);
	local.z = LocalMod(wb.z, CHUNK_Z);

	local.y = std::clamp(local.y, 0, CHUNK_Y - 1);

	return local;
}

Vector3int MapManager::WorldBlockIndexByPosition(const Vector3& position) const
{
	int bx = static_cast<int>(std::floor(position.x / BLOCK_SIZE));
	int by = static_cast<int>(std::floor(position.y / BLOCK_SIZE));
	int bz = static_cast<int>(std::floor(position.z / BLOCK_SIZE));
	return Vector3int(bx, by, bz);
}


// レイとブロックの交差判定（衝突ブロックを返す）
std::optional<lookAtBlock> MapManager::GetBlockByCrossedRay(const Ray& ray, const float maxDistance) const
{
	lookAtBlock result;

	const Vector3 rayStart = ray.origin;
	const Vector3 dir = ray.diff;
	const float rayLen = maxDistance;
	const Vector3 rayEnd = ray.origin + ray.diff * rayLen;

	auto WorldBlockIndexByPosition = [](const Vector3& p) -> Vector3int
		{
			return Vector3int(
				int(std::floor(p.x / BLOCK_SIZE)),
				int(std::floor(p.y / BLOCK_SIZE)),
				int(std::floor(p.z / BLOCK_SIZE))
			);
		};

	auto WorldChunkIndexFromWorldBlock = [](const Vector3int& wb) -> Vector2int
		{
			Vector2int cp;
			cp.x = int(std::floor(float(wb.x) / float(CHUNK_X)));
			cp.y = int(std::floor(float(wb.z) / float(CHUNK_Z)));
			return cp;
		};

	auto Sign = [](float v) -> int { return (v > 0.0f) - (v < 0.0f); };

	auto LocalMod = [](int a, int n) -> int { return (a % n + n) % n; };

	auto LocalIndexFromWorldBlock = [&](const Vector3int& wb) -> Vector3int
		{
			return Vector3int(
				LocalMod(wb.x, CHUNK_X),
				LocalMod(wb.y, CHUNK_Y),
				LocalMod(wb.z, CHUNK_Z)
			);
		};

	// sideDist（次の境界までの距離 t）
	auto NextBoundaryT = [](float origin, float dir, int cell, int step) -> float
		{
			// cell = floor(origin/BLOCK_SIZE) のブロック座標
			if (step > 0)
			{
				const float next = (float(cell) + 1.0f) * BLOCK_SIZE;
				return (next - origin) / dir;
			}
			else
			{
				const float next = float(cell) * BLOCK_SIZE;
				return (next - origin) / dir;
			}
		};

	// 現在のワールドブロックインデックス
	Vector3int currentWB = WorldBlockIndexByPosition(rayStart);
	const Vector3int endWB = WorldBlockIndexByPosition(rayEnd);

	// step（dir==0 の軸は 0）
	const int stepX = Sign(dir.x);
	const int stepY = Sign(dir.y);
	const int stepZ = Sign(dir.z);

	// deltaDist（dir==0 は INF 扱い）
	const float INF = std::numeric_limits<float>::infinity();
	const float deltaDistX = (std::abs(dir.x) < 1e-6f) ? INF : std::abs(BLOCK_SIZE / dir.x);
	const float deltaDistY = (std::abs(dir.y) < 1e-6f) ? INF : std::abs(BLOCK_SIZE / dir.y);
	const float deltaDistZ = (std::abs(dir.z) < 1e-6f) ? INF : std::abs(BLOCK_SIZE / dir.z);

	float sideDistX = (stepX == 0) ? INF : NextBoundaryT(rayStart.x, dir.x, currentWB.x, stepX);
	float sideDistY = (stepY == 0) ? INF : NextBoundaryT(rayStart.y, dir.y, currentWB.y, stepY);
	float sideDistZ = (stepZ == 0) ? INF : NextBoundaryT(rayStart.z, dir.z, currentWB.z, stepZ);

	// 最大ステップ
	const int maxSteps = 2048;

	// 直前に跨いだ面（＝ currentWB に入った面）
	AABBFace enterFace = AABBFace::NONE;


	for (int i = 0; i < maxSteps; ++i)
	{
		// currentWB -> chunk/local
		const Vector2int chunkPos = WorldChunkIndexFromWorldBlock(currentWB);
		const Vector3int local = LocalIndexFromWorldBlock(currentWB);

		Chunk* chunk = TryGetChunk(chunkPos);
		if (chunk)
		{
			// local.y は縦制限があるので範囲チェック
			if (0 <= local.x && local.x < CHUNK_X &&
				0 <= local.y && local.y < CHUNK_Y &&
				0 <= local.z && local.z < CHUNK_Z)
			{
				Block* b = chunk->blocks[local.x][local.y][local.z].get();
				if (b && b->GetBlockID() != BlockID::Air)
				{
					const AABB& aabb = b->aabb_;

					result.block = b;
					result.chunkIndex = chunkPos;
					result.localIndex = local;
					result.face = enterFace;

					// 「ブロック中心まで」ではなく「侵入面まで」の距離を返す
					// 今回のSweepは軸レイ(±1,0,0等)なのでこの計算でOK
					float dist = 0.0f;

					switch (enterFace)
					{
					case AABBFace::LEFT:   dist = std::abs(aabb.min.x - rayStart.x); break;
					case AABBFace::RIGHT:  dist = std::abs(aabb.max.x - rayStart.x); break;
					case AABBFace::BOTTOM: dist = std::abs(aabb.min.y - rayStart.y); break;
					case AABBFace::TOP:    dist = std::abs(aabb.max.y - rayStart.y); break;
					case AABBFace::BACK:   dist = std::abs(aabb.min.z - rayStart.z); break;
					case AABBFace::FRONT:  dist = std::abs(aabb.max.z - rayStart.z); break;
					default:
						// startが既にブロック内などのケース
						dist = 0.0f;
						break;
					}

					result.distance = dist;
					return result;
				}
			}
		}

		// 終点ブロックまで到達したら終了
		if (currentWB == endWB) return std::nullopt;

		// 次に跨ぐ境界（最小のsideDist）を選ぶ
		if (sideDistX < sideDistY)
		{
			if (sideDistX < sideDistZ)
			{
				// X方向へ進む
				currentWB.x += stepX;

				// X方向に進む場合、入ってきた面は stepX の反対側
				// stepX=+1 なら “LEFT面から入る”、stepX=-1 なら “RIGHT面から入る”
				enterFace = (stepX > 0) ? AABBFace::LEFT : AABBFace::RIGHT;

				sideDistX += deltaDistX;
			}
			else
			{
				// Z方向へ進む
				currentWB.z += stepZ;

				// stepZ=+1 なら “BACK面から入る”、stepZ=-1 なら “FRONT面から入る”
				enterFace = (stepZ > 0) ? AABBFace::BACK : AABBFace::FRONT;

				sideDistZ += deltaDistZ;
			}
		}
		else
		{
			if (sideDistY < sideDistZ)
			{
				// Y方向へ進む
				currentWB.y += stepY;

				// stepY=+1 なら “BOTTOM面から入る”、stepY=-1 なら “TOP面から入る”
				enterFace = (stepY > 0) ? AABBFace::BOTTOM : AABBFace::TOP;

				sideDistY += deltaDistY;
			}
			else
			{
				// Z方向へ進む
				currentWB.z += stepZ;

				enterFace = (stepZ > 0) ? AABBFace::BACK : AABBFace::FRONT;

				sideDistZ += deltaDistZ;
			}
		}

		// 伸びすぎ防止：レイ長を超えたら終了（tの近似として最小sideDistを使う）
		const float tApprox = my_min(sideDistX, my_min(sideDistY, sideDistZ));
		if (tApprox > rayLen) return std::nullopt;
	}

	return std::nullopt;
}

// レイとブロックの交差判定（衝突座標を返す）
std::optional<Vector3> MapManager::GetPositionByCrossedRay(const Ray& ray) const
{
	AABB aabb = GetAABB(ray.origin);

	// まずAABBで大まかに判定
	if (!IsCollision(ray, aabb))
	{
		return std::nullopt;
	}

	// AABBに当たっていた場合のみ、三角形ごとに詳細判定 最近衝突点を返す
	std::optional<Vector3> closestPoint = std::nullopt;
	float closestDist = std::numeric_limits<float>::infinity();

	// AABBの各面を構成する三角形を取得
	Triangle triangles[12];
	triangles[0] = Triangle{
		Vector3{aabb.min.x, aabb.min.y, aabb.min.z},
		Vector3{aabb.max.x, aabb.min.y, aabb.min.z},
		Vector3{aabb.max.x, aabb.max.y, aabb.min.z}};
	triangles[1] = Triangle{
		Vector3{aabb.min.x, aabb.min.y, aabb.min.z},
		Vector3{aabb.max.x, aabb.max.y, aabb.min.z},
		Vector3{aabb.min.x, aabb.max.y, aabb.min.z}};
	triangles[2] = Triangle{
		Vector3{aabb.min.x, aabb.min.y, aabb.max.z},
		Vector3{aabb.max.x, aabb.max.y, aabb.max.z},
		Vector3{aabb.max.x, aabb.min.y, aabb.max.z}};
	triangles[3] = Triangle{
		Vector3{aabb.min.x, aabb.min.y, aabb.max.z},
		Vector3{aabb.min.x, aabb.max.y, aabb.max.z},
		Vector3{aabb.max.x, aabb.max.y, aabb.max.z}};
	triangles[4] = Triangle{
		Vector3{aabb.min.x, aabb.min.y, aabb.min.z},
		Vector3{aabb.min.x, aabb.min.y, aabb.max.z},
		Vector3{aabb.min.x, aabb.max.y, aabb.max.z}};
	triangles[5] = Triangle{
		Vector3{aabb.min.x, aabb.min.y, aabb.min.z},
		Vector3{aabb.min.x, aabb.max.y, aabb.max.z},
		Vector3{aabb.min.x, aabb.max.y, aabb.min.z}};
	triangles[6] = Triangle{
		Vector3{aabb.max.x, aabb.min.y, aabb.min.z},
		Vector3{aabb.max.x, aabb.max.y, aabb.min.z},
		Vector3{aabb.max.x, aabb.max.y, aabb.max.z}};
	triangles[7] = Triangle{
		Vector3{aabb.max.x, aabb.min.y, aabb.min.z},
		Vector3{aabb.max.x, aabb.max.y, aabb.max.z},
		Vector3{aabb.max.x, aabb.min.y, aabb.max.z}};
	triangles[8] = Triangle{
		Vector3{aabb.min.x, aabb.min.y, aabb.min.z},
		Vector3{aabb.max.x, aabb.min.y, aabb.min.z},
		Vector3{aabb.max.x, aabb.min.y, aabb.max.z}};
	triangles[9] = Triangle{
		Vector3{aabb.min.x, aabb.min.y, aabb.min.z},
		Vector3{aabb.max.x, aabb.min.y, aabb.max.z},
		Vector3{aabb.min.x, aabb.min.y, aabb.max.z}};
	triangles[10] = Triangle{
		Vector3{aabb.min.x, aabb.max.y, aabb.min.z},
		Vector3{aabb.max.x, aabb.max.y, aabb.max.z},
		Vector3{aabb.max.x, aabb.max.y, aabb.min.z}};
	triangles[11] = Triangle{
		Vector3{aabb.min.x, aabb.max.y, aabb.min.z},
		Vector3{aabb.min.x, aabb.max.y, aabb.max.z},
		Vector3{aabb.max.x, aabb.max.y, aabb.max.z}};
	for (const Triangle& t : triangles)
	{
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
