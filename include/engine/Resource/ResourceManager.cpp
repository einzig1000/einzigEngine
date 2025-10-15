#include "ResourceManager.h"
#include "Utilities/functions.h"

ResourceManager::ResourceManager()
{
    audioManager_ = std::make_unique<AudioManager>();
    textureManager_ = std::make_unique<TextureManager>();
    modelManager_ = std::make_unique<ModelManager>();
}

ResourceManager::~ResourceManager()
{}
