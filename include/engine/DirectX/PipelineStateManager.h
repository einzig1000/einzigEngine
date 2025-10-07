#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <cassert>
#include <map>
#include "Utilities/functions.h"

class PipelineStateManager
{
public:
    PipelineStateManager(ID3D12Device* device);
    ~PipelineStateManager();

    ID3D12RootSignature* GetRootSignature() const { return rootSignature_object.Get(); }

    ID3D12PipelineState* GetPipelineState(BlendMode mode, D3D12_PRIMITIVE_TOPOLOGY_TYPE type) const;

private:
    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_object;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_particle;
    std::map<BlendMode, Microsoft::WRL::ComPtr<ID3D12PipelineState>> trianglePSOs; // Triangle描画用PSO
    std::map<BlendMode, Microsoft::WRL::ComPtr<ID3D12PipelineState>> linePSOs;     // Line描画用PSO

    Microsoft::WRL::ComPtr<IDxcUtils> dxcUtils;
    Microsoft::WRL::ComPtr<IDxcCompiler3> dxcCompiler;
    Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler;

    void InitializeDxc();
    void InitializeRootSignature(ID3D12Device* device);
    void InitializeRootSignature_object(ID3D12Device* device);
    void InitializeRootSignature_particle(ID3D12Device* device);
    void CreateAllPSOs(ID3D12Device* device);

    // 汎用的なPSO生成関数
    Microsoft::WRL::ComPtr<ID3D12PipelineState> CreatePipelineState(
        ID3D12Device* device,
        const D3D12_BLEND_DESC& blendDesc,
        D3D12_PRIMITIVE_TOPOLOGY_TYPE primitiveType,
        const D3D12_RASTERIZER_DESC& rasterizerDesc,
        const D3D12_INPUT_ELEMENT_DESC* inputElementDescs,
        UINT numInputElements
    );
};