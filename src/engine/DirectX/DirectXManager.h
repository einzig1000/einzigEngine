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
#include "FixFPS/FixFPS.h"

#include "Resource/ResourceManager.h"
#include "input/Input.h"
#include <memory>

class DirectXManager
{
public:
    DirectXManager(HWND hwnd);
    ~DirectXManager();

    ID3D12Device* GetDevice() const { return deviceManager->GetDevice(); }
	CommandContextManager* GetCommandContextManager() const { return commandContextManager; }
    DescriptorHeapManager* GetDescriptorHeapManager() const { return descriptorHeapManager; }
    SwapChainManager* GetSwapChain() const { return swapChainManager; };
    PipelineStateManager* GetPipelineStateManager() const { return pipelineStateManager; }
	SynchronizationManager* GetSynchronizationManager() const { return synchronizationManager; }

	ResourceManager* GetResourceManager() const { return resourceManager_; }

	FixFPS* GetFixFPS() const { return fixFPS_; }

    void BeginFrame();
    void EndFrame();
	void Resize();

private:
    SwapChainManager* swapChainManager;
    DeviceManager* deviceManager;
    CommandContextManager* commandContextManager;
    DepthStencilManager* depthStencilManager;
    PipelineStateManager* pipelineStateManager;
    DescriptorHeapManager* descriptorHeapManager;
    SynchronizationManager* synchronizationManager;
    ViewportScissorManager* viewportScissorManager;

    ResourceManager* resourceManager_;
    FixFPS* fixFPS_;
};