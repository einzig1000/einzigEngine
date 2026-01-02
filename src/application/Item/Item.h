#pragma once
#include "Game.h"


struct InventorySlot
{
	Item item;     // アイテムそのもの
	uint8_t count; // 所持数（0〜64）
	Vector2int position; // スロット位置
};


class Item
{
public:
	Item(ItemID id);
	ItemID GetID() const { return id_; }
	int32_t GetTextureHandleForInventory() const { return textureHandleForInventory_; }
	int32_t GetTextureHandleForModel() const { return textureHandleForModel_; }
	int32_t GetModelHandle() const { return modelHandle_; }
	int32_t GetAbleStackCount() const { return 64; } // 積載可能数（仮固定）

private:
	ItemID id_;

	// インベントリに表示する時のテクスチャ
	int32_t textureHandleForInventory_;
	// モデルとして描画する時のテクスチャ
	int32_t textureHandleForModel_;
	// モデルデータ
	int32_t modelHandle_;

};

