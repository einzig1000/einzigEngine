#include "Item.h"

void Item::Initialize(ItemID id)
{
	id_ = id;
	// インベントリに表示する時のテクスチャ
	textureHandleForInventory_ = ResourceID::Get2DTextureID(id);
	// モデルとして描画する時のテクスチャ
	textureHandleForModel_ = ResourceID::Get3DTextureID(id);
	// モデルデータ
	modelHandle_ = ResourceID::GetModelID(id);
	// 積載可能数（仮固定）
	ableStackCount_ = 64;
}