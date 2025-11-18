#pragma once
#include <string>
#include "Game.h"

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
	void UpdatePlayerCollisionY();
	void UpdatePlayerCollisionXZ();
	void Draw();

	Vector3int IndexByPosition(const Vector3& position);

private:
	// マップデータ
	Block* block_[MAX_BLOCK_X][MAX_BLOCK_Y][MAX_BLOCK_Z];
	int blockHeightMap_[MAX_BLOCK_X][MAX_BLOCK_Z];

	// ブロック破壊テクスチャ
	bool isBeingDestroyed_ = false; // 破壊中かいなか
	RenderData_Rect blockRect_[6];  // 6面分
	Transforms blockTriangleTransform_;

	// プレイヤー参照
	Player* player_;
};

