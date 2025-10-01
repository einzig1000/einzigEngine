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
    // 1. バックバッファのインデックス更新
    swapChainManager->UpdateBackBufferIndex();
    UINT frameIndex = swapChainManager->GetCurrentBackBufferIndex();

    // 2. 前回このバッファを使ったGPU処理が終わっているか確認
    synchronizationManager->WaitForGPU(frameIndex);

    // 3. コマンドリストをリセット
    commandContextManager->ResetCommandList(frameIndex);

    //// 5. 描画先のRTVとDSVを設定
    //D3D12_CPU_DESCRIPTOR_HANDLE offscreenRTV = swapChainManager->GetOffscreenCurrentRTVHandle();
    //D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = depthStencilManager->GetDSVHandle();
    //commandContextManager->GetCommandList(frameIndex)->OMSetRenderTargets(1, &offscreenRTV, FALSE, &dsvHandle);
    //
    //// 6. クリア（オフスクリーン + 深度）
    //const float clearColor[4] = { 0.1f, 0.25f, 0.5f, 1.0f };
    //commandContextManager->GetCommandList(frameIndex)->ClearRenderTargetView(offscreenRTV, clearColor, 0, nullptr);
    //commandContextManager->GetCommandList(frameIndex)->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
    //
    //// 7. SRVディスクリプタヒープセット（モデル描画で使う）
    //ID3D12DescriptorHeap* heaps[] = { descriptorHeapManager->GetSRVDescriptorHeap() };
    //commandContextManager->GetCommandList(frameIndex)->SetDescriptorHeaps(1, heaps);
    //
    //// 8. Viewport / Scissor
    //commandContextManager->GetCommandList(frameIndex)->RSSetViewports(1, &viewportScissorManager->GetViewport());
    //commandContextManager->GetCommandList(frameIndex)->RSSetScissorRects(1, &viewportScissorManager->GetScissorRect());
}

ID3D12GraphicsCommandList* DirectXManager::BeginScene(const float clearColor[4])
{
    UINT frameIndex = swapChainManager->GetCurrentBackBufferIndex();
    auto* cmd = commandContextManager->GetCommandList(frameIndex);

    // Offscreen が SRV 状態なら RT に戻す
    {
        D3D12_RESOURCE_BARRIER b{};
        b.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        b.Transition.pResource = swapChainManager->GetOffscreenRenderTarget();
        b.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
        b.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
        b.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        cmd->ResourceBarrier(1, &b);
    }

    D3D12_CPU_DESCRIPTOR_HANDLE offscreenRTV = swapChainManager->GetOffscreenCurrentRTVHandle();
    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = depthStencilManager->GetDSVHandle();
    cmd->OMSetRenderTargets(1, &offscreenRTV, FALSE, &dsvHandle);
    cmd->ClearRenderTargetView(offscreenRTV, clearColor, 0, nullptr);
    cmd->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

    ID3D12DescriptorHeap* heaps[] = { descriptorHeapManager->GetSRVDescriptorHeap() };
    cmd->SetDescriptorHeaps(1, heaps);
    cmd->RSSetViewports(1, &viewportScissorManager->GetViewport());
    cmd->RSSetScissorRects(1, &viewportScissorManager->GetScissorRect());
    return cmd;
}

ID3D12GraphicsCommandList* DirectXManager::BeginPostProcess()
{
    UINT frameIndex = swapChainManager->GetCurrentBackBufferIndex();
    auto* cmd = commandContextManager->GetCommandList(frameIndex);

    // Offscreen: RT -> SRV
    {
        D3D12_RESOURCE_BARRIER b{};
        b.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        b.Transition.pResource = swapChainManager->GetOffscreenRenderTarget();
        b.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
        b.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
        b.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        cmd->ResourceBarrier(1, &b);
    }
    // BackBuffer: PRESENT -> RT
    {
        D3D12_RESOURCE_BARRIER b{};
        b.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        b.Transition.pResource = swapChainManager->GetCurrentBackBufferResource();
        b.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
        b.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
        b.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        cmd->ResourceBarrier(1, &b);
    }

    auto backRTV = swapChainManager->GetCurrentRTVHandle();
    cmd->OMSetRenderTargets(1, &backRTV, FALSE, nullptr);
    return cmd;
}

void DirectXManager::EndFrame(bool drawImGui)
{
    UINT frameIndex = swapChainManager->GetCurrentBackBufferIndex();
    auto* cmd = commandContextManager->GetCommandList(frameIndex);

    if (drawImGui)
    {
        ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), cmd);
    }

    // BackBuffer: RT -> PRESENT
    {
        D3D12_RESOURCE_BARRIER b{};
        b.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        b.Transition.pResource = swapChainManager->GetCurrentBackBufferResource();
        b.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
        b.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
        b.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        cmd->ResourceBarrier(1, &b);
    }

    // 8. コマンドリストClose → 実行 → フェンス → Present
    HRESULT hr = cmd->Close();
    if (FAILED(hr)) { Log("CommandList Close失敗"); assert(false); }

    // 4. コマンドリストをExecute
    ID3D12CommandList* lists[] = { cmd };
    commandContextManager->GetCommandQueue()->ExecuteCommandLists(1, lists);

    // 5. フェンス値をインクリメントしてSignal
    synchronizationManager->Signal(commandContextManager->GetCommandQueue(), frameIndex);

    // 6. Present
    swapChainManager->Present();
}