// DirectXManager.cpp
#include "DirectXManager.h"
#include <memory> // std::unique_ptr 用
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"

// ImGuiのヘッダーをここにインクルードすることもできます
// #include "externals/imgui/imgui_impl_dx12.h"
// #include "externals/imgui/imgui_impl_win32.h"

DirectXManager::DirectXManager(HWND hwnd, int width, int height)
{
    // 各マネージャーの初期化
    // 依存関係を考慮して初期化順序を決定します
    deviceManager = std::make_unique<DeviceManager>();
    commandContextManager = std::make_unique<CommandContextManager>(deviceManager->GetDevice());
    swapChainManager = std::make_unique<SwapChainManager>(deviceManager->GetDevice(), commandContextManager->GetCommandQueue(), hwnd, width, height);
    depthStencilManager = std::make_unique<DepthStencilManager>(deviceManager->GetDevice(), width, height);
    pipelineStateManager = std::make_unique<PipelineStateManager>(deviceManager->GetDevice());
    descriptorHeapManager = std::make_unique<DescriptorHeapManager>(deviceManager->GetDevice());
    synchronizationManager = std::make_unique<SynchronizationManager>(deviceManager->GetDevice());
    viewportScissorManager = std::make_unique<ViewportScissorManager>(width, height);

    // DirectXとは直接関係ない初期化
    InitializeGetHitKey(hwnd);
    InitializeAudioManager();
}

DirectXManager::~DirectXManager()
{
    // 各マネージャーのデストラクタが解放を処理します
    // 生ポインタの解放は忘れずに
    delete audioManager_;
    audioManager_ = nullptr;
    delete getHitKey;
    getHitKey = nullptr;
}

void DirectXManager::InitializeGetHitKey(HWND hwnd)
{
    if (!getHitKey)
    {
        getHitKey = new GetHitKey(hwnd);
    }
}

void DirectXManager::InitializeAudioManager()
{
    audioManager_ = new AudioManager;
}

void DirectXManager::BeginFrame()
{
    getHitKey->Update();

    // バックバッファのインデックスを更新
    swapChainManager->UpdateBackBufferIndex();
    UINT backBufferIndex = swapChainManager->GetCurrentBackBufferIndex();

    // ResourceStateをPRESENTからRENDER_TARGETへ遷移
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Transition.pResource = swapChainManager->GetCurrentBackBufferResource();
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
    commandContextManager->GetCommandList()->ResourceBarrier(1, &barrier);

    // 描画先のRTVとDSVを設定
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = swapChainManager->GetCurrentRTVHandle();
    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = depthStencilManager->GetDSVHandle();
    commandContextManager->GetCommandList()->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);

    // クリア
    float clearColor[] = { 0.1f,0.25f,0.5f,1.0f };
    commandContextManager->GetCommandList()->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
    commandContextManager->GetCommandList()->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

    // ディスクリプタヒープを設定 (SRV用)
    ID3D12DescriptorHeap* descriptorHeaps[] = { descriptorHeapManager->GetSRVDescriptorHeap() };
    commandContextManager->GetCommandList()->SetDescriptorHeaps(1, descriptorHeaps);

    // ViewportとScissorを設定
    commandContextManager->GetCommandList()->RSSetViewports(1, &viewportScissorManager->GetViewport());
    commandContextManager->GetCommandList()->RSSetScissorRects(1, &viewportScissorManager->GetScissorRect());

    // RootSignatureとPSOを設定
    commandContextManager->GetCommandList()->SetGraphicsRootSignature(pipelineStateManager->GetRootSignature());
    commandContextManager->GetCommandList()->SetPipelineState(pipelineStateManager->GetPipelineState());
}

void DirectXManager::EndFrame()
{
    // ImGui 描画 (実際のコマンドはここに積まれる)
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandContextManager->GetCommandList());

    // ResourceStateをRENDER_TARGETからPRESENTへ遷移
    UINT backBufferIndex = swapChainManager->GetCurrentBackBufferIndex();
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Transition.pResource = swapChainManager->GetCurrentBackBufferResource();
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
    commandContextManager->GetCommandList()->ResourceBarrier(1, &barrier);

    // コマンドリストを確定・実行
    HRESULT hr = commandContextManager->GetCommandList()->Close();
    if (FAILED(hr))
    {
        // Log("Failed to close command list."); // Log関数が定義されていることを想定
        assert(false);
    }
    ID3D12CommandList* commandLists[] = { commandContextManager->GetCommandList() };
    commandContextManager->GetCommandQueue()->ExecuteCommandLists(1, commandLists);

    // スワップチェーンをプレゼンテーション
    swapChainManager->Present();

    // GPU同期
    synchronizationManager->Signal(commandContextManager->GetCommandQueue());
    synchronizationManager->WaitForGPU();

    // コマンドリストをリセット
    commandContextManager->ResetCommandList();
}