#pragma once
#include <d3d12sdklayers.h> // ID3D12Debug1 用
#include <d3d12.h>          // DirectX 12 API
#pragma comment(lib, "d3d12.lib")
#include <dxgi1_6.h>        // DXGI (SwapChainなど)
#pragma comment(lib, "dxgi.lib")// リンク
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
    Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilBuffer;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap;
    Microsoft::WRL::ComPtr<ID3D12Fence> fence; // フェンスオブジェクト
    UINT64 fenceValue;                        // フェンスの値
    HANDLE fenceEvent;                        // イベントハンドル
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2];
    //Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature = nullptr;
    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
    UINT backBufferIndex;

    void EnablezDebugLayer();
    void InitializeDevice();
    void InitializeCommandQueue();
    void InitializeSwapChain(HWND hwnd, int width, int height);
    void InitializeRenderTargetView();
    void InitializeDepthStencilView(int width, int height);
    void InitializeBarrier();
    void InitializeSynchronizationObjects();
    void InitializeRootSignature();
    void InitializePSO();
};