#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <cassert>
#include "Utilities/functions.h"

class PipelineStateManager
{
public:
    PipelineStateManager(ID3D12Device* device);
    ~PipelineStateManager();

    ID3D12RootSignature* GetRootSignature() const { return rootSignature.Get(); }
    // Triangle描画PSO
    ID3D12PipelineState* GetPipelineState() const { return graphicsPipelineState.Get(); }
    // Line描画PSO
    ID3D12PipelineState* GetLinePipelineState() const { return graphicsPipelineStateLine.Get(); }
    // ワイヤーフレーム描画PSO 
    ID3D12PipelineState* GetWireframePipelineState() const { return graphicsPipelineStateWireframe.Get(); }
    // グリッド描画PSO
    ID3D12PipelineState* GetGridPipelineState() const { return graphicsPipelineStateGrid.Get(); }

private:
    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateLine; // Line描画用PSO
    Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateWireframe; // ワイヤーフレーム用PSO
    Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateGrid; // グリッド描画用PSO

    Microsoft::WRL::ComPtr<IDxcUtils> dxcUtils;
    Microsoft::WRL::ComPtr<IDxcCompiler3> dxcCompiler;
    Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler;

    void InitializeDxc();
    void InitializeRootSignatureInternal(ID3D12Device* device);
    void InitializePSOInternal(ID3D12Device* device); // オブジェクト描画用
    void InitializeLinePSOInternal(ID3D12Device* device); // Line描画用
    void InitializeWireframePSOInternal(ID3D12Device* device); // ワイヤーフレーム用
    void InitializeGridPSOInternal(ID3D12Device* device); // グリッド用
};