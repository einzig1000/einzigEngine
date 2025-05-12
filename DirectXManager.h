#pragma once
#include <d3d12sdklayers.h> // ID3D12Debug1 用
#include <d3d12.h>          // DirectX 12 API
#pragma comment(lib, "d3d12.lib")
#include <dxgi1_6.h>        // DXGI (SwapChainなど)
#pragma comment(lib, "dxgi.lib")// リンク
#include <wrl.h>            // Microsoft::WRL::ComPtr
#include <cassert>          // assert 用

#include "functions.h"


class DirectXManager {
public:
    DirectXManager(HWND hwnd, int width, int height);
    ~DirectXManager();

    ID3D12Device* GetDevice() const { return device.Get(); }
    ID3D12GraphicsCommandList* GetCommandList() const { return commandList.Get(); }

    void BeginFrame();
    void EndFrame();

private:
    Microsoft::WRL::ComPtr<ID3D12Device> device; // DirectX 12 デバイスオブジェクト。GPUとのやり取りを管理。
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue; // コマンドキュー。GPUにコマンドを送信するためのキュー。
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList; // コマンドリスト。描画やリソース操作のコマンドを記録。
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator; // コマンドアロケータ。コマンドリストのメモリ管理を担当。
    Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain; // スワップチェーン。ダブルバッファリングを管理し、画面の描画を切り替える。
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap; // レンダーターゲットビュー用のディスクリプタヒープ。
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap; // 深度ステンシルビュー用のディスクリプタヒープ。
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap;
    Microsoft::WRL::ComPtr<ID3D12Resource> swapChainResources[2]; // スワップチェーンのバックバッファリソース。
    Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilBuffer; // 深度ステンシルバッファ。深度テストやステンシルテストに使用。
    Microsoft::WRL::ComPtr<ID3D12Fence> fence; // フェンスオブジェクト。GPUの処理完了を同期するために使用。
    UINT64 fenceValue; // フェンスの値。同期の進行状況を追跡。
    HANDLE fenceEvent; // イベントハンドル。フェンスのシグナルを待機するために使用。
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2]; // レンダーターゲットビューのディスクリプタハンドル。
    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature; // ルートシグネチャ。シェーダーとリソースのバインディングを定義。
    D3D12_RESOURCE_BARRIER barrier = {}; // リソースバリア。リソースの状態遷移を管理。
    UINT backBufferIndex; // 現在のバックバッファのインデックス。
    D3D12_VIEWPORT viewport{}; // ビューポート。描画領域を定義。
    D3D12_RECT scissorRect{}; // シザー矩形。描画のクリッピング領域を定義。
    Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState{};// グラフィックスパイプラインステートの設定。

    void EnableDebugLayer();
    void InitializeDevice();
    void InitializeCommandQueue();
    void InitializeSwapChain(HWND hwnd, int width, int height);
    void InitializeRenderTargetView();
    void InitializeDepthStencilView(int width, int height);
    void InitializeSynchronizationObjects();
    void InitializeRootSignature();
    void InitializePSO();
    void InitializeViewportAndScissor(int width, int height);
    void InitializeSRVDescriptorHeap();

};