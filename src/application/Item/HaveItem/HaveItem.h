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

	Hand hand_;

	// ドラッグ分配用
	bool isDragging_ = false;
	bool dragRightClick_ = false;
	std::vector<Vector2int> dragSlots_; // 右ドラッグで通過したスロット

	// 今カーソルが乗ってるインデックス
	Vector2int hoverIndex_ = { -1, -1 };

	void UpdateHoverIndex();
	void UpdateLeftClick();
	void UpdateRightClick();
	void UpdateDrag();
	void UpdateCounters();
	void SyncHotbar();

};

