#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <cassert>
#include <cstdint>
#include <memory>

class SrvManager;
class CBVManager;
class RTVManager;
class DSVManager;

class DescriptorHeapManager
{
public:
    DescriptorHeapManager(ID3D12Device* device);
    ~DescriptorHeapManager();

    SRVManager* GetSrvManager() const { return srvManager_.get(); }
	CBVManager* GetCbvManager() const { return cbvManager_.get(); }
	RTVManager* GetRtvManager() const { return rtvManager_.get(); }
	DSVManager* GetDsvManager() const { return dsvManager_.get(); }

private:
    std::unique_ptr<SRVManager> srvManager_;
	std::unique_ptr<CBVManager> cbvManager_;
	std::unique_ptr<RTVManager> rtvManager_;
	std::unique_ptr<DSVManager> dsvManager_;
};