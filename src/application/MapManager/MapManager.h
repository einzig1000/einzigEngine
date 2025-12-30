#pragma once
#include <string>
#include "Game.h"
#include "MapManager/DropItem/DropItem.h"
#include <queue>
#include <unordered_set>
#include "Utilities/PerlinNoise.h"
#include "engine/Physics/IWorldCollider.h"

class Block;
class Chunk;
class Player;
class BlockConfig;

class MapManager
{
public:
	MapManager(Player* player);
	~MapManager();

	void LoadMap(const std::string& mapFilePath);
	void SaveMap(const std::string& mapFilePath);
	void CreateNewMap(uint32_t seed);

	void Initialize();
	void Update();
	void Draw();
	void DrawImGui();

	void SetDrawRadius(int r) { drawRadius_ = r; }
	void SetUpdateRadius(int r) { updateRadius_ = r; }

	// ブロック破壊
	void DestroyBlockAt(const Vector2int& chunkPos, const Vector3int& localIndex);
	// ブロック設置
	bool SetBlockAt(const lookAtBlock& lab, const BlockID id);
	bool SetBlockAt(const Vector2int& chunkPos, const Vector3int& localIndex, const BlockID id);
	bool SetBlockAt(const Vector3& position, const BlockID id);

	// SweepAABB
	bool SweepAABB(const AABB& aabb, const Vector3& delta, Vector3& outCorrectedDelta) const;
	// 指定位置に固体ブロックがあるか
	bool isSolidAt(const Vector3& position) const;

	// レイとブロックの交差判定（衝突ブロックを返す）
	std::optional<lookAtBlock> GetBlockByCrossedRay(const Ray& ray, const float maxDistance) const;
	// レイとブロックの交差判定（衝突座標を返す）
	std::optional<Vector3> GetPositionByCrossedRay(const Ray& ray) const;

	AABB GetAABB(const Vector2int& chunkPos, const Vector3int& index) const;
	AABB GetAABB(const Vector3& position) const;
	bool GetIsActive(const Vector2int& chunkPos, const Vector3int& index) const;
	bool GetIsActive(const Vector3& position) const;
	Vector2int ChunkIndexByPosition(const Vector3& position) const;		// ワールド座標 → chunksのキーインデックス座標
	Vector3int BlockIndexByPosition(const Vector3& position) const;		// ワールド座標 → ブロックインデックス座標
	// ワールド座標からワールドブロックインデックス
	Vector3int WorldBlockIndexByPosition(const Vector3& position) const;

	// マップデータ
	std::unordered_map<Vector2int, Chunk*, Vector2intHash> chunks;
	
	// チャンク有無確認
	bool HasChunk(const Vector2int& chunkPos) const;
	// チャンク取得、なくても生成はしない
	Chunk* TryGetChunk(const Vector2int& chunkPos) const;
	// チャンク取得、なければスケジュールに登録して生成
	Chunk* GetOrCreateChunk(const Vector2int& chunkPos);
	// 欲しいチャンクが存在しなければスケジュールに登録
	void EnsureChunkScheduled(const Vector2int& chunkPos);
	// スケジュールに登録されたチャンクを1Fに1つ生成
	void ProcessChunkGeneration();


private:

	// マップファイルパス
	std::string mapFilePath_;

	// プレイヤー参照
	Player* player_;

	// ドロップアイテム管理
	std::vector<DropItem*> dropItems_;

	// このキューに入っているチャンクを順次生成していく
	std::queue<Vector2int> chunkGenQueue_;		
	// スケジュール済みチャンク集合
	std::unordered_set<Vector2int, Vector2intHash> chunkScheduled_;
	// 既に作成されたチャンク集合
	std::unordered_set<Vector2int, Vector2intHash> chunkCreated_;

	// パラメータ
	int drawRadius_ = 6;    // 描画半径（チャンク単位）
	int updateRadius_ = 2;   // 更新半径（チャンク単位）
	NoiseParameter noiseParam_;

};

