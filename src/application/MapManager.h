#pragma once
#include <string>
#include "Game.h"
#include "DropItem.h"

class Block;
class Player;

class MapManager
{
public:
	MapManager(Player* player);
	~MapManager();

	void LoadMap(const std::string& mapFilePath);
	void Initialize();
	void Update();
	void UpDataPlayerRayCollision();
	void UpdatePlayerCollisionY();
	void UpdatePlayerCollisionX();
	void UpdatePlayerCollisionZ();

	void Draw();

	Vector3int IndexByPosition(const Vector3& position);

private:
	// マップデータ
	Block* block_[MAX_BLOCK_X][MAX_BLOCK_Y][MAX_BLOCK_Z];
	int blockHeightMap_[MAX_BLOCK_X][MAX_BLOCK_Z];


	// 着地パーティクル
	RenderData_Particle3* landingParticle_;

	// プレイヤー参照
	Player* player_;

	// ドロップアイテム管理
	std::vector<DropItem*> dropItems_;

	std::map<BlockID, Blockinfo> blockInfoMap_;
};

