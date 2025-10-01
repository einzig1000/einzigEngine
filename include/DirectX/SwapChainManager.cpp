#include "DirectX/SwapChainManager.h"
#include "Utilities/functions.h"

SwapChainManager::SwapChainManager(ID3D12Device* device, ID3D12CommandQueue* commandQueue, HWND hwnd, int width, int height)
{
    InitializeSwapChainInternal(device, commandQueue, hwnd, width, height);
    InitializeOffscreenResources(device, width, height);
    InitializeRenderTargetView(device);
    backBufferIndex = 0;

    Log("コンストラクタ実行成功 : SwapChainManager");
}

SwapChainManager::~SwapChainManager()
{
    Log("デストラクタ実行成功 : SwapChainManager");
}

void SwapChainManager::InitializeSwapChainInternal(ID3D12Device* device, ID3D12CommandQueue* commandQueue, HWND hwnd, int width, int height)
{
    Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory;
    HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory));
    assert(SUCCEEDED(hr));

    swapChainDesc.Width = width;
    swapChainDesc.Height = height;
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = 2;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

    hr = dxgiFactory->CreateSwapChainForHwnd(
        commandQueue,
        hwnd,
        &swapChainDesc,
        nullptr,
        nullptr,
        reinterpret_cast<IDXGISwapChain1**>(swapChain.GetAddressOf())
    );
    assert(SUCCEEDED(hr));

    hr = swapChain->GetBuffer(0, IID_PPV_ARGS(&swapChainResources[0]));
    assert(SUCCEEDED(hr));
    hr = swapChain->GetBuffer(1, IID_PPV_ARGS(&swapChainResources[1]));
    assert(SUCCEEDED(hr));
}

void SwapChainManager::InitializeOffscreenResources(ID3D12Device* device, int width, int height)
{
    // オフスクリーン用レンダーターゲット作成
    D3D12_RESOURCE_DESC desc = {};
    desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    desc.Width = width;
    desc.Height = height;
    desc.DepthOrArraySize = 1;
    desc.MipLevels = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
    desc.SampleDesc.Count = 1;
    desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

    D3D12_HEAP_PROPERTIES heapProps = {};
    heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;

    D3D12_CLEAR_VALUE clearValue = {};
    clearValue.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    clearValue.Color[0] = 0.1f;
    clearValue.Color[1] = 0.25f;
    clearValue.Color[2] = 0.5f;
    clearValue.Color[3] = 1.0f;

    HRESULT hr = device->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_RENDER_TARGET,
        &clearValue,
        IID_PPV_ARGS(&offscreenRenderTarget)
    );
    assert(SUCCEEDED(hr));

    // SRVディスクリプタヒープ作成
    D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
    srvHeapDesc.NumDescriptors = 1;
    srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    hr = device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&offscreenSRVHeap));
    assert(SUCCEEDED(hr));

    // SRV作成
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;

    device->CreateShaderResourceView(
        offscreenRenderTarget.Get(),
        &srvDesc,
        offscreenSRVHeap->GetCPUDescriptorHandleForHeapStart()
    );

    // オフスクリーン用RTVディスクリプタヒープ作成
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
    rtvHeapDesc.NumDescriptors = 1;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    hr = device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&offscreenRTVHeap));
    assert(SUCCEEDED(hr));

    // ハンドル取得
    offscreenRTVHandle = offscreenRTVHeap->GetCPUDescriptorHandleForHeapStart();

    // RTV作成
    device->CreateRenderTargetView(offscreenRenderTarget.Get(), nullptr, offscreenRTVHandle);
}

void SwapChainManager::InitializeRenderTargetView(ID3D12Device* device)
{
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
    rtvHeapDesc.NumDescriptors = 2;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    HRESULT hr = device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvDescriptorHeap));
    assert(SUCCEEDED(hr));

    // オフスクリーンレンダリングの時は
    // DXGI_FORMAT_R8G8B8A8_UNORMにする
    //rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

    rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
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