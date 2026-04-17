#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <cassert>
#include <cstdint>
#include "definition/definition.h"


class CBV_SRV_UAVManager
{
public:

	enum class ViewType
    {
        SRV,
        UAV,
		CBV
    };

    struct Allocation
    {
        uint32_t index = UINT32_MAX;
        D3D12_CPU_DESCRIPTOR_HANDLE cpu{};
        D3D12_GPU_DESCRIPTOR_HANDLE gpu{};
		ViewType type;
    };

    CBV_SRV_UAVManager(ID3D12Device* device);
    ~CBV_SRV_UAVManager();

    ID3D12DescriptorHeap* GetSRVDescriptorHeap() const { return descriptorHeap_.Get(); }
    uint32_t GetDescriptorSizeSRV() const { return descriptorSize_; }

    // 空いてるスロットインデックスを取得しnextIndex_をインクリメント
    uint32_t Allocate();

    // Allocate()で取得したスロットインデックスのCPU/GPUハンドルを取得
    D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandleAt(uint32_t index) const;
    D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandleAt(uint32_t index) const;

	// SRVの作成とスロットの割り当て
    Allocation CreateSRV(ID3D12Resource* resource, const D3D12_SHADER_RESOURCE_VIEW_DESC* desc);

    Allocation CreateSRVforTexture(ID3D12Resource* resource, DXGI_FORMAT format, UINT mipLevels);
    Allocation CreateSRVforTextureArray(ID3D12Resource* resource, DXGI_FORMAT format, UINT mipLevels, UINT arraySize);
    Allocation CreateSRVforStructuredBuffer(ID3D12Resource* resource, UINT numElements, UINT structureByteStride);
    void CreateSRVforImGui(UINT bufferCount, D3D12_SHADER_RESOURCE_VIEW_DESC  format);

	Allocation CreateCBV(ID3D12Resource* resource, const D3D12_CONSTANT_BUFFER_VIEW_DESC* desc);


private:

    /// <summary>
    /// バッファリソースを作成する関数
    /// </summary>
    /// <param name="device">DirectX 12 デバイス</param>
    /// <param name="sizeInBytes">バッファのサイズ (バイト単位)</param>
    /// <returns>作成されたバッファリソース</returns>
    Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(ID3D12Device* device, size_t sizeInBytes);

    /// <summary>
    /// 定数バッファリソースを作成する関数(256の倍数になる
    /// </summary>
    /// <param name="device">DirectX 12 デバイス</param>
    /// <param name="sizeInBytes">バッファのサイズ (バイト単位)</param>
    /// <returns>作成された定数バッファリソース</returns>
    Microsoft::WRL::ComPtr<ID3D12Resource> CreateConstantBufferResource(
        ID3D12Device* device, size_t sizeInBytes);



	// デバイス
    ID3D12Device* device_ = nullptr;

    // SRV用のディスクリプタヒープ
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap_;

    // ディスクリプタサイズ
    uint32_t descriptorSize_;
    // 最大スロット数
    uint32_t capacity_ = 0;
    // 次のスロットインデックス
    uint32_t nextIndex_ = 0;
};