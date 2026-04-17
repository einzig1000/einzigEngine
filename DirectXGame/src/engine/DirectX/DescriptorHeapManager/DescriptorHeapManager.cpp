#include "DirectX/DescriptorHeapManager/DescriptorHeapManager.h"
#include "DirectX/DescriptorHeapManager/SRV/SRVManager.h"
#include "DirectX/DescriptorHeapManager/CBV/CBVManager.h"
#include "DirectX/DescriptorHeapManager/RTV/RTVManager.h"
#include "DirectX/DescriptorHeapManager/DSV/DSVManager.h"
#include "Utilities/Logger/Logger.h"

DescriptorHeapManager::DescriptorHeapManager(ID3D12Device* device)
{
    srvManager_ = std::make_unique<SRVManager>(device);
	cbvManager_ = std::make_unique<CBVManager>(device);
	rtvManager_ = std::make_unique<RTVManager>(device);
	dsvManager_ = std::make_unique<DSVManager>(device);

    Log("コンストラクタ実行成功 : DescriptorHeapManager");
}

DescriptorHeapManager::~DescriptorHeapManager()
{
    Log("デストラクタ実行成功 : DescriptorHeapManager");
}