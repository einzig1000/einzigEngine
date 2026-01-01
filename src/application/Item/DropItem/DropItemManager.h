#pragma once
#include "definition/definition.h"

class Player;
class DropItem;

class DropItemManager
{
public:
	DropItemManager(Player* player);
	void AddItem(ItemID id, Vector3 pos);
	void Update();
	void Draw();

	// ドロップアイテム配列
	std::vector<DropItem*> items_;

	Player* player_;
};
