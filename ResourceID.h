#pragma once
#include "definition/definition.h"
#include <vector>

enum class TextureID
{
	None,
	Stone,

	TexMax,
};

class ResourceID
{
public:
	ResourceID();
	~ResourceID();

private:
	
	std::vector<int> tt;

	  <int> blockTextureID_;

};