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
    TextureData* GetTextureData(uint32_t textureID);

	// テクスチャ数を取得
	size_t GetTextureCount() const { return textures_.size(); }

private:
    // 画像データを詰める
    std::vector<TextureData> textures_;

    // アップロード用一時リソースを保持するリスト
    std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> intermediateUploadResources_;


    /// <summary>
    /// テクスチャのメタデータを基に DirectX 12 のテクスチャリソースを作成する関数
    /// </summary>
    /// <param name="device">DirectX 12 デバイス</param>
    /// <param name="metadata">テクスチャのメタデータ</param>
    /// <returns>作成されたテクスチャリソース</returns>
    Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata);


    /// <summary>
    /// テクスチャデータを GPU にアップロードする関数
    /// </summary>
    /// <param name="texture">アップロード先のテクスチャリソース</param>
    /// <param name="mipImages">ミップマップデータ</param>
    /// <param name="device">DirectX 12 デバイス</param>
    /// <param name="commandList">コマンドリスト</param>
    /// <returns>アップロードに使用した中間リソース</returns>
    [[nodiscard]]
    Microsoft::WRL::ComPtr<ID3D12Resource> UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages, ID3D12Device* device, ID3D12GraphicsCommandList* commandList);

};

