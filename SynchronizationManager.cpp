#include "SynchronizationManager.h"

SynchronizationManager::SynchronizationManager(ID3D12Device* device) : fenceValue(0)
{
    HRESULT hr = device->CreateFence(
        fenceValue,
        D3D12_FENCE_FLAG_NONE,
        IID_PPV_ARGS(&fence)
    );
    assert(SUCCEEDED(hr));

    fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    assert(fenceEvent != nullptr);
}

SynchronizationManager::~SynchronizationManager()
{
    if (fenceEvent)
    {
        CloseHandle(fenceEvent);
    }
}

void SynchronizationManager::Signal(ID3D12CommandQueue* commandQueue)
{
    fenceValue++;
    HRESULT hr = commandQueue->Signal(fence.Get(), fenceValue);
    assert(SUCCEEDED(hr));
}

void SynchronizationManager::WaitForGPU()
{
    if (fence->GetCompletedValue() < fenceValue)
    {
        HRESULT hr = fence->SetEventOnCompletion(fenceValue, fenceEvent);
        assert(SUCCEEDED(hr));
        WaitForSingleObject(fenceEvent, INFINITE);
    }
}