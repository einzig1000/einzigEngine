#include "TextureManager.h"
#include "externals/DirectXTex/d3dx12.h"
#include "externals/DirectXTex/DirectXTex.h"
#include "functions.h"
#include "cassert"

TextureManager::TextureManager(ID3D12Device* device, ID3D12DescriptorHeap* srvDescriptorHeap)
    :device_(device), srvDescriptorHeap_(srvDescriptorHeap)
{
    assert(device_ != nullptr && "ID3D12Device* device cannot be null.");
    assert(srvDescriptorHeap_ != nullptr && "ID3D12DescriptorHeap* srvDescriptorHeap cannot be null.");
}


uint32_t TextureManager::LoadTexture(const std::string& filePath, ID3D12GraphicsCommandList* commandList)
{
    // ボックスを作成
    TextureData& text = textures_.emplace_back();

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
    text.textureResource = (CreateTextureResource(device_, text.metadata));
    Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource = UploadTextureData(text.textureResource.Get(), text.mipImage, device_, commandList);


    const uint32_t descriptorSizeSRV = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU = GetCPUDescriptorHandle(srvDescriptorHeap_, descriptorSizeSRV, text.number + 1);
    text.textureSrvHandleGPU = GetGPUDescriptorHandle(srvDescriptorHeap_, descriptorSizeSRV, text.number + 1);

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format = text.metadata.format;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = UINT(text.metadata.mipLevels);

    device_->CreateShaderResourceView(text.textureResource.Get(), &srvDesc, textureSrvHandleCPU);

    return text.number;
}

TextureData* TextureManager::GetTexture(uint32_t textureID)
{
    for (auto& t : textures_)
    {
        if (t.number == textureID)
        {
            return &t;
        }
    }
    Log("存在しないテクスチャIDです:%d", textureID);
    return nullptr;
}
