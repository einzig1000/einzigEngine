#pragma once
#include "Game.h"

class Itemslot
{
public:
	Itemslot();
	void Update();
	void Draw();
	void AddItemToItemslot(int itemID);


	// アイテムスロットのスプライト配列
	RenderData_Sprite slotSprite_[10];
	// スロット内のアイテムスプライト配列
	RenderData_Sprite itemSprite_[10];
	// 選択中スロット番号
	int selectedSlot_ = 0;

};

