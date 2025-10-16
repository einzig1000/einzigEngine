#pragma once
#include <cstdint>
#include <string>
#include <d3d12.h>
#include <vector>
#include <map>
#include "definition/definition.h"

class DescriptorHeapManager;

class TextureManager
{
public:
    TextureManager();
    ~TextureManager();

    uint32_t LoadTexture(
        const std::string& filePath, 
        ID3D12GraphicsCommandList* commandList,
        DescriptorHeapManager* descriptorHeap,
        ID3D12Device* device
    );
    TextureData* GetTexture(uint32_t textureID);

private:
    // 画像データを詰める
    std::vector<TextureData> textures_;

    // アップロード用一時リソースを保持するリスト
    std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> intermediateUploadResources_;
};

