#include "Resource/Texture/TextureManager.h"
#include "externals/DirectXTex/d3dx12.h"
#include "externals/DirectXTex/DirectXTex.h"
#include "Utilities/functions.h"
#include "DirectX/DescriptorHeapManager.h"
#include "cassert"

TextureManager::TextureManager(ID3D12GraphicsCommandList* commandList, DescriptorHeapManager* descriptorHeap, ID3D12Device* device)
	:commandList_(commandList), descriptorHeap_(descriptorHeap), device_(device)
{
    // 透明テクスチャを最初に作成
	//CreateTransparentTexture();
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
    text.textureResource = CreateTextureResource(device_, text.metadata);
    Microsoft::WRL::ComPtr<ID3D12Resource> tempIntermediateResource = UploadTextureData(text.textureResource.Get(), text.mipImage, device_, commandList_);
    intermediateUploadResources_.push_back(tempIntermediateResource);

	SRVAllocation srvAllocation = descriptorHeap_->GetSrvManager()->CreateSRVforTexture(text.textureResource.Get(), text.metadata.format, UINT(text.metadata.mipLevels));
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

int32_t TextureManager::LoadTexture2DArray(const std::vector<std::string>& filePaths)
{
	// filePaths[0] を代表名として使う
	std::string arrayName = filePaths[1] + "_Array";
    auto exists = std::find_if(
        textures_.begin(), textures_.end(),
        [&arrayName](const TextureData& tex) { return tex.filePath == arrayName; }
    );
    if (exists != textures_.end())
    {
        return exists->number;
    }

    if (filePaths.empty())
    {
        Log("LoadTexture2DArray: filePaths is empty. name:%s", arrayName.c_str());
        return -1;
    }

    // まず全画像を読み、同一条件チェック
    std::vector<DirectX::ScratchImage> srcImages;
    srcImages.resize(filePaths.size());

    std::vector<DirectX::ScratchImage> srcMipImages;
    srcMipImages.resize(filePaths.size());

    DirectX::TexMetadata baseMeta{};

    for (size_t i = 0; i < filePaths.size(); ++i)
    {
        std::wstring pathW = ConvertString(filePaths[i]);

        HRESULT hr = DirectX::LoadFromWICFile(pathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, srcImages[i]);
        assert(SUCCEEDED(hr));

        hr = DirectX::GenerateMipMaps(
            srcImages[i].GetImages(),
            srcImages[i].GetImageCount(),
            srcImages[i].GetMetadata(),
            DirectX::TEX_FILTER_SRGB,
            0,
            srcMipImages[i]);
        assert(SUCCEEDED(hr));

        const auto meta = srcMipImages[i].GetMetadata();
        if (i == 0)
        {
            baseMeta = meta;
            // Texture2DArray 前提なので arraySize=1 の画像であること
            assert(baseMeta.arraySize == 1);
        }
        else
        {
            // サイズ/フォーマット/ミップ数を揃える（今回の前提）
            assert(meta.width == baseMeta.width);
            assert(meta.height == baseMeta.height);
            assert(meta.format == baseMeta.format);
            assert(meta.mipLevels == baseMeta.mipLevels);
            assert(meta.dimension == baseMeta.dimension);
            assert(meta.arraySize == 1);
        }
    }

    // 各要素の先頭mip(0)の Image を配列として束ねる
    std::vector<const DirectX::Image*> images;
    images.reserve(filePaths.size());
    for (size_t i = 0; i < filePaths.size(); ++i)
    {
        const DirectX::Image* img0 = srcMipImages[i].GetImage(0, 0, 0);
        assert(img0);
        images.push_back(img0);
    }

    // 2D配列イメージ(配列=6, mip=baseMeta.mipLevels)を作成
    DirectX::ScratchImage arrayScratch;
    HRESULT hr = arrayScratch.Initialize2D(baseMeta.format, baseMeta.width, baseMeta.height, filePaths.size(), baseMeta.mipLevels);
    assert(SUCCEEDED(hr));

    // mipごとに各sliceへコピー（DirectXTexのCopyRectangleで詰める）
    for (size_t slice = 0; slice < filePaths.size(); ++slice)
    {
        for (size_t mip = 0; mip < baseMeta.mipLevels; ++mip)
        {
            const DirectX::Image* src = srcMipImages[slice].GetImage(mip, 0, 0);
            DirectX::Image* dst = const_cast<DirectX::Image*>(arrayScratch.GetImage(mip, slice, 0));
            assert(src && dst);

            DirectX::Rect rect{};
            rect.x = 0;
            rect.y = 0;
            rect.w = static_cast<LONG>(dst->width);
            rect.h = static_cast<LONG>(dst->height);

            hr = DirectX::CopyRectangle(*src, rect, *dst, DirectX::TEX_FILTER_DEFAULT, 0, 0);
            assert(SUCCEEDED(hr));
        }
    }

    // TextureDataとして登録
    TextureData tex{};
    tex.filePath = arrayName;
    tex.number = static_cast<uint32_t>(textures_.size());
    tex.mipImage = std::move(arrayScratch);
    tex.metadata = tex.mipImage.GetMetadata(); // arraySize が filePaths.size() になる

    // GPUリソース作成＆アップロード（既存関数を流用）
    tex.textureResource = CreateTextureResource(device_, tex.metadata);
    Microsoft::WRL::ComPtr<ID3D12Resource> tempIntermediateResource = UploadTextureData(tex.textureResource.Get(), tex.mipImage, device_, commandList_);
    intermediateUploadResources_.push_back(tempIntermediateResource);

    // Texture2DArray用のSRVを作る
    SRVAllocation srvAllocation = descriptorHeap_->GetSrvManager()->CreateSRVforTextureArray(
        tex.textureResource.Get(),
        tex.metadata.format,
        UINT(tex.metadata.mipLevels),
        UINT(tex.metadata.arraySize));
    tex.textureSrvHandleGPU = srvAllocation.gpu;

    textures_.push_back(std::move(tex));
    return tex.number;
}

void TextureManager::CreateTransparentTexture()
{
    // 透明テクスチャを作成
    TextureData text;
    // ファイルパスを保存
    text.filePath = "TransparentTexture";
    // 1x1のRGBA8形式の透明テクスチャデータを作成
    DirectX::ScratchImage mipImageLocal;
    DirectX::Image image{};
    image.width = 1;
    image.height = 1;
    image.format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    image.rowPitch = 4; // 1ピクセルあたり4バイト (RGBA8)
    image.slicePitch = image.rowPitch * image.height;
    std::vector<uint8_t> pixelData = { 0, 0, 0, 0 };
    image.pixels = pixelData.data();
    HRESULT hr = mipImageLocal.InitializeFromImage(image);
    assert(SUCCEEDED(hr));
    text.metadata = mipImageLocal.GetMetadata();
    text.number = static_cast<uint32_t>(textures_.size());
    text.mipImage = std::move(mipImageLocal);
    // テクスチャリソースとSRVの作成
    text.textureResource = CreateTextureResource(device_, text.metadata);
    Microsoft::WRL::ComPtr<ID3D12Resource> tempIntermediateResource = UploadTextureData(text.textureResource.Get(), text.mipImage, device_, commandList_);
    intermediateUploadResources_.push_back(tempIntermediateResource);
    SRVAllocation srvAllocation = descriptorHeap_->GetSrvManager()->CreateSRVforTexture(text.textureResource.Get(), text.metadata.format, UINT(text.metadata.mipLevels));
    text.textureSrvHandleGPU = srvAllocation.gpu;
	textures_.push_back(std::move(text));
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
    Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource = CreateBufferResource(device, intermediateSize);
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
