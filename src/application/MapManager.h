#pragma once
#include <string>
#include "Game.h"
#include "DropItem.h"

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
	void CreateNewMap();
	Chunk* CreateChunk(const Vector2int& chunkPos);
	Chunk* GetOrCreateChunk(const Vector2int& chunkPos);

	void Initialize();
	void Update();
	void UpDataPlayerRayCollision();

	void Draw();
	void DrawImGui();

	AABB GetAABB(const Vector2int& chunkPos, const Vector3int& index);
	AABB GetAABB(const Vector3& position);
	bool GetIsActive(const Vector2int& chunkPos, const Vector3int& index);
	bool GetIsActive(const Vector3& position);
	Vector2int ChunkIndexByPosition(const Vector3& position);		// ワールド座標				→	chunksのキーインデックス座標
	Vector3int BlockIndexByPosition(const Vector3& position);		// ワールド座標				→	ブロックインデックス座標

private:

	std::string mapFilePath_;

	// マップデータ
	std::unordered_map<Vector2int, Chunk*, Vector2intHash> chunks;

	BlockConfig* blockConfig_;


	// 着地パーティクル
	RenderData_Particle* landingParticle_;

	// プレイヤー参照
	Player* player_;

	// ドロップアイテム管理
	std::vector<DropItem*> dropItems_;


	std::optional<Vector3> IntersectRayBlock(const Ray& ray, const std::vector<VertexData>& vertices, const AABB& aabb);
};

