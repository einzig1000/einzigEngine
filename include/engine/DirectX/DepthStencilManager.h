#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <cassert>

class DepthStencilManager
{
public:
    DepthStencilManager(ID3D12Device* device);
    ~DepthStencilManager();

    D3D12_CPU_DESCRIPTOR_HANDLE GetDSVHandle() const { return dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(); }
    ID3D12DescriptorHeap* GetDSVDescriptorHeap() const { return dsvDescriptorHeap.Get(); } 

    void Resize(ID3D12Device* device);

private:
    Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilBuffer;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap;
};