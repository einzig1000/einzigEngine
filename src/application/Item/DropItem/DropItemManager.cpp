#include "DropItemManager.h"
#include "Item/DropItem/DropItem.h"

DropItemManager::DropItemManager(){}

void DropItemManager::AddItem(ItemID id, Vector3 pos)
{
	//items_.emplace_back(new DropItem(player_));
	//items_.back()->SetItem(id, pos);

	DropItem* newItem = new DropItem();
	newItem->SetMapManager(mapManager_);
	newItem->SetPlayer(player_);
	newItem->SetItem(id, pos);
	items_.emplace_back(newItem);
}

void DropItemManager::Update()
{
	for (auto& item : items_)
	{
		item->Update();
	}
}

void DropItemManager::Draw()
{
	for (auto& item : items_)
	{
		item->Draw();
	}
}

