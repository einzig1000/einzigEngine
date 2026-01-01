#pragma once
#include "definition/definition.h"
#include <vector>
#include <unordered_map>


enum class ModelID
{
	None,
	Cube,
	Plane,
	Corn,
	Sphere,

	MAX,
};

enum class TextureID
{
	None,
	UVChecker,
	monsterBall,
	white1x1,
	empty1x1,
	Circle,

	TITLE_start,
	TITLE_option,

	Item_slot,

	BreakBlock_Array,

	MAX,
};

enum class UITextureID
{
	None,

	Inventory,


	MAX,
};

class ResourceID
{
public:

	static void reload();

	// その他
	static int32_t GetTextureID(TextureID id)
	{
		return TextureIDs_[static_cast<size_t>(id)];
	}
	static int32_t GetModelID(ModelID id)
	{
		return ModelIDs_[static_cast<size_t>(id)];
	}

	// ブロック関連
	static int32_t GetTextureID(BlockID id)
	{
		return BlockTextureIDs_[static_cast<size_t>(id)];
	}
	static int32_t GetModelID(BlockID id)
	{
		return BlockModelIDs_[static_cast<size_t>(id)];
	}

	// UIテクスチャID取得
	static int32_t GetTextureID(UITextureID id)
	{
		return UITextureIDs_[static_cast<size_t>(id)];
	}

	// アイテム関連
	static int32_t Get3DTextureID(ItemID id)
	{
		return ItemModelTextureIDs_[static_cast<size_t>(id)];
	}
	static int32_t Get2DTextureID(ItemID id)
	{
		return ItemIconTextureIDs_[static_cast<size_t>(id)];
	}
	static int32_t GetModelID(ItemID id)
	{
		return ItemModelIDs_[static_cast<size_t>(id)];
	}


private:

	// 全てのアイテムのテクスチャID配列（インベントリとかに表示するアイコン）
	static std::vector<int> ItemIconTextureIDs_;
	// 全てのアイテムのテクスチャID配列（モデルとして描画する用）
	static std::vector<int> ItemModelTextureIDs_;
	// 全てのアイテムのモデルID配列
	static std::vector<int> ItemModelIDs_;

	// ブロックテクスチャのみの配列
	static std::vector<int> BlockTextureIDs_;
	// ブロックモデルのみの配列
	static std::vector<int> BlockModelIDs_;

	// UIテクスチャID配列
	static std::vector<int> UITextureIDs_;
	// その他のテクスチャID配列
	static std::vector<int> TextureIDs_;
	// その他のモデルID配列
	static std::vector<int> ModelIDs_;


	static inline const std::unordered_map<TextureID, std::string> textureFilePaths_ =
	{
	{ TextureID::UVChecker,    "resources/Prototypes/texture/uvChecker.png" },
	{ TextureID::monsterBall,  "resources/Prototypes/texture/monsterBall.png" },
	{ TextureID::white1x1,     "resources/Prototypes/texture/white1x1.png" },
	{ TextureID::empty1x1,     "resources/Prototypes/texture/empty1x1.png" },
	{ TextureID::Circle,       "resources/Prototypes/texture/circle.png" },
	{ TextureID::TITLE_start,  "resources/Minecraft/Title/start.png" },
	{ TextureID::TITLE_option, "resources/Minecraft/Title/option.png" },
	{ TextureID::Item_slot,    "resources/Minecraft/Item_slot.png" },
	};

	ResourceID() = delete;
	~ResourceID() = delete;

};