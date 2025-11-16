#include "ResourceID.h"
#include "Game.h"

std::vector<int> ResourceID::blockTextureIDs_;
std::vector<int> ResourceID::blockModelIDs_;
std::vector<int> ResourceID::TextureIDs_;

ResourceID::ResourceID()
{
#pragma region ブロックテクスチャ読み込み

	blockTextureIDs_.resize(static_cast<size_t>(BlockTextureID::TexMax));

	int tmp = 0;
	blockTextureIDs_[size_t(BlockTextureID::None)] = tmp;

	tmp = Game::Resource::LoadTexture("resources/Minecraft/Blocks/stone.png");
	blockTextureIDs_[size_t(BlockTextureID::Stone)] = tmp;

	tmp = Game::Resource::LoadTexture("resources/Minecraft/Blocks/grass.png");
	blockTextureIDs_[size_t(BlockTextureID::Grass)] = tmp;

	tmp = Game::Resource::LoadTexture("resources/Minecraft/Blocks/dirt.png");
	blockTextureIDs_[size_t(BlockTextureID::Dirt)] = tmp;

	tmp = Game::Resource::LoadTexture("resources/Minecraft/Blocks/lawn.png");
	blockTextureIDs_[size_t(BlockTextureID::lawn)] = tmp;

	tmp = Game::Resource::LoadTexture("resources/Minecraft/Blocks/wood.png");
	blockTextureIDs_[size_t(BlockTextureID::wood)] = tmp;

	tmp = Game::Resource::LoadTexture("resources/Minecraft/Blocks/leaf.png");
	blockTextureIDs_[size_t(BlockTextureID::leaf)] = tmp;

	tmp = Game::Resource::LoadTexture("resources/Minecraft/breakBlock/breakBlock_0.png");
	blockTextureIDs_[size_t(BlockTextureID::BreakBlock_0)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/breakBlock/breakBlock_1.png");
	blockTextureIDs_[size_t(BlockTextureID::BreakBlock_1)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/breakBlock/breakBlock_2.png");
	blockTextureIDs_[size_t(BlockTextureID::BreakBlock_2)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/breakBlock/breakBlock_3.png");
	blockTextureIDs_[size_t(BlockTextureID::BreakBlock_3)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/breakBlock/breakBlock_4.png");
	blockTextureIDs_[size_t(BlockTextureID::BreakBlock_4)] = tmp;


#pragma endregion

#pragma region ブロックモデル読み込み

	blockModelIDs_.resize(static_cast<size_t>(ModelID::ModelMax));

	tmp = 0;
	blockModelIDs_[size_t(ModelID::None)] = tmp;

	tmp = Game::Resource::LoadModel("Resources/Prototypes/model/", "cube.obj");
	blockModelIDs_[size_t(ModelID::Cube)] = tmp;

#pragma endregion

#pragma region テストテクスチャ読み込み

	TextureIDs_.resize(static_cast<size_t>(TextureID::TexMax));

	tmp = 0;
	TextureIDs_[size_t(TextureID::None)] = tmp;

	tmp = Game::Resource::LoadTexture("Resources/Prototypes/texture/uvChecker.png");
	TextureIDs_[size_t(TextureID::UVChecker)] = tmp;

#pragma endregion
}

ResourceID::~ResourceID()
{

}