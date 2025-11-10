#include "DirectX/SrvManager.h"
#include "Utilities/functions.h"
#include "SrvManager.h"

SrvManager::SrvManager(ID3D12Device* device)
    :device_(device)
{
	// SRVスロット一つ分のサイズ取得
    descriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// SRV用ディスクリプタヒープ作成
    capacity_ = 512;
    nextIndex_ = 0;
    D3D12_DESCRIPTOR_HEAP_DESC DescriptorHeapDesc{};
	DescriptorHeapDesc.NumDescriptors = capacity_;
    DescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    DescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

    HRESULT hr = device->CreateDescriptorHeap(&DescriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap));
    assert(SUCCEEDED(hr));

    Log("コンストラクタ実行成功 : DescriptorHeapManager");
}

SrvManager::~SrvManager()
{
    Log("デストラクタ実行成功 : DescriptorHeapManager");
}

uint32_t SrvManager::Allocate()
{
    if (nextIndex_ >= capacity_)
    {
        Log("DescriptorHeapが小さいぜ");
        return UINT32_MAX;
    }
    return nextIndex_++;
}

D3D12_CPU_DESCRIPTOR_HANDLE SrvManager::GetCPUHandleAt(uint32_t i) const
{
    D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap.Get()->GetCPUDescriptorHandleForHeapStart();
    handleCPU.ptr += (descriptorSize * i);
    return handleCPU;
}

D3D12_GPU_DESCRIPTOR_HANDLE SrvManager::GetGPUHandleAt(uint32_t i) const
{
    D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap.Get()->GetGPUDescriptorHandleForHeapStart();
    handleGPU.ptr += (descriptorSize * i);
    return handleGPU;
}

SRVAllocation SrvManager::CreateSRV(ID3D12Resource* resource, const D3D12_SHADER_RESOURCE_VIEW_DESC* desc)
{
    // 次スロットのインデックス取得
    uint32_t index = Allocate();

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

SRVAllocation SrvManager::CreateSRVforTexture(ID3D12Resource* resource, DXGI_FORMAT format, UINT mipLevels)
{
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format = format;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = mipLevels;
	return CreateSRV(resource, &srvDesc);
}

SRVAllocation SrvManager::CreateSRVforStructuredBuffer(ID3D12Resource* resource, UINT numElements, UINT structureByteStride)
{
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format = DXGI_FORMAT_UNKNOWN;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
    srvDesc.Buffer.FirstElement = 0;
    srvDesc.Buffer.NumElements = numElements;
    srvDesc.Buffer.StructureByteStride = structureByteStride;
	return CreateSRV(resource, &srvDesc);
}

void SrvManager::CreateSRVforImGui(UINT bufferCount, D3D12_RENDER_TARGET_VIEW_DESC format)
{
    //uint32_t index = Allocate();
    //ImGui_ImplDX12_Init(
    //    device_,
    //    bufferCount,
    //    format,
    //    descriptorHeap,
    //    GetCPUHandleAt(index),                    // ImGuiフォントSRV用のCPUハンドル
    //    GetGPUHandleAt(index)                     // ImGuiフォントSRV用のGPUハンドル
    //);
}
