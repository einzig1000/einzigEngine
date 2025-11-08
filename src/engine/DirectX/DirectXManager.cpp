#include "DirectX/DirectXManager.h"
#include <memory>
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"


DirectXManager::DirectXManager(HWND hwnd)
{
    deviceManager = std::make_unique<DeviceManager>();
    commandContextManager = std::make_unique<CommandContextManager>(deviceManager->GetDevice());
    swapChainManager = std::make_unique<SwapChainManager>(deviceManager->GetDevice(), commandContextManager->GetCommandQueue(), hwnd);
    depthStencilManager = std::make_unique<DepthStencilManager>(deviceManager->GetDevice());
    pipelineStateManager = std::make_unique<PipelineStateManager>(deviceManager->GetDevice());
    descriptorHeapManager = std::make_unique<DescriptorHeapManager>(deviceManager->GetDevice());
    synchronizationManager = std::make_unique<SynchronizationManager>(deviceManager->GetDevice());
    viewportScissorManager = std::make_unique<ViewportScissorManager>();


	resourceManager_ = std::make_unique<ResourceManager>();
    fixFPS_ = std::make_unique<FixFPS>();

    Log("コンストラクタ実行成功 : DirectXManager");
}

DirectXManager::~DirectXManager()
{
    Log("デストラクタ実行成功 : DirectXManager");
}

void DirectXManager::BeginFrame()
{
    // コマンドリストをリセット
    commandContextManager->ResetCommandList();

    // バックバッファのインデックスを更新
    swapChainManager->UpdateBackBufferIndex();
    UINT backBufferIndex = swapChainManager->GetCurrentBackBufferIndex();

    // ResourceStateをPRESENTからRENDER_TARGETへ遷移
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Transition.pResource = swapChainManager->GetCurrentBackBufferResource();
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
    commandContextManager->GetCommandList()->ResourceBarrier(1, &barrier);

    // 描画先のRTVとDSVを指定
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = swapChainManager->GetCurrentRTVHandle();
    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = depthStencilManager->GetDSVHandle();
    commandContextManager->GetCommandList()->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);

    // クリア
    float clearColor[] = { 0.1f,0.1f,0.1f,1.0f };
    commandContextManager->GetCommandList()->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
    commandContextManager->GetCommandList()->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

    // SRV用のディスクリプタヒープを指定
    ID3D12DescriptorHeap* descriptorHeaps[] = { descriptorHeapManager->GetSRVDescriptorHeap() };
    commandContextManager->GetCommandList()->SetDescriptorHeaps(1, descriptorHeaps);

    // ViewportとScissorを設定
    commandContextManager->GetCommandList()->RSSetViewports(1, &viewportScissorManager->GetViewport());
    commandContextManager->GetCommandList()->RSSetScissorRects(1, &viewportScissorManager->GetScissorRect());
}

void DirectXManager::EndFrame()
{
    // ImGui の初期化みたいなもん
    ImDrawData* draw_data = ImGui::GetDrawData();
    if (draw_data != nullptr && draw_data->CmdListsCount > 0)
    {
        ImGui_ImplDX12_RenderDrawData(draw_data, commandContextManager->GetCommandList());
    }

    // ResourceStateをRENDER_TARGETからPRESENTへ遷移
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = swapChainManager->GetCurrentBackBufferResource();
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
    commandContextManager->GetCommandList()->ResourceBarrier(1, &barrier);

    // コマンドリストを確定・実行
    HRESULT hr = commandContextManager->GetCommandList()->Close();
    if (FAILED(hr))
    {
        Log("コマンドリストの確定に失敗"); 
        assert(false);
    }
    ID3D12CommandList* commandLists[] = { commandContextManager->GetCommandList() };
    commandContextManager->GetCommandQueue()->ExecuteCommandLists(1, commandLists);

    // フェンスシグナル
    synchronizationManager->Signal(commandContextManager->GetCommandQueue());

    // スワップチェーンをプレゼンテーション
    swapChainManager->Present();

    // GPU同期
    synchronizationManager->WaitForGPU();

	// FPS制限
    fixFPS_->UpdateFixFPS();
}

void DirectXManager::Resize()
{
    // スワップチェーンのリサイズ
    swapChainManager->Resize(
        GetDevice(),
        GetCommandContextManager()->GetCommandQueue());

	// デプスステンシルバッファのリサイズ
    depthStencilManager->Resize(
        GetDevice());

    // ビューポートとシザー矩形の更新
    viewportScissorManager->Resize();
}