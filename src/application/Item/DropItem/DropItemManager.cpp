#include "DropItemManager.h"
#include "Item/DropItem/DropItem.h"

DropItemManager::DropItemManager(Player* player)
{}

void DropItemManager::AddItem(ItemID id, Vector3 pos)
{
	items_.emplace_back(new DropItem(player_));
	items_.back()->SetItem(id, pos);
}

void DropItemManager::Update()
{
}

void DropItemManager::Draw()
{
}

