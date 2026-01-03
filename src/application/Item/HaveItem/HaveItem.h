#pragma once
#include "definition/definition.h"
#include <array>
#include "Item/Item.h"
#include "Game.h"

class HaveItem
{
public:
	HaveItem();
	~HaveItem();
	// アイテム獲得
	void AddItem(ItemID id);

	void UpdateInventry();
	void UpdateHotbar();

	void DrawInventory();
	void DrawHotbar();

private:
	std::array<std::array<InventorySlot, 9>, 4> inventory_;
	std::array<std::array<Vector3, 9>, 4> baseInventoryPositions_;
	std::array<InventorySlot, 9> hotbar_;


	// 今掴んでるインデックス
	Vector2int grabbedIndex_ = { -1, -1 };
	bool grabbed_ = false;

	// 今カーソルが乗ってるインデックス
	Vector2int hoverIndex_ = { -1, -1 };
};

