#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <cassert>
#include <cstdint>

class DescriptorHeapManager
{
public:
    DescriptorHeapManager(ID3D12Device* device);
    ~DescriptorHeapManager();

    ID3D12DescriptorHeap* GetSRVDescriptorHeap() const { return srvDescriptorHeap.Get(); }

private:
    uint32_t descriptorSizeSRV;

    // 
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap;
	
    // 
    D3D12_SHADER_RESOURCE_VIEW_DESC instancingSrvDesc{};
};