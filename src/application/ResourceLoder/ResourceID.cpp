#include "ResourceLoder/ResourceID.h"
#include "Game.h"

std::vector<int> ResourceID::blockTextureIDs_;
std::vector<int> ResourceID::modelIDs_;
std::vector<int> ResourceID::TextureIDs_;

void ResourceID::reload()
{

#pragma region ブロックテクスチャ読み込み

	blockTextureIDs_.resize(static_cast<size_t>(BlockID::MAX));

	int tmp = -1;
	blockTextureIDs_[size_t(BlockID::None)] = tmp;

	tmp = Game::Resource::LoadTexture("resources/Minecraft/Blocks/stone.png");
	blockTextureIDs_[size_t(BlockID::Stone)] = tmp;

	tmp = Game::Resource::LoadTexture("resources/Minecraft/Blocks/glass.png");
	blockTextureIDs_[size_t(BlockID::Glass)] = tmp;

	tmp = Game::Resource::LoadTexture("resources/Minecraft/Blocks/dirt.png");
	blockTextureIDs_[size_t(BlockID::Dirt)] = tmp;

	tmp = Game::Resource::LoadTexture("resources/Minecraft/Blocks/lawn.png");
	blockTextureIDs_[size_t(BlockID::lawn)] = tmp;

	tmp = Game::Resource::LoadTexture("resources/Minecraft/Blocks/wood.png");
	blockTextureIDs_[size_t(BlockID::wood)] = tmp;

	tmp = Game::Resource::LoadTexture("resources/Minecraft/Blocks/leaf.png");
	blockTextureIDs_[size_t(BlockID::leaf)] = tmp;


#pragma endregion

#pragma region モデル読み込み

	modelIDs_.resize(static_cast<size_t>(ModelID::MAX));

	tmp = -1;
	modelIDs_[size_t(ModelID::None)] = tmp;

	tmp = Game::Resource::LoadModel("resources/Prototypes/model/", "cube.obj");
	modelIDs_[size_t(ModelID::Cube)] = tmp;

	tmp = Game::Resource::LoadModel("resources/Prototypes/model/", "plane.obj");
	modelIDs_[size_t(ModelID::Plane)] = tmp;

	tmp = Game::Resource::LoadModel("resources/Prototypes/model/", "corn.obj");
	modelIDs_[size_t(ModelID::Corn)] = tmp;

	tmp = Game::Resource::LoadModel("resources/Prototypes/model/", "sphere.obj");
	modelIDs_[size_t(ModelID::Sphere)] = tmp;

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

	tmp = Game::Resource::LoadTexture("resources/Prototypes/texture/circle.png");
	TextureIDs_[size_t(TextureID::Circle)] = tmp;

	tmp = Game::Resource::LoadTexture("resources/Minecraft/Title/start.png");
	TextureIDs_[size_t(TextureID::TITLE_start)] = tmp;

	tmp = Game::Resource::LoadTexture("resources/Minecraft/Title/option.png");
	TextureIDs_[size_t(TextureID::TITLE_option)] = tmp;

	tmp = Game::Resource::LoadTexture("resources/Minecraft/Item_slot.png");
	TextureIDs_[size_t(TextureID::Item_slot)] = tmp;


	tmp = Game::Resource::LoadTexture("resources/Minecraft/breakBlock/breakBlock_0.png");
	TextureIDs_[size_t(TextureID::BreakBlock_0)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/breakBlock/breakBlock_1.png");
	TextureIDs_[size_t(TextureID::BreakBlock_1)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/breakBlock/breakBlock_2.png");
	TextureIDs_[size_t(TextureID::BreakBlock_2)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/breakBlock/breakBlock_3.png");
	TextureIDs_[size_t(TextureID::BreakBlock_3)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/breakBlock/breakBlock_4.png");
	TextureIDs_[size_t(TextureID::BreakBlock_4)] = tmp;

#pragma endregion

}