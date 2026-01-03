#include "ResourceLoder/ResourceID.h"
#include "Game.h"

// 全てのアイテムのテクスチャID配列（インベントリとかに表示するアイコン）
std::vector<int> ResourceID::ItemIconTextureIDs_;
// 全てのアイテムのテクスチャID配列（モデルとして描画する用）
std::vector<int> ResourceID::ItemModelTextureIDs_;
// 全てのアイテムのモデルID配列
std::vector<int> ResourceID::ItemModelIDs_;

// ブロックテクスチャのみの配列
std::vector<int> ResourceID::BlockTextureIDs_;
// ブロックモデルのみの配列
std::vector<int> ResourceID::BlockModelIDs_;

// UIテクスチャID配列
std::vector<int> ResourceID::UITextureIDs_;

// その他のテクスチャID配列
std::vector<int> ResourceID::TextureIDs_;
// その他のモデルID配列
std::vector<int> ResourceID::ModelIDs_;

void ResourceID::reload()
{
	int tmp = -1;

#pragma region 全てのアイテムのテクスチャID配列（インベントリとかに表示するアイコン）

	ItemIconTextureIDs_.resize(static_cast<size_t>(ItemID::MAX));

	// 一旦モデル用のテクスチャを流用

	tmp = -1;
	ItemIconTextureIDs_[size_t(ItemID::None)] = tmp;

	tmp = Game::Resource::LoadTexture("resources/Minecraft/tool/sword/sword.png");
	ItemIconTextureIDs_[size_t(ItemID::木の剣)] = tmp;
	ItemIconTextureIDs_[size_t(ItemID::石の剣)] = tmp;
	ItemIconTextureIDs_[size_t(ItemID::鉄の剣)] = tmp;
	ItemIconTextureIDs_[size_t(ItemID::ダイヤの剣)] = tmp;

	tmp = Game::Resource::LoadTexture("resources/Minecraft/tool/pickel/pickel.png");
	ItemIconTextureIDs_[size_t(ItemID::木のツルハシ)] = tmp;
	ItemIconTextureIDs_[size_t(ItemID::石のツルハシ)] = tmp;
	ItemIconTextureIDs_[size_t(ItemID::鉄のツルハシ)] = tmp;
	ItemIconTextureIDs_[size_t(ItemID::ダイヤのツルハシ)] = tmp;

	tmp = Game::Resource::LoadTexture("resources/Minecraft/tool/axe/axe.png");
	ItemIconTextureIDs_[size_t(ItemID::木の斧)] = tmp;
	ItemIconTextureIDs_[size_t(ItemID::石の斧)] = tmp;
	ItemIconTextureIDs_[size_t(ItemID::鉄の斧)] = tmp;
	ItemIconTextureIDs_[size_t(ItemID::ダイヤの斧)] = tmp;


	tmp = Game::Resource::LoadTexture("resources/Minecraft/Blocks/glassIcon.png");
	ItemIconTextureIDs_[size_t(ItemID::ガラスブロック)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/Blocks/LeafIcon.png");
	ItemIconTextureIDs_[size_t(ItemID::葉ブロック)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/Blocks/WoodIcon.png");
	ItemIconTextureIDs_[size_t(ItemID::木ブロック)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/Blocks/LawnIcon.png");
	ItemIconTextureIDs_[size_t(ItemID::芝ブロック)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/Blocks/dirtIcon.png");
	ItemIconTextureIDs_[size_t(ItemID::土ブロック)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/Blocks/stoneIcon.png");
	ItemIconTextureIDs_[size_t(ItemID::石ブロック)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/Blocks/ironIcon.png");
	ItemIconTextureIDs_[size_t(ItemID::鉄ブロック)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/Blocks/goldIcon.png");
	ItemIconTextureIDs_[size_t(ItemID::金ブロック)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/Blocks/diamondIcon.png");
	ItemIconTextureIDs_[size_t(ItemID::ダイヤブロック)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/Blocks/bedrockIcon.png");
	ItemIconTextureIDs_[size_t(ItemID::岩盤ブロック)] = tmp;

	tmp = Game::Resource::LoadTexture("resources/Minecraft/item/ingot/ironIcon.png");
	ItemIconTextureIDs_[size_t(ItemID::鉄インゴット)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/item/ingot/goldIcon.png");
	ItemIconTextureIDs_[size_t(ItemID::金インゴット)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/item/diamond/diamondIcon.png");
	ItemIconTextureIDs_[size_t(ItemID::ダイヤモンド)] = tmp;

	tmp = Game::Resource::LoadTexture("resources/Prototypes/texture/cube.png");
	ItemIconTextureIDs_[size_t(ItemID::ビーコン)] = tmp;



#pragma endregion

#pragma region 全てのアイテムのモデルID配列

	ItemModelIDs_.resize(static_cast<size_t>(ItemID::MAX));

	tmp = -1;
	ItemModelIDs_[size_t(ItemID::None)] = tmp;

	tmp = Game::Resource::LoadModel("resources/Minecraft/tool/sword/", "sword.obj");
	ItemModelIDs_[size_t(ItemID::木の剣)] = tmp;
	ItemModelIDs_[size_t(ItemID::石の剣)] = tmp;
	ItemModelIDs_[size_t(ItemID::鉄の剣)] = tmp;
	ItemModelIDs_[size_t(ItemID::ダイヤの剣)] = tmp;

	tmp = Game::Resource::LoadModel("resources/Minecraft/tool/pickel/", "pickel.obj");
	ItemModelIDs_[size_t(ItemID::木のツルハシ)] = tmp;
	ItemModelIDs_[size_t(ItemID::石のツルハシ)] = tmp;
	ItemModelIDs_[size_t(ItemID::鉄のツルハシ)] = tmp;
	ItemModelIDs_[size_t(ItemID::ダイヤのツルハシ)] = tmp;

	tmp = Game::Resource::LoadModel("resources/Minecraft/tool/axe/", "axe.obj");
	ItemModelIDs_[size_t(ItemID::木の斧)] = tmp;
	ItemModelIDs_[size_t(ItemID::石の斧)] = tmp;
	ItemModelIDs_[size_t(ItemID::鉄の斧)] = tmp;
	ItemModelIDs_[size_t(ItemID::ダイヤの斧)] = tmp;

	tmp = Game::Resource::LoadModel("resources/Prototypes/model/", "cube.obj");
	ItemModelIDs_[size_t(ItemID::葉ブロック)] = tmp;
	ItemModelIDs_[size_t(ItemID::木ブロック)] = tmp;
	ItemModelIDs_[size_t(ItemID::芝ブロック)] = tmp;
	ItemModelIDs_[size_t(ItemID::土ブロック)] = tmp;
	ItemModelIDs_[size_t(ItemID::石ブロック)] = tmp;
	ItemModelIDs_[size_t(ItemID::鉄ブロック)] = tmp;
	ItemModelIDs_[size_t(ItemID::金ブロック)] = tmp;
	ItemModelIDs_[size_t(ItemID::ダイヤブロック)] = tmp;
	ItemModelIDs_[size_t(ItemID::岩盤ブロック)] = tmp;

	tmp = Game::Resource::LoadModel("resources/Minecraft/item/ingot/", "ingot.obj");
	ItemModelIDs_[size_t(ItemID::鉄インゴット)] = tmp;
	ItemModelIDs_[size_t(ItemID::金インゴット)] = tmp;

	tmp = Game::Resource::LoadModel("resources/Minecraft/item/diamond/", "diamond.obj");
	ItemModelIDs_[size_t(ItemID::ダイヤモンド)] = tmp;

	//tmp = Game::Resource::LoadModel("resources/Minecraft/block/beacon/", "beacon.obj");
	tmp = Game::Resource::LoadModel("resources/Prototypes/model/", "cube.obj");
	ItemModelIDs_[size_t(ItemID::ビーコン)] = tmp;

#pragma endregion

#pragma region 全てのアイテムのテクスチャID配列（モデルとして描画する用）

	ItemModelTextureIDs_.resize(static_cast<size_t>(ItemID::MAX));

	tmp = -1;
	ItemModelTextureIDs_[size_t(ItemID::None)] = tmp;

	tmp = Game::Resource::LoadTexture("resources/Minecraft/tool/sword/sword.png");
	ItemModelTextureIDs_[size_t(ItemID::木の剣)] = tmp;
	ItemModelTextureIDs_[size_t(ItemID::石の剣)] = tmp;
	ItemModelTextureIDs_[size_t(ItemID::鉄の剣)] = tmp;
	ItemModelTextureIDs_[size_t(ItemID::ダイヤの剣)] = tmp;

	tmp = Game::Resource::LoadTexture("resources/Minecraft/tool/pickel/pickel.png");
	ItemModelTextureIDs_[size_t(ItemID::木のツルハシ)] = tmp;
	ItemModelTextureIDs_[size_t(ItemID::石のツルハシ)] = tmp;
	ItemModelTextureIDs_[size_t(ItemID::鉄のツルハシ)] = tmp;
	ItemModelTextureIDs_[size_t(ItemID::ダイヤのツルハシ)] = tmp;

	tmp = Game::Resource::LoadTexture("resources/Minecraft/tool/axe/axe.png");
	ItemModelTextureIDs_[size_t(ItemID::木の斧)] = tmp;
	ItemModelTextureIDs_[size_t(ItemID::石の斧)] = tmp;
	ItemModelTextureIDs_[size_t(ItemID::鉄の斧)] = tmp;
	ItemModelTextureIDs_[size_t(ItemID::ダイヤの斧)] = tmp;

	tmp = Game::Resource::LoadTexture("resources/Minecraft/Blocks/glass.png");
	ItemModelTextureIDs_[size_t(ItemID::ガラスブロック)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/Blocks/Leaf.png");
	ItemModelTextureIDs_[size_t(ItemID::葉ブロック)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/Blocks/Wood.png");
	ItemModelTextureIDs_[size_t(ItemID::木ブロック)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/Blocks/Lawn.png");
	ItemModelTextureIDs_[size_t(ItemID::芝ブロック)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/Blocks/dirt.png");
	ItemModelTextureIDs_[size_t(ItemID::土ブロック)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/Blocks/stone.png");
	ItemModelTextureIDs_[size_t(ItemID::石ブロック)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/Blocks/iron.png");
	ItemModelTextureIDs_[size_t(ItemID::鉄ブロック)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/Blocks/gold.png");
	ItemModelTextureIDs_[size_t(ItemID::金ブロック)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/Blocks/diamond.png");
	ItemModelTextureIDs_[size_t(ItemID::ダイヤブロック)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/Blocks/bedrock.png");
	ItemModelTextureIDs_[size_t(ItemID::岩盤ブロック)] = tmp;

	tmp = Game::Resource::LoadTexture("resources/Minecraft/item/ingot/iron.png");
	ItemModelTextureIDs_[size_t(ItemID::鉄インゴット)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/item/ingot/gold.png");
	ItemModelTextureIDs_[size_t(ItemID::金インゴット)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/item/diamond/diamond.png");
	ItemModelTextureIDs_[size_t(ItemID::ダイヤモンド)] = tmp;

	tmp = Game::Resource::LoadTexture("resources/Minecraft/blocks/beacon.png");
	ItemModelTextureIDs_[size_t(ItemID::ビーコン)] = tmp;

#pragma endregion

#pragma region ブロックテクスチャ読み込み

	BlockTextureIDs_.resize(static_cast<size_t>(BlockID::MAX));

	tmp = -1;
	BlockTextureIDs_[size_t(BlockID::Air)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/Blocks/glass.png");
	BlockTextureIDs_[size_t(BlockID::Glass)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/Blocks/Leaf.png");
	BlockTextureIDs_[size_t(BlockID::Leaf)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/Blocks/Wood.png");
	BlockTextureIDs_[size_t(BlockID::Wood)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/Blocks/Lawn.png");
	BlockTextureIDs_[size_t(BlockID::Lawn)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/Blocks/dirt.png");
	BlockTextureIDs_[size_t(BlockID::Dirt)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/Blocks/stone.png");
	BlockTextureIDs_[size_t(BlockID::Stone)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/Blocks/iron.png");
	BlockTextureIDs_[size_t(BlockID::Iron)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/Blocks/gold.png");
	BlockTextureIDs_[size_t(BlockID::Gold)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/Blocks/diamond.png");
	BlockTextureIDs_[size_t(BlockID::Diamond)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/Blocks/bedrock.png");
	BlockTextureIDs_[size_t(BlockID::Bedrock)] = tmp;

#pragma endregion

#pragma region ブロックモデル読み込み
	
	BlockModelIDs_.resize(static_cast<size_t>(BlockID::MAX));

	tmp = -1;
	BlockModelIDs_[size_t(BlockID::Air)] = tmp;
	tmp = Game::Resource::LoadModel("resources/Prototypes/model/", "cube.obj");
	BlockModelIDs_[size_t(BlockID::Leaf)] = tmp;
	BlockModelIDs_[size_t(BlockID::Wood)] = tmp;
	BlockModelIDs_[size_t(BlockID::Lawn)] = tmp;
	BlockModelIDs_[size_t(BlockID::Dirt)] = tmp;
	BlockModelIDs_[size_t(BlockID::Stone)] = tmp;
	BlockModelIDs_[size_t(BlockID::Iron)] = tmp;
	BlockModelIDs_[size_t(BlockID::Gold)] = tmp;
	BlockModelIDs_[size_t(BlockID::Diamond)] = tmp;
	BlockModelIDs_[size_t(BlockID::Bedrock)] = tmp;

#pragma endregion

#pragma region テクスチャ読み込み

	TextureIDs_.resize(static_cast<size_t>(TextureID::MAX));

	tmp = -1;
	TextureIDs_[size_t(TextureID::None)] = tmp;

	tmp = Game::Resource::LoadTexture("resources/Prototypes/texture/uvChecker.png");
	TextureIDs_[size_t(TextureID::UVChecker)] = tmp;

	tmp = Game::Resource::LoadTexture("resources/Prototypes/texture/monsterBall.png");
	TextureIDs_[size_t(TextureID::monsterBall)] = tmp;

	tmp = Game::Resource::LoadTexture("resources/Prototypes/texture/white1x1.png");
	TextureIDs_[size_t(TextureID::white1x1)] = tmp;

	tmp = Game::Resource::LoadTexture("resources/Prototypes/texture/particle/circle.png");
	TextureIDs_[size_t(TextureID::Circle)] = tmp;

	tmp = Game::Resource::LoadTexture("resources/Minecraft/Title/start.png");
	TextureIDs_[size_t(TextureID::TITLE_start)] = tmp;

	tmp = Game::Resource::LoadTexture("resources/Minecraft/Title/option.png");
	TextureIDs_[size_t(TextureID::TITLE_option)] = tmp;

	tmp = Game::Resource::LoadTexture("resources/Minecraft/Item_slot.png");
	TextureIDs_[size_t(TextureID::Item_slot)] = tmp;

#pragma endregion

#pragma region モデル読み込み

	ModelIDs_.resize(static_cast<size_t>(ModelID::MAX));

	tmp = -1;
	ModelIDs_[size_t(ModelID::None)] = tmp;

	tmp = Game::Resource::LoadModel("resources/Prototypes/model/", "cube.obj");
	ModelIDs_[size_t(ModelID::Cube)] = tmp;
	tmp = Game::Resource::LoadModel("resources/Prototypes/model/", "plane.obj");
	ModelIDs_[size_t(ModelID::Plane)] = tmp;
	tmp = Game::Resource::LoadModel("resources/Prototypes/model/", "corn.obj");
	ModelIDs_[size_t(ModelID::Corn)] = tmp;
	tmp = Game::Resource::LoadModel("resources/Prototypes/model/", "sphere.obj");
	ModelIDs_[size_t(ModelID::Sphere)] = tmp;

#pragma endregion

#pragma region UIテクスチャ読み込み

	UITextureIDs_.resize(static_cast<size_t>(UITextureID::MAX));
	tmp = -1;
	UITextureIDs_[size_t(UITextureID::None)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/UI/inventory.png");
	UITextureIDs_[size_t(UITextureID::Inventory)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/UI/Hotbar.png");
	UITextureIDs_[size_t(UITextureID::Hotbar)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/UI/Hotbar_Selected.png");
	UITextureIDs_[size_t(UITextureID::Hotbar_Selected)] = tmp;

#pragma endregion


	std::vector<std::string> breakBlockFilePaths =
	{
		"resources/Minecraft/breakBlock/breakBlock_0.png",
		"resources/Minecraft/breakBlock/breakBlock_1.png",
		"resources/Minecraft/breakBlock/breakBlock_2.png",
		"resources/Minecraft/breakBlock/breakBlock_3.png",
		"resources/Minecraft/breakBlock/breakBlock_4.png",
		"resources/Minecraft/breakBlock/breakBlock_5.png",
	};

	tmp = Game::Resource::LoadTextureArray(breakBlockFilePaths);
	TextureIDs_[size_t(TextureID::BreakBlock_Array)] = tmp;

}