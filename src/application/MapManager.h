#pragma once
#include <string>
#include "Game.h"
#include "DropItem.h"

class Block;
class Player;
class BlockConfig;

class MapManager
{
public:
	MapManager(Player* player);
	~MapManager();

	void LoadMap(const std::string& mapFilePath);
	void SaveMap(const std::string& mapFilePath);

	// 表面にでているブロックの座標をblockData_の対応するデータに変換して格納
	void SetExposedBlocks();;

	void Initialize();
	void Update();
	void UpDataPlayerRayCollision();
	void UpdatePlayerCollisionY();
	void UpdatePlayerCollisionX();
	void UpdatePlayerCollisionZ();

	void Draw();

	Vector3int IndexByPosition(const Vector3& position);
	Vector3 PositionByIndex(const Vector3int& index);
	AABB AABBByIndex(const Vector3int& index);

private:
	// マップデータ
	Block* block_[MAX_BLOCK_X][MAX_BLOCK_Y][MAX_BLOCK_Z];
	int blockHeightMap_[MAX_BLOCK_X][MAX_BLOCK_Z];
	// 描画データ
	std::map<BlockID, std::unique_ptr<RenderData_Block>> blockData_;

	BlockConfig* blockConfig_;

	std::map<BlockID, uint32_t> blockDrawSumMap_;




	// 着地パーティクル
	RenderData_Particle* landingParticle_;

	// プレイヤー参照
	Player* player_;

	// ドロップアイテム管理
	std::vector<DropItem*> dropItems_;


	std::optional<Vector3> IntersectRayBlock(const Ray& ray, const std::vector<VertexData>& vertices, const AABB& aabb, const Matrix4x4 worldMatrix);
};

