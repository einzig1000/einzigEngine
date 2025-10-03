#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <cassert>

class DescriptorHeapManager
{
public:
    DescriptorHeapManager(ID3D12Device* device);
    ~DescriptorHeapManager();

    ID3D12DescriptorHeap* GetSRVDescriptorHeap() const { return srvDescriptorHeap.Get(); }

private:
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap;
};