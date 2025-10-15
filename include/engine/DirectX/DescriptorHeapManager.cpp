#include "DirectX/DescriptorHeapManager.h"
#include "Utilities/functions.h"

DescriptorHeapManager::DescriptorHeapManager(ID3D12Device* device)
{
	descriptorSizeSRV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    D3D12_DESCRIPTOR_HEAP_DESC DescriptorHeapDesc{};
    DescriptorHeapDesc.NumDescriptors = 128;
    DescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    DescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    HRESULT hr = device->CreateDescriptorHeap(&DescriptorHeapDesc, IID_PPV_ARGS(&srvDescriptorHeap));
    assert(SUCCEEDED(hr));

    //instancingSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
    //instancingSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    //instancingSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
    //instancingSrvDesc.Buffer.FirstElement = 0;
    //instancingSrvDesc.Buffer.NumElements = knumInstance;
    //instancingSrvDesc.Buffer.StructureByteStride = sizeof(TransformationMatrix);
    //D3D12_CPU_DESCRIPTOR_HANDLE instancingSrvHandleCPU = GetCPUDescriptorHandle(srvDescriptorHeap.Get(), descriptorSizeSRV, 3);
    //D3D12_GPU_DESCRIPTOR_HANDLE instancingSrvHandleGPU = GetGPUDescriptorHandle(srvDescriptorHeap.Get(), descriptorSizeSRV, 3);
    //device->CreateShaderResourceView(instancingResource.Get(), &instancingSrvDesc, instancingSrvHandleCPU);





    Log("コンストラクタ実行成功 : DescriptorHeapManager");
}

DescriptorHeapManager::~DescriptorHeapManager()
{
    Log("デストラクタ実行成功 : DescriptorHeapManager");
}