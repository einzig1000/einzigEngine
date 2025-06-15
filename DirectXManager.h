#pragma once
#include <d3d12.h>
#include <wrl.h>

#include "DeviceManager.h"
#include "CommandContextManager.h"
#include "SwapChainManager.h"
#include "DepthStencilManager.h"
#include "PipelineStateManager.h"
#include "DescriptorHeapManager.h"
#include "SynchronizationManager.h"
#include "ViewportScissorManager.h"

#include "GetHitKey.h"
#include "AudioManager.h"
#include "TextureManager.h"

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

    AudioManager* GetAudioManager() const { return audioManager_; }
    TextureManager* GetTextureManager() const { return textureManager_; }

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

    D3D12_RESOURCE_BARRIER barrier = {};

    AudioManager* audioManager_;
    TextureManager* textureManager_;
    GetHitKey* getHitKey_;

    void InitializeGetHitKey(HWND hwnd);
    void InitializeTextureManager(ID3D12Device* device, ID3D12DescriptorHeap* srvDescriptorHeap);
    void InitializeAudioManager();
};