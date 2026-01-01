#pragma once
#include "Game.h"

class Item
{
public:
	Item(ItemID id);
	ItemID GetID() const { return id_; }
	int32_t GetTextureHandleForInventory() const { return textureHandleForInventory_; }
	int32_t GetTextureHandleForModel() const { return textureHandleForModel_; }
	int32_t GetModelHandle() const { return modelHandle_; }

private:
	ItemID id_;

	// インベントリに表示する時のテクスチャ
	int32_t textureHandleForInventory_;
	// モデルとして描画する時のテクスチャ
	int32_t textureHandleForModel_;
	// モデルデータ
	int32_t modelHandle_;

};

