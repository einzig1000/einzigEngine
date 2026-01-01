#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <cassert>

class SwapChainManager
{
public:
    SwapChainManager(ID3D12Device* device, ID3D12CommandQueue* commandQueue, HWND hwnd);
    ~SwapChainManager();

    UINT GetCurrentBackBufferIndex() const { return backBufferIndex; }
    ID3D12Resource* GetCurrentBackBufferResource() const { return swapChainResources[backBufferIndex].Get(); }
    D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentRTVHandle() const { return rtvHandles[backBufferIndex]; }
    ID3D12DescriptorHeap* GetRTVDescriptorHeap() const { return rtvDescriptorHeap.Get(); }
    const DXGI_SWAP_CHAIN_DESC1& GetSwapChainDesc() const { return swapChainDesc; }
    const D3D12_RENDER_TARGET_VIEW_DESC& GetRtvDesc() const { return rtvDesc; }

    void Present();
    void UpdateBackBufferIndex();

    void Resize(ID3D12Device* device, ID3D12CommandQueue* commandQueue);

private:
    Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain;
    Microsoft::WRL::ComPtr<ID3D12Resource> swapChainResources[2];
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap;
	Microsoft::WRL::ComPtr<ID3D12Device> device_;
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2];
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc;
    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc;
    UINT backBufferIndex;

    void InitializeSwapChainInternal(ID3D12Device* device, ID3D12CommandQueue* commandQueue, HWND hwnd);
    void InitializeRenderTargetView(ID3D12Device* device);
};