#include "DirectX/SwapChainManager.h"
#include "Utilities/functions.h"
#include "Window/WindowManager.h"
#include <Windows.h> 

SwapChainManager::SwapChainManager(ID3D12Device* device, ID3D12CommandQueue* commandQueue, HWND hwnd)
{
    InitializeSwapChainInternal(device, commandQueue, hwnd);
    InitializeRenderTargetView(device);
    backBufferIndex = 0;

    Log("コンストラクタ実行成功 : SwapChainManager");
}

SwapChainManager::~SwapChainManager()
{
    // ここで

	Log("デストラクタ実行成功 : SwapChainManager");
}

void SwapChainManager::InitializeSwapChainInternal(ID3D12Device* device, ID3D12CommandQueue* commandQueue, HWND hwnd)
{
    Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory;
    HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory));
    assert(SUCCEEDED(hr));

    swapChainDesc.Width = UINT(WindowManager::winWidth_);
    swapChainDesc.Height = UINT(WindowManager::winHeight_);
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    //swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = 2;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

    Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain1;
    hr = dxgiFactory->CreateSwapChainForHwnd(
        commandQueue, hwnd, &swapChainDesc, nullptr, nullptr,
        swapChain1.ReleaseAndGetAddressOf());
    assert(SUCCEEDED(hr));

    // IDXGISwapChain4 へ昇格
    hr = swapChain1.As(&swapChain);
    assert(SUCCEEDED(hr));

    hr = swapChain->GetBuffer(0, IID_PPV_ARGS(&swapChainResources[0]));
    assert(SUCCEEDED(hr));
    hr = swapChain->GetBuffer(1, IID_PPV_ARGS(&swapChainResources[1]));
    assert(SUCCEEDED(hr));
}

void SwapChainManager::InitializeRenderTargetView(ID3D12Device* device)
{
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
    rtvHeapDesc.NumDescriptors = 2;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    HRESULT hr = device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvDescriptorHeap));
    assert(SUCCEEDED(hr));

    rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    //rtvDesc.Format = swapChainDesc.Format;
    rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

    D3D12_CPU_DESCRIPTOR_HANDLE rtvStartHandle = rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
    rtvHandles[0] = rtvStartHandle;
    device->CreateRenderTargetView(swapChainResources[0].Get(), &rtvDesc, rtvHandles[0]);
    rtvHandles[1].ptr = rtvHandles[0].ptr + device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    device->CreateRenderTargetView(swapChainResources[1].Get(), &rtvDesc, rtvHandles[1]);
}

void SwapChainManager::Present()
{
    swapChain->Present(1, 0);
}

void SwapChainManager::UpdateBackBufferIndex()
{
    backBufferIndex = swapChain->GetCurrentBackBufferIndex();
}

void SwapChainManager::Resize(ID3D12Device* device, ID3D12CommandQueue* commandQueue)
{
    if (WindowManager::winWidth_ == 0 || WindowManager::winHeight_ == 0) return;

    // 全GPU作業の完了待ち（厳密）
    Microsoft::WRL::ComPtr<ID3D12Fence> fence;
    HRESULT hr = device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
    assert(SUCCEEDED(hr));
    HANDLE evt = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    assert(evt != nullptr);
    UINT64 value = 1;
    hr = commandQueue->Signal(fence.Get(), value);
    assert(SUCCEEDED(hr));
    hr = fence->SetEventOnCompletion(value, evt);
    assert(SUCCEEDED(hr));
    WaitForSingleObject(evt, INFINITE);
    CloseHandle(evt);

    // バックバッファの参照を解放
    swapChainResources[0].Reset();
    swapChainResources[1].Reset();

    hr = swapChain->ResizeBuffers(
        swapChainDesc.BufferCount,
        UINT(WindowManager::winWidth_),
        UINT(WindowManager::winHeight_),
        swapChainDesc.Format,
        0);
    assert(SUCCEEDED(hr));

    swapChainDesc.Width = UINT(WindowManager::winWidth_);
    swapChainDesc.Height = UINT(WindowManager::winHeight_);

    // 新しいバックバッファの取得とRTV再作成
    hr = swapChain->GetBuffer(0, IID_PPV_ARGS(&swapChainResources[0]));
    assert(SUCCEEDED(hr));
    hr = swapChain->GetBuffer(1, IID_PPV_ARGS(&swapChainResources[1]));
    assert(SUCCEEDED(hr));

    D3D12_CPU_DESCRIPTOR_HANDLE start = rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
    rtvHandles[0] = start;
    device->CreateRenderTargetView(swapChainResources[0].Get(), &rtvDesc, rtvHandles[0]);
    SIZE_T inc = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    rtvHandles[1].ptr = start.ptr + inc;
    device->CreateRenderTargetView(swapChainResources[1].Get(), &rtvDesc, rtvHandles[1]);

    backBufferIndex = swapChain->GetCurrentBackBufferIndex();
}