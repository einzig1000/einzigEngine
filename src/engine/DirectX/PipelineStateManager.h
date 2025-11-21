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
    ID3D12RootSignature* GetRootSignature_particle() const { return rootSignature_particle.Get(); }
	ID3D12RootSignature* GetRootSignature_minecraftMap() const { return rootSignature_minecraftMap.Get(); }

    ID3D12PipelineState* GetPipelineState(BlendMode mode, D3D12_PRIMITIVE_TOPOLOGY_TYPE type) const;
    ID3D12PipelineState* GetParticlePipelineState(BlendMode mode) const;
    ID3D12PipelineState* GetMinecraftMapPipelineState(BlendMode mode) const;

private:
    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_object;
    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_particle;
    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_minecraftMap;
    std::map<BlendMode, Microsoft::WRL::ComPtr<ID3D12PipelineState>> trianglePSOs; // Triangle描画用PSO
    std::map<BlendMode, Microsoft::WRL::ComPtr<ID3D12PipelineState>> particlePSOs; // Particle描画用PSO
    std::map<BlendMode, Microsoft::WRL::ComPtr<ID3D12PipelineState>> linePSOs;     // Line描画用PSO
	std::map<BlendMode, Microsoft::WRL::ComPtr<ID3D12PipelineState>> minecraftMapPSOs; // MinecraftMap用PSO

    Microsoft::WRL::ComPtr<IDxcUtils> dxcUtils;
    Microsoft::WRL::ComPtr<IDxcCompiler3> dxcCompiler;
    Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler;

    std::unordered_map<std::wstring, Microsoft::WRL::ComPtr<IDxcBlob>> shaderCache_;

    void InitializeDxc();
    void InitializeRootSignature(ID3D12Device* device);
    void InitializeRootSignature_object(ID3D12Device* device);
    void InitializeRootSignature_particle(ID3D12Device* device);
	void InitializeRootSignature_minecraftMap(ID3D12Device* device);
    void CreateAllPSOs(ID3D12Device* device);

    // 追加: キャッシュ付きコンパイル
    Microsoft::WRL::ComPtr<IDxcBlob> GetOrCompileShader(const wchar_t* path, const wchar_t* target);

    // 汎用的なPSO生成関数
    Microsoft::WRL::ComPtr<ID3D12PipelineState> CreatePipelineState(
        ID3D12Device* device,
        ID3D12RootSignature* rs,
        const D3D12_BLEND_DESC& blendDesc,
        const D3D12_RASTERIZER_DESC& rasterizerDesc,
        D3D12_PRIMITIVE_TOPOLOGY_TYPE primitiveType,
        const D3D12_INPUT_ELEMENT_DESC* inputElementDescs,
        UINT numInputElements,
        IDxcBlob* vsBlob,
        IDxcBlob* psBlob,
        const D3D12_DEPTH_STENCIL_DESC& depthStencilDesc
    );


    /// <summary>
    /// 指定されたタイプと数のディスクリプタヒープを作成する関数
    /// </summary>
    /// <param name="device">DirectX 12 デバイス</param>
    /// <param name="heapType">ディスクリプタヒープのタイプ</param>
    /// <param name="numDescriptors">ディスクリプタの数</param>
    /// <param name="shaderVisible">シェーダーからアクセス可能かどうか</param>
    /// <returns>作成されたディスクリプタヒープ</returns>
    Microsoft::WRL::ComPtr<IDxcBlob> CompileShader(
        const std::wstring& filePath, 
        const wchar_t* profile);
};