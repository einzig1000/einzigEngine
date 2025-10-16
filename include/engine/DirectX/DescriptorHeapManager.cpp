#include "DirectX/DescriptorHeapManager.h"
#include "Utilities/functions.h"

DescriptorHeapManager::DescriptorHeapManager(ID3D12Device* device)
{
    device_ = device;
	descriptorSizeSRV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    D3D12_DESCRIPTOR_HEAP_DESC DescriptorHeapDesc{};
    DescriptorHeapDesc.NumDescriptors = 512;
    DescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    DescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

    HRESULT hr = device->CreateDescriptorHeap(&DescriptorHeapDesc, IID_PPV_ARGS(&srvDescriptorHeap));
    assert(SUCCEEDED(hr));

    srvCapacity_ = srvDescriptorHeap->GetDesc().NumDescriptors;
    srvNext_ = 0;


    Log("コンストラクタ実行成功 : DescriptorHeapManager");
}

DescriptorHeapManager::~DescriptorHeapManager()
{
    Log("デストラクタ実行成功 : DescriptorHeapManager");
}

uint32_t DescriptorHeapManager::AllocateSRVSlot()
{
    if (srvNext_ >= srvCapacity_)
    {
        Log("DescriptorHeapが小さいぜ");
		return UINT32_MAX;
    }
    return srvNext_++;
}

D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeapManager::GetCPUHandleAt(uint32_t i) const
{
    return GetCPUDescriptorHandle(srvDescriptorHeap.Get(), descriptorSizeSRV, i);
}

D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeapManager::GetGPUHandleAt(uint32_t i) const
{
    return GetGPUDescriptorHandle(srvDescriptorHeap.Get(), descriptorSizeSRV, i);
}

SRVAllocation DescriptorHeapManager::CreateSRV(ID3D12Resource* resource, const D3D12_SHADER_RESOURCE_VIEW_DESC* desc)
{
	// 次スロットのインデックス取得
    uint32_t index = AllocateSRVSlot();

    D3D12_CPU_DESCRIPTOR_HANDLE cpu = GetCPUHandleAt(index);
    D3D12_GPU_DESCRIPTOR_HANDLE gpu = GetGPUHandleAt(index);

    // SRV作成
    device_->CreateShaderResourceView(resource, desc, cpu);

    SRVAllocation alloc{};
    alloc.index = index;
    alloc.cpu = cpu;
    alloc.gpu = gpu;
    return alloc;
}