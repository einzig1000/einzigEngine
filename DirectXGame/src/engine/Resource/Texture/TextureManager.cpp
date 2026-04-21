#include "Resource/Texture/TextureManager.h"
#include "externals/DirectXTex/d3dx12.h"
#include "externals/DirectXTex/DirectXTex.h"
#include <Utilities/Logger/Logger.h>
#include <Utilities/Converter/StringConverter/StringConverter.h>
#include "DirectX/DescriptorHeapManager/DescriptorHeapManager.h"
#include <DirectX/Resource/Dx12ResourceFactory.h>
#include <cassert>

TextureManager::TextureManager(ID3D12GraphicsCommandList* commandList, DescriptorHeapManager* descriptorHeap, ID3D12Device* device)
	:commandList_(commandList), descriptorHeap_(descriptorHeap), device_(device)
{
}

TextureManager::~TextureManager()
{
    intermediateUploadResources_.clear();
}


int32_t TextureManager::LoadTexture(const std::string& filePath)
{
    auto exists = std::find_if(
        textures_.begin(), textures_.end(),
        [&filePath](const TextureData& tex) { return tex.filePath == filePath; }
	);
    if (exists != textures_.end())
    {
        return exists->number;
	}

    // ボックスを作成
    TextureData text;

	// ファイルパスを保存
    text.filePath = filePath;

    // テクスチャファイルを読んでプログラムを扱えるようにする
    DirectX::ScratchImage image{};
    std::wstring filePathw = StringConverter::Convert(filePath);
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
    text.textureResource = CreateTextureResource(device_, text.metadata);
    Microsoft::WRL::ComPtr<ID3D12Resource> tempIntermediateResource = UploadTextureData(text.textureResource.Get(), text.mipImage, device_, commandList_);
    intermediateUploadResources_.push_back(tempIntermediateResource);

    SRV_UAVManager::Allocation srvAllocation = descriptorHeap_->GetSRV_UAVManager()->CreateSRVforTexture(text.textureResource.Get(), text.metadata.format, UINT(text.metadata.mipLevels));
	text.textureSrvHandleGPU = srvAllocation.gpu;

    textures_.push_back(std::move(text));

    return text.number;
}

TextureData* TextureManager::GetTextureData(int32_t textureID)
{
    if (textureID < 0)
    {
        return nullptr;
	}

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

// 2,
Microsoft::WRL::ComPtr<ID3D12Resource> TextureManager::CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata)
{
    // 1,metadataを基にResourceの設定
    D3D12_RESOURCE_DESC resourceDesc{};
    resourceDesc.Width = UINT(metadata.width);
    resourceDesc.Height = UINT(metadata.height);
    resourceDesc.MipLevels = UINT16(metadata.mipLevels); // mipmapの数
    resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize); // 奥行き or 配列Textureの配列数
    resourceDesc.Format = metadata.format; // TextureのFormat

    resourceDesc.SampleDesc.Count = 1; // サンプリングカウント。１固定
    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension); // Textureの次元数。普段使ってるのは２次元

    // 2,利用するHeapの設定
    D3D12_HEAP_PROPERTIES heapProperties{};
    heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

    // 3,Resourceを生成する
    Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
    HRESULT hr = device->CreateCommittedResource(
        &heapProperties, // Heapの設定
        D3D12_HEAP_FLAG_NONE, // Heapの特殊な設定
        &resourceDesc, // Resourceの設定
        D3D12_RESOURCE_STATE_COPY_DEST, // 初回のResourceState.Textureは基本読むだけ
        nullptr, // Clear最適解。使わないのでnullptr
        IID_PPV_ARGS(&resource) // 作成するResourceポインタへのポインタ
    );
    assert(SUCCEEDED(hr));
    resource->SetName(L"CreateTextureResource()");

    return resource;
}



// 3,TextureResourceにデータを転送する
[[nodiscard]]
Microsoft::WRL::ComPtr<ID3D12Resource> TextureManager::UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages, ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
{
    std::vector<D3D12_SUBRESOURCE_DATA> subresources;
    DirectX::PrepareUpload(device, mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subresources);
    uint64_t intermediateSize = GetRequiredIntermediateSize(texture, 0, UINT(subresources.size()));
    Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource = Dx12ResourceFactory::CreateBufferResource(device, intermediateSize);
    UpdateSubresources(commandList, texture, intermediateResource.Get(), 0, 0, UINT(subresources.size()), subresources.data());
    // Tetureへの転送後は利用できるよう、D3D12_RESOURCE_STATE_COPY_DESTからD3D12_RESOURCE_STATE_GENERIC_READ ResourceStateを変更する
    D3D12_RESOURCE_BARRIER barrier{};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = texture;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
    commandList->ResourceBarrier(1, &barrier);
    return intermediateResource;
}
