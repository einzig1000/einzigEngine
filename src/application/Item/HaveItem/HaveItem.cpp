#include "Item/HaveItem/HaveItem.h"

HaveItem::HaveItem()
{
	for (auto& row : inventory_)
	{
		for (auto& slot : row)
		{
			//slot.count = 0;
		}
	}
}

HaveItem::~HaveItem()
{}

// アイテム獲得
void HaveItem::AddItem(ItemID id)
{
	for (auto& row : inventory_)
	{
		for (auto& slot : row)
		{
			// if (既にinventory_に登録されているアイテム && 
			// そのスロットのcountがitem->GetAbleStackCount()未満)　そのスロットに追加
			if (slot.item.GetID() == id && slot.count < slot.item.GetAbleStackCount())
			{
				// 既に所持しているアイテムのスロットに追加
				slot.count++;
				return;
			}
		}
	}

	// if (まだもっていない || item->GetAbleStackCount()以下のスロットがない)　新しいスロットに追加
	for (auto& row : inventory_)
	{
		for (auto& slot : row)
		{
			// 空スロットを探す
			if (slot.count == 0)
			{
				// 新しいスロットに追加
				slot.item = Item(id);
				slot.count = 1;
				return;
			}
		}
	}
}

// 所持アイテム数取得
int HaveItem::GetHaveItemCount(ItemID id) const
{
}