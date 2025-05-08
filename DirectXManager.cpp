#include "DirectXManager.h" // クラス定義
#include <vector>           // std::vector (リソース管理用)


DirectXManager::DirectXManager(HWND hwnd, int width, int height) {
#ifdef _DEBUG
    EnablezDebugLayer();
#endif
    InitializeDevice();
    InitializeCommandQueue();
    InitializeSwapChain(hwnd, width, height);
    InitializeRenderTargetView();
}

DirectXManager::~DirectXManager() {
    // リソース解放処理
}

void DirectXManager::EnablezDebugLayer()
{
    ID3D12Debug1* debugController = nullptr;
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
    {
        // デバッグレイヤーを有効化する
        debugController->EnableDebugLayer();
        // さらにＧＰＵ側でもチェックを行うようにする
        debugController->SetEnableGPUBasedValidation(TRUE);
    }
}

void DirectXManager::InitializeDevice() {
    // DXGIFactoryの生成
    Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory;
    // エラーチェック
    HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory));
    assert(SUCCEEDED(hr));
    
    // 使用するアダプタ(GPU)を決定する
    Microsoft::WRL::ComPtr<IDXGIAdapter4> useAdapter;
    // 良い順にアダプタを頼む
    for (UINT i = 0; dxgiFactory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter)) != DXGI_ERROR_NOT_FOUND; ++i) {
        DXGI_ADAPTER_DESC3 adapterDesc;
        hr = useAdapter->GetDesc3(&adapterDesc);
        assert(SUCCEEDED(hr)); //エラーチェック
		// ソフトウェアアダプタでなければ採用
        if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
            break;
        }
    }
    //エラーチェック
    hr = D3D12CreateDevice(useAdapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&device));
    assert(SUCCEEDED(hr));
}

void DirectXManager::InitializeCommandQueue() {
    D3D12_COMMAND_QUEUE_DESC commandQueueDesc = {};
    commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

    // CommandQueueの設定
    HRESULT hr = device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue));
    assert(SUCCEEDED(hr));

    // CommandAllocatorの設定
    hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator));
    assert(SUCCEEDED(hr));

    // CommandListの設定
    hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator.Get(), nullptr, IID_PPV_ARGS(&commandList));
    assert(SUCCEEDED(hr));
}

void DirectXManager::InitializeSwapChain(HWND hwnd, int width, int height) {
    Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory;
    HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory));
    assert(SUCCEEDED(hr));

    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    // 画面の幅。ウィンドウのクライアント領域を同じものにしておく
    swapChainDesc.Width = width;
    // 画面の高さ。ウィンドウのクライアント領域を同じものにしておく
    swapChainDesc.Height = height;
    // 色の形式
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    // マルチサンプルしない
    swapChainDesc.SampleDesc.Count = 1;
    // ダブルバッファ
    swapChainDesc.BufferCount = 2;
    // 描画のターゲットとして利用する
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    // モニタにうつしたら、中身を破棄
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

    Microsoft::WRL::ComPtr<IDXGISwapChain1> tempSwapChain;
    hr = dxgiFactory->CreateSwapChainForHwnd(commandQueue.Get(), hwnd, &swapChainDesc, nullptr, nullptr, &tempSwapChain);
    assert(SUCCEEDED(hr));

    hr = tempSwapChain.As(&swapChain);
    assert(SUCCEEDED(hr));



    // コマンドキュー、ウィンドウハンドル、設定を渡して生成する
    hr = dxgiFactory->CreateSwapChainForHwnd(commandQueue, hwnd, &swapChainDesc, nullptr, nullptr, reinterpret_cast<IDXGISwapChain1**>(&swapChain));
    assert(SUCCEEDED(hr));
}

void DirectXManager::InitializeRenderTargetView() {
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
    rtvHeapDesc.NumDescriptors = 2;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    HRESULT hr = device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvDescriptorHeap));
    assert(SUCCEEDED(hr));

    for (UINT i = 0; i < 2; ++i) {
        hr = swapChain->GetBuffer(i, IID_PPV_ARGS(&swapChainResources[i]));
        assert(SUCCEEDED(hr));

        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
        rtvHandle.ptr += i * device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        device->CreateRenderTargetView(swapChainResources[i].Get(), nullptr, rtvHandle);
    }
}

void DirectXManager::BeginFrame() {
    backBufferIndex = swapChain->GetCurrentBackBufferIndex();
    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Transition.pResource = swapChainResources[backBufferIndex].Get();
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
    commandList->ResourceBarrier(1, &barrier);
}

void DirectXManager::EndFrame() {
    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Transition.pResource = swapChainResources[backBufferIndex].Get();
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
    commandList->ResourceBarrier(1, &barrier);

    commandList->Close();
    ID3D12CommandList* commandLists[] = { commandList.Get() };
    commandQueue->ExecuteCommandLists(1, commandLists);

    swapChain->Present(1, 0);
    commandAllocator->Reset();
    commandList->Reset(commandAllocator.Get(), nullptr);
}