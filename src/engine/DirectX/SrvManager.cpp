#include "DirectX/SrvManager.h"
#include "Utilities/functions.h"
#include "SrvManager.h"

SrvManager::SrvManager(ID3D12Device* device)
    :device_(device)
{
	// SRVスロット一つ分のサイズ取得
    descriptorSize_ = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	capacity_ = 16384;
    nextIndex_ = 0;

    // CPU-only staging heap
    {
        D3D12_DESCRIPTOR_HEAP_DESC desc{};
        desc.NumDescriptors = capacity_;
        desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

        device_->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&stagingHeap));
    }

    // GPU-visible heap
    {
        D3D12_DESCRIPTOR_HEAP_DESC desc{};
        desc.NumDescriptors = capacity_;
        desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

        device_->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&gpuHeap));
    }

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
        Log("DescriptorHeapが小さいぜ\n");
        ExpandCapacity();
		Log("拡張に成功したぜ\n");
    }
    return nextIndex_++;
}

D3D12_CPU_DESCRIPTOR_HANDLE SrvManager::GetGPUHeapCPUHandleAt(uint32_t index) const
{
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = gpuHeap->GetCPUDescriptorHandleForHeapStart();
    handleCPU.ptr += (descriptorSize_ * index);
	return handleCPU;
}

D3D12_GPU_DESCRIPTOR_HANDLE SrvManager::GetGPUHeapGPUHandleAt(uint32_t index) const
{
    D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = gpuHeap->GetGPUDescriptorHandleForHeapStart();
    handleGPU.ptr += (descriptorSize_ * index);
	return handleGPU;
}

D3D12_CPU_DESCRIPTOR_HANDLE SrvManager::GetStagingHeapCPUHandleAt(uint32_t index) const
{
    D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = stagingHeap->GetCPUDescriptorHandleForHeapStart();
    handleCPU.ptr += (descriptorSize_ * index);
	return handleCPU;
}

D3D12_GPU_DESCRIPTOR_HANDLE SrvManager::GetStagingHeapGPUHandleAt(uint32_t index) const
{
    D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = stagingHeap->GetGPUDescriptorHandleForHeapStart();
    handleGPU.ptr += (descriptorSize_ * index);
	return handleGPU;
}

SRVAllocation SrvManager::CreateSRV(ID3D12Resource* resource, const D3D12_SHADER_RESOURCE_VIEW_DESC* desc)
{
    // 次スロットのインデックス取得
    uint32_t index = Allocate();

	// CPU/GPUハンドル計算
    D3D12_CPU_DESCRIPTOR_HANDLE cpuStaging = stagingHeap->GetCPUDescriptorHandleForHeapStart();
    cpuStaging.ptr += index * descriptorSize_;

    // SRV を stagingHeap に作成
    device_->CreateShaderResourceView(resource, desc, cpuStaging);

    // staging → gpuHeap にコピー
    D3D12_CPU_DESCRIPTOR_HANDLE cpuGPU = gpuHeap->GetCPUDescriptorHandleForHeapStart();
    cpuGPU.ptr += index * descriptorSize_;
    device_->CopyDescriptorsSimple(
        1,
        cpuGPU,
        cpuStaging,
        D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
    );

    SRVAllocation alloc{};
    alloc.index = index;
    alloc.cpu = cpuStaging;
    alloc.gpu = gpuHeap->GetGPUDescriptorHandleForHeapStart();
    alloc.gpu.ptr += index * descriptorSize_;

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
	srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
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

void SrvManager::ExpandCapacity()
{
    uint32_t newCapacity = capacity_ * 2;

    // 新 staging heap
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> newStaging;
    {
        D3D12_DESCRIPTOR_HEAP_DESC desc{};
        desc.NumDescriptors = newCapacity;
        desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        device_->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&newStaging));
    }

    // 新 GPU heap
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> newGPU;
    {
        D3D12_DESCRIPTOR_HEAP_DESC desc{};
        desc.NumDescriptors = newCapacity;
        desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        device_->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&newGPU));
    }

    // staging → newStaging にコピー
    device_->CopyDescriptorsSimple(
        nextIndex_,
        newStaging->GetCPUDescriptorHandleForHeapStart(),
        stagingHeap->GetCPUDescriptorHandleForHeapStart(),
        D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
    );

    // newStaging → newGPU にコピー
    device_->CopyDescriptorsSimple(
        nextIndex_,
        newGPU->GetCPUDescriptorHandleForHeapStart(),
        newStaging->GetCPUDescriptorHandleForHeapStart(),
        D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
    );

    stagingHeap = newStaging;
    gpuHeap = newGPU;
    capacity_ = newCapacity;

}

