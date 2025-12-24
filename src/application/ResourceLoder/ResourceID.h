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

class ResourceID
{
public:

	static void reload();

	static uint32_t GetTextureID(TextureID id)
	{
		return TextureIDs_[static_cast<size_t>(id)];
	}

	static uint32_t GetTextureID(BlockID id)
	{
		return blockTextureIDs_[static_cast<size_t>(id)];
	}

	static uint32_t GetModelID(ModelID id)
	{
		return modelIDs_[static_cast<size_t>(id)];
	}

private:

	static std::vector<int> blockTextureIDs_;
	static std::vector<int> modelIDs_;
	static std::vector<int> TextureIDs_;


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