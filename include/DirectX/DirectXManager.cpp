#include "DirectX/DirectXManager.h"
#include <memory>
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"


DirectXManager::DirectXManager(HWND hwnd, int width, int height)
{
    deviceManager = std::make_unique<DeviceManager>();
    commandContextManager = std::make_unique<CommandContextManager>(deviceManager->GetDevice());
    swapChainManager = std::make_unique<SwapChainManager>(deviceManager->GetDevice(), commandContextManager->GetCommandQueue(), hwnd, width, height);
    depthStencilManager = std::make_unique<DepthStencilManager>(deviceManager->GetDevice(), width, height);
    pipelineStateManager = std::make_unique<PipelineStateManager>(deviceManager->GetDevice());
    descriptorHeapManager = std::make_unique<DescriptorHeapManager>(deviceManager->GetDevice());
    synchronizationManager = std::make_unique<SynchronizationManager>(deviceManager->GetDevice());
    viewportScissorManager = std::make_unique<ViewportScissorManager>(width, height);

    audioManager_ = std::make_unique<AudioManager>();
    textureManager_ = std::make_unique<TextureManager>(GetDevice(), GetsrvDescriptorHeap());

    Log("コンストラクタ実行成功 : DirectXManager");
}

DirectXManager::~DirectXManager()
{
    Log("デストラクタ実行成功 : DirectXManager");
}

void DirectXManager::BeginFrame()
{
    // 1. バックバッファのインデックスを更新
    swapChainManager->UpdateBackBufferIndex();
    UINT frameIndex = swapChainManager->GetCurrentBackBufferIndex();

    // 2. 前回このバッファを使ったGPU処理が終わっているか確認
    synchronizationManager->WaitForGPU(frameIndex);

    // 3. コマンドリストをリセット
    commandContextManager->ResetCommandList(frameIndex);

    // 4. ResourceStateをPRESENTからRENDER_TARGETへ遷移
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = swapChainManager->GetCurrentBackBufferResource();
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
    commandContextManager->GetCommandList(frameIndex)->ResourceBarrier(1, &barrier);

    // 5. 描画先のRTVとDSVを設定
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = swapChainManager->GetCurrentRTVHandle();
    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = depthStencilManager->GetDSVHandle();
    commandContextManager->GetCommandList(frameIndex)->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);

    // 6. クリア
    float clearColor[] = { 0.1f, 0.25f, 0.5f, 1.0f };
    commandContextManager->GetCommandList(frameIndex)->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
    commandContextManager->GetCommandList(frameIndex)->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

    // 7. ディスクリプタヒープを設定 (SRV用)
    ID3D12DescriptorHeap* descriptorHeaps[] = { descriptorHeapManager->GetSRVDescriptorHeap() };
    commandContextManager->GetCommandList(frameIndex)->SetDescriptorHeaps(1, descriptorHeaps);

    // 8. ViewportとScissorを設定
    commandContextManager->GetCommandList(frameIndex)->RSSetViewports(1, &viewportScissorManager->GetViewport());
    commandContextManager->GetCommandList(frameIndex)->RSSetScissorRects(1, &viewportScissorManager->GetScissorRect());
}

void DirectXManager::EndFrame()
{
    UINT frameIndex = swapChainManager->GetCurrentBackBufferIndex();

    // 1. ImGui の初期化みたいなもん
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandContextManager->GetCommandList(frameIndex));

    // 2. ResourceStateをRENDER_TARGETからPRESENTへ遷移
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = swapChainManager->GetCurrentBackBufferResource();
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
    commandContextManager->GetCommandList(frameIndex)->ResourceBarrier(1, &barrier);

    // 3. コマンドリストをClose
    HRESULT hr = commandContextManager->GetCommandList(frameIndex)->Close();
    if (FAILED(hr))
    {
        Log("コマンドリストの確定・実行に失敗しました");
        assert(false);
    }

    // 4. コマンドリストをExecute
    ID3D12CommandList* commandLists[] = { commandContextManager->GetCommandList(frameIndex) };
    commandContextManager->GetCommandQueue()->ExecuteCommandLists(1, commandLists);

    // 5. フェンス値をインクリメントしてSignal
    synchronizationManager->Signal(commandContextManager->GetCommandQueue(), frameIndex);

    // 6. Present
    swapChainManager->Present();

    //// 5. GPU同期
    //synchronizationManager->Signal(commandContextManager->GetCommandQueue(), frameIndex);
    //synchronizationManager->WaitForGPU(frameIndex);
}