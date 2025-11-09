#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <cassert>
#include <cstdint>
#include "DirectX/SrvManager.h"

class DescriptorHeapManager
{
public:
    DescriptorHeapManager(ID3D12Device* device);
    ~DescriptorHeapManager();

    SrvManager* GetSrvManager() const { return srvManager_; }


private:
	SrvManager* srvManager_;
};