#pragma once
#include "definition/definition.h"
#include <array>
#include "Item/Item.h"


class HaveItem
{
public:
	HaveItem();
	~HaveItem();
	// アイテム獲得
	void AddItem(ItemID id);
	// 所持アイテム数取得
	int GetHaveItemCount(ItemID id) const;

private:
	// 9x4のアイテム所持配列　1つのアイテムにつき最大64個まで所持可能
	std::array<std::array<InventorySlot, 9>, 4> inventory_;

	Vector2int offset_ = { 0,0 }; // インベントリ内ポジションオフセット
};

