#pragma once
#include <d3d12.h>
#include <wrl.h>

// 各マネージャークラスのインクルード
#include "DeviceManager.h"
#include "CommandContextManager.h"
#include "SwapChainManager.h"
#include "DepthStencilManager.h"
#include "PipelineStateManager.h"
#include "DescriptorHeapManager.h"
#include "SynchronizationManager.h"
#include "ViewportScissorManager.h"

// その他の外部依存クラス
#include "GetHitKey.h"
#include "AudioManager.h"

class DirectXManager
{
public:
    DirectXManager(HWND hwnd, int width, int height);
    ~DirectXManager();

    // 各マネージャーへのアクセスを提供（必要に応じて）
    ID3D12Device* GetDevice() const { return deviceManager->GetDevice(); }
    ID3D12GraphicsCommandList* GetCommandList() const { return commandContextManager->GetCommandList(); }
    ID3D12DescriptorHeap* GetsrvDescriptorHeap() const { return descriptorHeapManager->GetSRVDescriptorHeap(); }
    AudioManager* GetAudioManager() const { return audioManager_; }
    const DXGI_SWAP_CHAIN_DESC1& GetSwapChainDesc() const { return swapChainManager->GetSwapChainDesc(); };
    const D3D12_RENDER_TARGET_VIEW_DESC& GetRtvDesc() const { return swapChainManager->GetRtvDesc(); }
    

    void BeginFrame();
    void EndFrame();

private:
    // 各マネージャーのインスタンス
    std::unique_ptr<DeviceManager> deviceManager;
    std::unique_ptr<CommandContextManager> commandContextManager;
    std::unique_ptr<SwapChainManager> swapChainManager;
    std::unique_ptr<DepthStencilManager> depthStencilManager;
    std::unique_ptr<PipelineStateManager> pipelineStateManager;
    std::unique_ptr<DescriptorHeapManager> descriptorHeapManager;
    std::unique_ptr<SynchronizationManager> synchronizationManager;
    std::unique_ptr<ViewportScissorManager> viewportScissorManager;

    // その他の外部依存クラス
    AudioManager* audioManager_; // DirectXとは直接関係ないため、別途管理
    GetHitKey* getHitKey;       // 同上

    // フレームごとのバリア情報
    D3D12_RESOURCE_BARRIER barrier = {};

    void InitializeGetHitKey(HWND hwnd);
    void InitializeAudioManager();
};