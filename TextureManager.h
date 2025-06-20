#pragma once
#include <cstdint>
#include <string>
#include <d3d12.h>
#include <vector>
#include "definition.h"

class TextureManager
{
public:
    // 書き換えるからconstするな
    TextureManager(ID3D12Device* device, ID3D12DescriptorHeap* srvDescriptorHeap);
    ~TextureManager();

    uint32_t LoadTexture(const std::string& filePath, ID3D12GraphicsCommandList* commandList);
    TextureData* GetTexture(uint32_t textureID);

private:
    // 画像データを詰める
    std::vector<TextureData> textures_;

    ID3D12Device* device_;
    ID3D12DescriptorHeap* srvDescriptorHeap_;

    // アップロード用一時リソースを保持するリスト
    std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> intermediateUploadResources_;
};

