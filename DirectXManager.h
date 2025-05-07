#pragma once
#include <d3d12.h>          // DirectX 12 API
#include <dxgi1_6.h>        // DXGI (SwapChainなど)
#include <wrl.h>            // Microsoft::WRL::ComPtr
#include <cassert>          // assert 用

//#include "functions.h"


class DirectXManager {
public:
    DirectXManager(HWND hwnd, int width, int height);
    ~DirectXManager();

    ID3D12Device* GetDevice() const { return device.Get(); }
    ID3D12GraphicsCommandList* GetCommandList() const { return commandList.Get(); }

    void BeginFrame();
    void EndFrame();

private:
    Microsoft::WRL::ComPtr<ID3D12Device> device;
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
    Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap;
    Microsoft::WRL::ComPtr<ID3D12Resource> swapChainResources[2];
    UINT backBufferIndex;

    void InitializeDevice();
    void InitializeCommandQueue();
    void InitializeSwapChain(HWND hwnd, int width, int height);
    void InitializeRenderTargetView();
};