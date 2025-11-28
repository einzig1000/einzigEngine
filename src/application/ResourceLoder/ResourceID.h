#pragma once
#include "definition/definition.h"
#include <vector>
#include "ResourceName.h"

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

	ResourceID() = delete;
	~ResourceID() = delete;

};