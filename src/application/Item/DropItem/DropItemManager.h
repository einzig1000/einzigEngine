#pragma once
#include "definition/definition.h"

class Player;
class DropItem;
class MapManager;

class DropItemManager
{
public:
	DropItemManager();
	void SetMapManager(MapManager* mapManager) { mapManager_ = mapManager; }
	void SetPlayer(Player* player) { player_ = player; }
	void AddItem(ItemID id, Vector3 pos);
	void Update();
	void Draw();

	// ドロップアイテム配列
	std::vector<DropItem*> items_;


private:

	Player* player_;

	MapManager* mapManager_;

};
