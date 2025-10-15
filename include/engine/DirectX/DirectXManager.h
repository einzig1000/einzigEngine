#pragma once
#include <d3d12.h>
#include <wrl.h>

#include "DeviceManager.h"
#include "DirectX/CommandContextManager.h"
#include "DirectX/SwapChainManager.h"
#include "DirectX/DepthStencilManager.h"
#include "DirectX/PipelineStateManager.h"
#include "DirectX/DescriptorHeapManager.h"
#include "DirectX/SynchronizationManager.h"
#include "DirectX/ViewportScissorManager.h"

#include "Resource/ResourceManager.h"
#include "input/Input.h"
#include <memory>

class DirectXManager
{
public:
    DirectXManager(HWND hwnd, int width, int height);
    ~DirectXManager();

    ID3D12Device* GetDevice() const { return deviceManager->GetDevice(); }
    ID3D12GraphicsCommandList* GetCommandList() const { return commandContextManager->GetCommandList(); }
    ID3D12DescriptorHeap* GetsrvDescriptorHeap() const { return descriptorHeapManager->GetSRVDescriptorHeap(); }
    const DXGI_SWAP_CHAIN_DESC1& GetSwapChainDesc() const { return swapChainManager->GetSwapChainDesc(); };
    const D3D12_RENDER_TARGET_VIEW_DESC& GetRtvDesc() const { return swapChainManager->GetRtvDesc(); }
    PipelineStateManager* GetPipelineStateManager() const { return pipelineStateManager.get(); }

	ResourceManager* GetResourceManager() const { return resourceManager_.get(); }

    void BeginFrame();
    void EndFrame();

private:
    std::unique_ptr<DeviceManager> deviceManager;
    std::unique_ptr<CommandContextManager> commandContextManager;
    std::unique_ptr<SwapChainManager> swapChainManager;
    std::unique_ptr<DepthStencilManager> depthStencilManager;
    std::unique_ptr<PipelineStateManager> pipelineStateManager;
    std::unique_ptr<DescriptorHeapManager> descriptorHeapManager;
    std::unique_ptr<SynchronizationManager> synchronizationManager;
    std::unique_ptr<ViewportScissorManager> viewportScissorManager;

    std::unique_ptr<ResourceManager> resourceManager_;

    D3D12_RESOURCE_BARRIER barrier = {};

};