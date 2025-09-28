#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <array>
#include <cassert>
#include "definition/definition.h"


class CommandContextManager
{
public:
    CommandContextManager(ID3D12Device* device);
    ~CommandContextManager();

    ID3D12CommandQueue* GetCommandQueue() const { return commandQueue.Get(); }
    ID3D12GraphicsCommandList* GetCommandList(UINT frameIndex) const { return frameResources[frameIndex].commandList.Get(); }
    ID3D12CommandAllocator* GetCommandAllocator(UINT frameIndex) const { return frameResources[frameIndex].commandAllocator.Get(); }

    void ResetCommandList(UINT frameIndex);

private:
    std::vector<FrameResource> frameResources; // バッファ数分
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue;
    //std::array<Microsoft::WRL::ComPtr<ID3D12CommandAllocator>, kFrameCount> commandAllocator;
    //std::array<Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>, kFrameCount> commandList;
};