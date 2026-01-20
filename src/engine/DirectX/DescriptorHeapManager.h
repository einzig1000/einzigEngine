#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <cassert>
#include <cstdint>
#include <memory>
#include "DirectX/SrvManager.h"

class DescriptorHeapManager
{
public:
    DescriptorHeapManager(ID3D12Device* device);
    ~DescriptorHeapManager();

    SrvManager* GetSrvManager() const { return srvManager_.get(); }


private:
    std::unique_ptr<SrvManager> srvManager_;
};