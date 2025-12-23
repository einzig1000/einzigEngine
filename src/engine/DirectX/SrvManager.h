#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <cassert>
#include <cstdint>
#include "definition/definition.h"


class SrvManager
{
public:
    SrvManager(ID3D12Device* device);
    ~SrvManager();

    ID3D12DescriptorHeap* GetSRVDescriptorHeap() const { return descriptorHeap.Get(); }
    uint32_t GetdescriptorSizeSRV() const { return descriptorSize; }

	// 空いてるスロットインデックスを取得しnextIndex_をインクリメント
    uint32_t Allocate();

	// Allocate()で取得したスロットインデックスのCPU/GPUハンドルを取得
    D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandleAt(uint32_t index) const;
    D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandleAt(uint32_t index) const;

    SRVAllocation CreateSRV(ID3D12Resource* resource, const D3D12_SHADER_RESOURCE_VIEW_DESC* desc);

    SRVAllocation CreateSRVforTexture(ID3D12Resource* resource, DXGI_FORMAT format, UINT mipLevels);
	SRVAllocation CreateSRVforStructuredBuffer(ID3D12Resource* resource, UINT numElements, UINT structureByteStride);
    void CreateSRVforImGui(UINT bufferCount, D3D12_RENDER_TARGET_VIEW_DESC format);

private:
	void ExpandCapacity();

    ID3D12Device* device_ = nullptr;


	// SRV用のディスクリプタヒープ
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap;

	// ディスクリプタサイズ
    uint32_t descriptorSize;
	// 最大スロット数
    uint32_t capacity_ = 0;
	// 次のスロットインデックス
    uint32_t nextIndex_ = 0;
};

