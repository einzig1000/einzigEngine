#pragma once
#include "definition/definition.h"
#include <vector>

enum class BlockTextureID
{
	None,
	Stone,
	Grass,
	Dirt,

	BreakBlock_0,
	BreakBlock_1,
	BreakBlock_2,
	BreakBlock_3,
	BreakBlock_4,


	TexMax,
};

enum class ModelID
{
	None,
	Cube,


	ModelMax,
};

enum class TextureID
{
	None,
	UVChecker,

	TexMax,
};

class ResourceID
{
public:
	ResourceID();
	~ResourceID();

	static std::vector<int> blockTextureIDs_;
	static std::vector<int> blockModelIDs_;
	static std::vector<int> TextureIDs_;

private:
	

};