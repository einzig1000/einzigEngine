#include "Dx12ResourceFactory.h"
#include <cassert>

namespace Dx12ResourceFactory
{

    Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(
        ID3D12Device* device, size_t sizeInBytes)
    {
        // ID3D12Resourceを格納するポインタ
        Microsoft::WRL::ComPtr<ID3D12Resource> pResource = nullptr;

        D3D12_HEAP_PROPERTIES heapProperties{};
        heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;

        // リソース記述子を作成
        D3D12_RESOURCE_DESC resourceDesc{};
        // バッファリソース。テクスチャの場合はまた別の設定をする
        resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        resourceDesc.Width = sizeInBytes;
        // バッファの場合はこれらは１にする決まり
        resourceDesc.Height = 1;
        resourceDesc.DepthOrArraySize = 1;
        resourceDesc.MipLevels = 1;
        resourceDesc.SampleDesc.Count = 1;
        // バッファの場合はこれにする決まり
        resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

        // リソースを作成
        HRESULT hr = device->CreateCommittedResource(
            &heapProperties,        // ヒープのプロパティ
            D3D12_HEAP_FLAG_NONE,   // ヒープフラグ
            &resourceDesc,          // リソースの記述子
            D3D12_RESOURCE_STATE_GENERIC_READ,           // 初期状態
            nullptr,                // Clear値 (バッファの場合はnullptr)
            IID_PPV_ARGS(&pResource) // ID3D12Resourceポインタを取得
        );

        assert(SUCCEEDED(hr));
        pResource->SetName(L"CreateBufferResource()");

        return pResource; // 作成したリソースを返す
    };

    Microsoft::WRL::ComPtr<ID3D12Resource> CreateConstantBufferResource(
        ID3D12Device* device, size_t sizeInBytes)
    {
        size_t ConstantSize;
        ConstantSize = (sizeInBytes + (D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT - 1)) & ~(D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT - 1);

        return CreateBufferResource(device, ConstantSize);
    };

}