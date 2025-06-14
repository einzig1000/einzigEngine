#include "DescriptorHeapManager.h"

DescriptorHeapManager::DescriptorHeapManager(ID3D12Device* device)
{
    D3D12_DESCRIPTOR_HEAP_DESC DescriptorHeapDesc{};
    DescriptorHeapDesc.NumDescriptors = 128; // 十分な数を確保
    DescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    DescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    HRESULT hr = device->CreateDescriptorHeap(&DescriptorHeapDesc, IID_PPV_ARGS(&srvDescriptorHeap));
    assert(SUCCEEDED(hr));
}

DescriptorHeapManager::~DescriptorHeapManager()
{
    // ComPtrが自動で解放します
}