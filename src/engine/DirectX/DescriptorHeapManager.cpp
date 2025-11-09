#include "DirectX/DescriptorHeapManager.h"
#include "Utilities/functions.h"
#include "SrvManager.h"

DescriptorHeapManager::DescriptorHeapManager(ID3D12Device* device)
{
	srvManager_ = new SrvManager(device);

    Log("コンストラクタ実行成功 : DescriptorHeapManager");
}

DescriptorHeapManager::~DescriptorHeapManager()
{
    Log("デストラクタ実行成功 : DescriptorHeapManager");
}