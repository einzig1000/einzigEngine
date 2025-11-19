#pragma once
#include "Game.h"

struct slotData
{
	uint32_t texture = 0;
	int hasItemCount = 0;
};

class Itemslot
{
public:
	Itemslot();
	void Update();
	void Draw();
	void AddItemToItemslot(int itemID);

	int getSelectedItemID() const
	{
		if (hasItem_[selectedSlot_].hasItemCount > 0)
		{
			return hasItem_[selectedSlot_].texture;
		}
		return -1;
	}
	void useSelectedItem()
	{
		if (hasItem_[selectedSlot_].hasItemCount > 0)
		{
			hasItem_[selectedSlot_].hasItemCount--;
		}
	}

	// アイテムスロットのスプライト配列
	RenderData_Sprite slotSprite_[10];
	// スロット内のアイテムスプライト配列
	RenderData_Sprite itemSprite_[40];
	// 選択中スロット番号
	int selectedSlot_ = 0;

	// テクスチャ & 何個もってるか
	slotData hasItem_[40];

	// アイテム一覧か画面下のみ表示か
	bool isShowAllItems_ = false;
};

