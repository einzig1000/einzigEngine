#include "DirectX/DescriptorHeapManager/CBV_SRV_UAV/CBV_SRV_UAVManager.h"
#include "Utilities/functions.h"
#include "Utilities/Logger/Logger.h"

CBV_SRV_UAVManager::CBV_SRV_UAVManager(ID3D12Device* device)
    :device_(device)
{
    // SRVスロット一つ分のサイズ取得
    descriptorSize_ = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    // SRV用ディスクリプタヒープ作成
	capacity_ = 1024;
    nextIndex_ = 0;
    D3D12_DESCRIPTOR_HEAP_DESC DescriptorHeapDesc{};
    DescriptorHeapDesc.NumDescriptors = capacity_;
    DescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    DescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    HRESULT hr = device->CreateDescriptorHeap(&DescriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap_));
    assert(SUCCEEDED(hr));

    Log("コンストラクタ実行成功 : DescriptorHeapManager");
}

CBV_SRV_UAVManager::~CBV_SRV_UAVManager()
{
    Log("デストラクタ実行成功 : DescriptorHeapManager");
}

uint32_t CBV_SRV_UAVManager::Allocate()
{
    if (nextIndex_ >= capacity_)
    {
		Log("SRVのスロットが足りません。容量を増やしてください。");
		assert(false);
        return UINT32_MAX;
    }
    return nextIndex_++;
}

D3D12_CPU_DESCRIPTOR_HANDLE CBV_SRV_UAVManager::GetCPUHandleAt(uint32_t i) const
{
    D3D12_CPU_DESCRIPTOR_HANDLE handle = descriptorHeap_.Get()->GetCPUDescriptorHandleForHeapStart();
    handle.ptr += (descriptorSize_ * i);
    return handle;
}

D3D12_GPU_DESCRIPTOR_HANDLE CBV_SRV_UAVManager::GetGPUHandleAt(uint32_t i) const
{
    D3D12_GPU_DESCRIPTOR_HANDLE handle = descriptorHeap_.Get()->GetGPUDescriptorHandleForHeapStart();
    handle.ptr += (descriptorSize_ * i);
    return handle;
}

CBV_SRV_UAVManager::Allocation CBV_SRV_UAVManager::CreateSRV(ID3D12Resource* resource, const D3D12_SHADER_RESOURCE_VIEW_DESC* desc)
{
    // 次スロットのインデックス取得
    uint32_t index = Allocate();

	// ハンドル取得
    D3D12_CPU_DESCRIPTOR_HANDLE cpu = GetCPUHandleAt(index);
    D3D12_GPU_DESCRIPTOR_HANDLE gpu = GetGPUHandleAt(index);

    // SRV作成
    device_->CreateShaderResourceView(resource, desc, cpu);

	return Allocation{ index, cpu, gpu };
}

CBV_SRV_UAVManager::Allocation CBV_SRV_UAVManager::CreateSRVforTexture(ID3D12Resource* resource, DXGI_FORMAT format, UINT mipLevels)
{
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format = format;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = mipLevels;
    return CreateSRV(resource, &srvDesc);
}

CBV_SRV_UAVManager::Allocation CBV_SRV_UAVManager::CreateSRVforTextureArray(ID3D12Resource* resource, DXGI_FORMAT format, UINT mipLevels, UINT arraySize)
{
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format = format;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Texture2DArray.MostDetailedMip = 0;
    srvDesc.Texture2DArray.MipLevels = mipLevels;
    srvDesc.Texture2DArray.FirstArraySlice = 0;
    srvDesc.Texture2DArray.ArraySize = arraySize;
    srvDesc.Texture2DArray.PlaneSlice = 0;
    srvDesc.Texture2DArray.ResourceMinLODClamp = 0.0f;

    return CreateSRV(resource, &srvDesc);
}

CBV_SRV_UAVManager::Allocation CBV_SRV_UAVManager::CreateSRVforStructuredBuffer(ID3D12Resource* resource, UINT numElements, UINT structureByteStride)
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

void CBV_SRV_UAVManager::CreateSRVforImGui(UINT bufferCount, D3D12_SHADER_RESOURCE_VIEW_DESC format)
{
    uint32_t index = Allocate();
    ImGui_ImplDX12_Init(
        device_,
        bufferCount,
        format,
        descriptorHeap_,
        GetCPUHandleAt(index),                    // ImGuiフォントSRV用のCPUハンドル
        GetGPUHandleAt(index)                     // ImGuiフォントSRV用のGPUハンドル
    );
}


