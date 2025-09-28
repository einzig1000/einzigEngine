#include "DirectX/CommandContextManager.h"
#include "Utilities/functions.h"

CommandContextManager::CommandContextManager(ID3D12Device* device)
{
    frameResources.resize(kFrameCount);

    D3D12_COMMAND_QUEUE_DESC commandQueueDesc = {};
    commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

    HRESULT hr = device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue));
    assert(SUCCEEDED(hr));

    for (UINT i = 0; i < kFrameCount; ++i)
    {
        hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&frameResources[i].commandAllocator));
        assert(SUCCEEDED(hr));

        hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, frameResources[i].commandAllocator.Get(), nullptr, IID_PPV_ARGS(&frameResources[i].commandList));
        assert(SUCCEEDED(hr));
        // コマンドリストは初期状態でOpenなので一度Closeしておく
        frameResources[i].commandList->Close();
        frameResources[i].fenceValue = 0;
    }

    Log("コンストラクタ実行成功 : CommandContextManager");
}

CommandContextManager::~CommandContextManager()
{
    Log("デストラクタ実行成功 : CommandContextManager");
}

void CommandContextManager::ResetCommandList(UINT frameIndex)
{
    HRESULT hr = frameResources[frameIndex].commandAllocator->Reset();
    assert(SUCCEEDED(hr));
    hr = frameResources[frameIndex].commandList->Reset(frameResources[frameIndex].commandAllocator.Get(), nullptr);
    assert(SUCCEEDED(hr));
}