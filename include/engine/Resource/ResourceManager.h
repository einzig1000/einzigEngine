#pragma once
#include <d3d12.h>
#include <wrl.h>
#include "Resource/Audio/AudioManager.h"
#include "Resource/Texture/TextureManager.h"
#include "Resource/Model/ModelManager.h"
#include <memory> 

class ResourceManager
{
public:
	ResourceManager();
	~ResourceManager();

	AudioManager* GetAudioManager() const { return audioManager_.get(); }
	TextureManager* GetTextureManager() const { return textureManager_.get(); }
	ModelManager* GetModelManager() const { return modelManager_.get(); }

private:

	std::unique_ptr<AudioManager> audioManager_;
	std::unique_ptr<TextureManager> textureManager_;
	std::unique_ptr<ModelManager> modelManager_;

};

