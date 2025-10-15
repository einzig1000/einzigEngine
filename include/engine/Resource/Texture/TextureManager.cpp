#include "Resource/Texture/TextureManager.h"
#include "externals/DirectXTex/d3dx12.h"
#include "externals/DirectXTex/DirectXTex.h"
#include "Utilities/functions.h"
#include "cassert"

TextureManager::TextureManager()
{
    //assert(device_ != nullptr && "ID3D12Device* device cannot be null.");
    //assert(srvDescriptorHeap_ != nullptr && "ID3D12DescriptorHeap* srvDescriptorHeap cannot be null.");
}

TextureManager::~TextureManager()
{
    intermediateUploadResources_.clear();
}


uint32_t TextureManager::LoadTexture(const std::string& filePath, ID3D12GraphicsCommandList* commandList, ID3D12DescriptorHeap* srvDescriptorHeap, ID3D12Device* device)
{
    // ボックスを作成
    TextureData text;

    // テクスチャファイルを読んでプログラムを扱えるようにする
    DirectX::ScratchImage image{};
    std::wstring filePathw = ConvertString(filePath);
    HRESULT hr = DirectX::LoadFromWICFile(filePathw.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
    assert(SUCCEEDED(hr));

    // ミップマップの作成
    DirectX::ScratchImage mipImageLocal;
    hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImageLocal);
    assert(SUCCEEDED(hr));

    text.metadata = mipImageLocal.GetMetadata();
    text.number = static_cast<uint32_t> (textures_.size());
    text.mipImage = std::move(mipImageLocal);


    // テクスチャリソースとSRVの作成
    text.textureResource = CreateTextureResource(device, text.metadata);
    Microsoft::WRL::ComPtr<ID3D12Resource> tempIntermediateResource = UploadTextureData(text.textureResource.Get(), text.mipImage, device, commandList);
    intermediateUploadResources_.push_back(tempIntermediateResource);


    const uint32_t descriptorSizeSRV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU = GetCPUDescriptorHandle(srvDescriptorHeap, descriptorSizeSRV, text.number + 1);
    text.textureSrvHandleGPU = GetGPUDescriptorHandle(srvDescriptorHeap, descriptorSizeSRV, text.number + 1);

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format = text.metadata.format;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = UINT(text.metadata.mipLevels);

    device->CreateShaderResourceView(text.textureResource.Get(), &srvDesc, textureSrvHandleCPU);

    textures_.push_back(std::move(text));

    return text.number;
}

TextureData* TextureManager::GetTexture(uint32_t textureID)
{
    if (textureID < textures_.size())
    {
        return &textures_[textureID];
    }
    else
    {
        Log("存在しないテクスチャIDです:%d", textureID);
        return nullptr;
    }
}
