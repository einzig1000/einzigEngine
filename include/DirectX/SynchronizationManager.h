#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <cassert>
#include <array>
#include "definition/definition.h"

class SynchronizationManager
{
public:
    SynchronizationManager(ID3D12Device* device);
    ~SynchronizationManager();

    void Signal(ID3D12CommandQueue* commandQueue, UINT frameIndex);
    void WaitForGPU(UINT frameIndex);

private:
    Microsoft::WRL::ComPtr<ID3D12Fence> fence;
    HANDLE fenceEvent;
    std::array<UINT64, kFrameCount> fenceValues = {};
};