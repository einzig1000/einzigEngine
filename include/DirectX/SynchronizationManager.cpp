#include "DirectX/SynchronizationManager.h"
#include "Utilities/functions.h"

SynchronizationManager::SynchronizationManager(ID3D12Device* device)
{
    HRESULT hr = device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
    assert(SUCCEEDED(hr));
    fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    assert(fenceEvent != nullptr);
    fenceValues.fill(0);

    Log("コンストラクタ実行成功 : SynchronizationManager");
}

SynchronizationManager::~SynchronizationManager()
{
    if (fenceEvent) CloseHandle(fenceEvent);
    Log("デストラクタ実行成功 : SynchronizationManager");
}

void SynchronizationManager::Signal(ID3D12CommandQueue* commandQueue, UINT frameIndex)
{
    fenceValues[frameIndex]++;
    //commandQueue->Signal(fence.Get(), fenceValues[frameIndex]);
    HRESULT hr = commandQueue->Signal(fence.Get(), fenceValues[frameIndex]);
    assert(SUCCEEDED(hr));
}

void SynchronizationManager::WaitForGPU(UINT frameIndex)
{

    if (fence->GetCompletedValue() < fenceValues[frameIndex])
    {
        //fence->SetEventOnCompletion(fenceValues[frameIndex], fenceEvent);
        //WaitForSingleObject(fenceEvent, INFINITE);

        HRESULT hr = fence->SetEventOnCompletion(fenceValues[frameIndex], fenceEvent);
        assert(SUCCEEDED(hr));
        WaitForSingleObject(fenceEvent, INFINITE);
    }
}