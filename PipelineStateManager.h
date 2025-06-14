#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <cassert>
#include <dxcapi.h> // for shader compilation
#include "functions.h"

// GraphicsUtility.h (例: CompileShader関数が定義されていると仮定)
// もし CompileShader を PipelineStateManager 内に持たせるなら、この行は不要
// #include "GraphicsUtility.h" 

class PipelineStateManager
{
public:
    PipelineStateManager(ID3D12Device* device);
    ~PipelineStateManager();

    ID3D12RootSignature* GetRootSignature() const { return rootSignature.Get(); }
    ID3D12PipelineState* GetPipelineState() const { return graphicsPipelineState.Get(); }

private:
    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState;

    Microsoft::WRL::ComPtr<IDxcUtils> dxcUtils;
    Microsoft::WRL::ComPtr<IDxcCompiler3> dxcCompiler;
    Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler;

    void InitializeDxc();
    void InitializeRootSignatureInternal(ID3D12Device* device);
    void InitializePSOInternal(ID3D12Device* device);
};