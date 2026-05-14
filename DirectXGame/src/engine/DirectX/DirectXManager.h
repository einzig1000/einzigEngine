#pragma once
#include <d3d12.h>
#include <memory>

#include <DirectX/DeviceManager.h>
#include <DirectX/CommandContextManager.h>
#include <DirectX/SwapChainManager.h>
#include <DirectX/Pipeline/PipelineStateManager/PipelineStateManager.h>
#include <DirectX/DescriptorHeapManager/DescriptorHeapManager.h>
#include <DirectX/SynchronizationManager.h>
#include <DirectX/ViewportScissorManager.h>

/// <summary>
/// DirectX管理クラス
/// </summary>
class DirectXManager
{
public:
    DirectXManager(HWND hwnd);
    ~DirectXManager();

    ID3D12Device* GetDevice() const { return deviceManager->GetDevice(); }
    CommandContextManager* GetCommandContextManager() const { return commandContextManager.get(); }
    DescriptorHeapManager* GetDescriptorHeapManager() const { return descriptorHeapManager.get(); }
    SwapChainManager* GetSwapChain() const { return swapChainManager.get(); };
    PipelineStateManager* GetPipelineStateManager() const { return pipelineStateManager.get(); }
    SynchronizationManager* GetSynchronizationManager() const { return synchronizationManager.get(); }

    void BeginFrame();
    void EndFrame();
    void Resize();

private:
    std::unique_ptr<SwapChainManager> swapChainManager;
    std::unique_ptr<DeviceManager> deviceManager;
    std::unique_ptr<CommandContextManager> commandContextManager;
    std::unique_ptr<PipelineStateManager> pipelineStateManager;
    std::unique_ptr<DescriptorHeapManager> descriptorHeapManager;
    std::unique_ptr<SynchronizationManager> synchronizationManager;
    std::unique_ptr<ViewportScissorManager> viewportScissorManager;
};