#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <cassert>
#include <cstdint>

struct SRVAllocation
{
    uint32_t index = UINT32_MAX;
    D3D12_CPU_DESCRIPTOR_HANDLE cpu{};
    D3D12_GPU_DESCRIPTOR_HANDLE gpu{};
};

class DescriptorHeapManager
{
public:
    DescriptorHeapManager(ID3D12Device* device);
    ~DescriptorHeapManager();

    ID3D12DescriptorHeap* GetSRVDescriptorHeap() const { return srvDescriptorHeap.Get(); }
    uint32_t GetdescriptorSizeSRV() const { return descriptorSizeSRV; }
    uint32_t AllocateSRVSlot();

    D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandleAt(uint32_t index) const;
    D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandleAt(uint32_t index) const;

    SRVAllocation CreateSRV(ID3D12Resource* resource, const D3D12_SHADER_RESOURCE_VIEW_DESC* desc);

private:
    ID3D12Device* device_ = nullptr;

    uint32_t descriptorSizeSRV;

    // 
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap;

    uint32_t srvCapacity_ = 0;
    uint32_t srvNext_ = 0;
};