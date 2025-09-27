#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <cassert>

class SwapChainManager
{
public:
    SwapChainManager(ID3D12Device* device, ID3D12CommandQueue* commandQueue, HWND hwnd, int width, int height);
    ~SwapChainManager();

    UINT GetCurrentBackBufferIndex() const { return backBufferIndex; }
    ID3D12Resource* GetCurrentBackBufferResource() const { return swapChainResources[backBufferIndex].Get(); }
    D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentRTVHandle() const { return rtvHandles[backBufferIndex]; }
    ID3D12DescriptorHeap* GetRTVDescriptorHeap() const { return rtvDescriptorHeap.Get(); }
    const DXGI_SWAP_CHAIN_DESC1& GetSwapChainDesc() const { return swapChainDesc; }
    const D3D12_RENDER_TARGET_VIEW_DESC& GetRtvDesc() const { return rtvDesc; }


    // 生成後の取得用（生ポインタを返す）
    ID3D12Resource* GetOffscreenRenderTarget() const { return offscreenRenderTarget.Get(); }
    ID3D12DescriptorHeap* GetOffscreenRTVDescriptorHeap() const { return offscreenRTVHeap.Get(); }
    ID3D12DescriptorHeap* GetOffscreenSRVDescriptorHeap() const { return offscreenSRVHeap.Get(); }

    // 生成時のCreateDescriptorHeap用
    ID3D12Resource** GetOffscreenRenderTargetAddress() { return offscreenRenderTarget.GetAddressOf(); }
    ID3D12DescriptorHeap** GetOffscreenSRVDescriptorHeapAddress() { return offscreenSRVHeap.GetAddressOf(); }
    ID3D12DescriptorHeap** GetOffscreenRTVDescriptorHeapAddress() { return offscreenRTVHeap.GetAddressOf(); }

    // オフスクリーンレンダーターゲットの取得
    D3D12_CPU_DESCRIPTOR_HANDLE& GetOffscreenCurrentRTVHandle() { return offscreenRTVHandle; }

    void Present();
    void UpdateBackBufferIndex();

private:
    Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain;
    Microsoft::WRL::ComPtr<ID3D12Resource> swapChainResources[2];
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap;
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2];
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc;
    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc;
    UINT backBufferIndex;

    // オフスクリーンレンダリング
    Microsoft::WRL::ComPtr<ID3D12Resource> offscreenRenderTarget;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> offscreenRTVHeap;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> offscreenSRVHeap;
    D3D12_CPU_DESCRIPTOR_HANDLE offscreenRTVHandle;

    void InitializeSwapChainInternal(ID3D12Device* device, ID3D12CommandQueue* commandQueue, HWND hwnd, int width, int height);
    void InitializeOffscreenResources(ID3D12Device* device, int width, int height);
    void InitializeRenderTargetView(ID3D12Device* device);
};