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

    ID3D12RootSignature* GetTriangleRootSignature() const { return rootSignatureTriangle.Get(); }
    ID3D12RootSignature* GetLineRootSignature() const { return rootSignatureLine.Get(); }
    ID3D12RootSignature* GetWireframeRootSignature() const { return rootSignatureWireframe.Get(); }
    ID3D12RootSignature* GetGridRootSignature() const { return rootSignatureGrid.Get(); }
    
    ID3D12PipelineState* GetTrianglePipelineState() const { return graphicsPipelineState.Get(); }           // Triangle描画PSO
    ID3D12PipelineState* GetLinePipelineState() const { return graphicsPipelineStateLine.Get(); }           // Line描画PSO
    ID3D12PipelineState* GetWireframePipelineState() const { return graphicsPipelineStateWireframe.Get(); } // ワイヤーフレーム描画PSO 
    ID3D12PipelineState* GetGridPipelineState() const { return graphicsPipelineStateGrid.Get(); }           // グリッド描画PSO

private:
    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignatureTriangle;
    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignatureLine;
    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignatureWireframe;
    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignatureGrid;

    Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState;          // Triangle描画PSO
    Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateLine;      // Line描画用PSO
    Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateWireframe; // ワイヤーフレーム用PSO
    Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateGrid;      // グリッド描画用PSO

    Microsoft::WRL::ComPtr<IDxcUtils> dxcUtils;
    Microsoft::WRL::ComPtr<IDxcCompiler3> dxcCompiler;
    Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler;

    void InitializeDxc();

    void InitializeTriangleRootSignatureInternal(ID3D12Device* device);
    void InitializeLineRootSignatureInternal(ID3D12Device* device);
    void InitializeWireframeRootSignatureInternal(ID3D12Device* device);
    void InitializeGridRootSignatureInternal(ID3D12Device* device);

    void InitializeTrianglePSOInternal(ID3D12Device* device);           // Triangle用
    void InitializeLinePSOInternal(ID3D12Device* device);       // Line用
    void InitializeWireframePSOInternal(ID3D12Device* device);  // ワイヤーフレーム用
    void InitializeGridPSOInternal(ID3D12Device* device);       // グリッド用
};