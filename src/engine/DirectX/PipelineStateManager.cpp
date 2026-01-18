#include "DirectX/PipelineStateManager.h"
#include <string>
#include "Utilities/functions.h"
#include <filesystem>

#pragma comment(lib, "dxcompiler.lib")



PipelineStateManager::PipelineStateManager(ID3D12Device* device)
{
    InitializeDxc();
    InitializeRootSignature(device);
    CreateAllPSOs(device);

    Log("コンストラクタ実行成功 : PipelineStateManager");
}

PipelineStateManager::~PipelineStateManager()
{
    Log("デストラクタ実行成功 : PipelineStateManager");
}

void PipelineStateManager::InitializeDxc()
{
    HRESULT hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils));
    assert(SUCCEEDED(hr));
    hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler));
    assert(SUCCEEDED(hr));
    hr = dxcUtils->CreateDefaultIncludeHandler(&includeHandler);
    assert(SUCCEEDED(hr));
}


ID3D12PipelineState* PipelineStateManager::GetTrianglePipelineState(BlendMode mode) const
{
    auto it = trianglePSOs.find(mode);
	return (it != trianglePSOs.end()) ? it->second.Get() : nullptr;
}
ID3D12PipelineState* PipelineStateManager::GetLinePipelineState(BlendMode mode) const
{
    auto it = linePSOs.find(mode);
	return (it != linePSOs.end()) ? it->second.Get() : nullptr;
}
ID3D12PipelineState* PipelineStateManager::GetParticlePipelineState(BlendMode mode) const
{
    auto it = particlePSOs.find(mode);
    return (it != particlePSOs.end()) ? it->second.Get() : nullptr;
}
ID3D12PipelineState* PipelineStateManager::GetBlockPipelineState(BlendMode mode) const
{
    auto it = blockPSOs.find(mode);
	return (it != blockPSOs.end()) ? it->second.Get() : nullptr;
}


void PipelineStateManager::InitializeRootSignature(ID3D12Device* device)
{
    InitializeRootSignature_object(device);
    InitializeRootSignature_particle(device);
    InitializeRootSignature_block(device);
}

void PipelineStateManager::InitializeRootSignature_object(ID3D12Device* device)
{
    HRESULT hr;
    // DescriptorRange for SRV (t0)
    D3D12_DESCRIPTOR_RANGE defaultTexture[1] = {};
    defaultTexture[0].BaseShaderRegister = 0; // t0 レジスタ
    defaultTexture[0].NumDescriptors = 1;
    defaultTexture[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    defaultTexture[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;


    D3D12_ROOT_PARAMETER rootParameters[5]{};

    // ルートパラメータ0: Material (register b0)
    rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; // VS, PS 両方からアクセス可能
    rootParameters[0].Descriptor.ShaderRegister = 0; // b0

    // ルートパラメータ1: TransformationMatrix (WVP, World) (register b1)
    rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; // VS, PS 両方からアクセス可能
    rootParameters[1].Descriptor.ShaderRegister = 1; // b1

    // ルートパラメータ2: Texture (SRV) Descriptor Table (register t0)
    rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PSからのみアクセス
    rootParameters[2].DescriptorTable.pDescriptorRanges = defaultTexture;
    rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(defaultTexture);

    // ルートパラメータ4: DirectionalLight (color, direction, intensity) (register b2)
    rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; // VS, PS 両方からアクセス可能
    rootParameters[3].Descriptor.ShaderRegister = 2; // b2

	// ルートパラメータ5 : Camera (register b3)
	rootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; // VS, PS 両方からアクセス可能
	rootParameters[4].Descriptor.ShaderRegister = 3; // b3

    D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
    staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
    staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
    staticSamplers[0].ShaderRegister = 0; // s0 レジスタ
    staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

    D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
    rootSignatureDesc.pParameters = rootParameters;
    rootSignatureDesc.NumParameters = _countof(rootParameters);
    rootSignatureDesc.pStaticSamplers = staticSamplers;
    rootSignatureDesc.NumStaticSamplers = _countof(staticSamplers);
    rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;


    Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob = nullptr;
    Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
    hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &signatureBlob, &errorBlob);
    if (FAILED(hr))
    {
        if (errorBlob)
        {
            Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
        }
        assert(false);
    }
    hr = device->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature_object));
    assert(SUCCEEDED(hr));
}
void PipelineStateManager::InitializeRootSignature_particle(ID3D12Device * device)
{
    HRESULT hr;
    
    // テクスチャ用(t0, PS) と インスタンス用(t1, VS) の2レンジを別テーブルに
    D3D12_DESCRIPTOR_RANGE rangeTex[1] = {};
    rangeTex[0].BaseShaderRegister = 0; // t0 レジスタ
    rangeTex[0].NumDescriptors = 1;
    rangeTex[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    rangeTex[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

    D3D12_DESCRIPTOR_RANGE rangeInst[1];
    rangeInst[0].BaseShaderRegister = 1; // t1
    rangeInst[0].NumDescriptors = 1;
    rangeInst[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    rangeInst[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
    rangeInst[0].RegisterSpace = 0;


    D3D12_ROOT_PARAMETER rootParameters[4] = {};
    
    // ルートパラメータ0: Material (b0)
    rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
    rootParameters[0].Descriptor.ShaderRegister = 0;
    
    // ルートパラメータ1: Texture [SRV] (register t0)
    rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[1].DescriptorTable.NumDescriptorRanges = _countof(rangeTex);
    rootParameters[1].DescriptorTable.pDescriptorRanges = rangeTex;

    // ルートパラメータ2: WorldMatrix [SRV] (t1)（VS可視）
    rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(rangeInst);
    rootParameters[2].DescriptorTable.pDescriptorRanges = rangeInst;

	// ルートパラメータ3: ViewProjectionMatrix (b1)
	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameters[3].Descriptor.ShaderRegister = 1;

    D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
    staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
    staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
    staticSamplers[0].ShaderRegister = 0; // s0 レジスタ
    staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
    
    D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
    rootSignatureDesc.pParameters = rootParameters;
    rootSignatureDesc.NumParameters = _countof(rootParameters);
    rootSignatureDesc.pStaticSamplers = staticSamplers;
    rootSignatureDesc.NumStaticSamplers = _countof(staticSamplers);
    rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob = nullptr;
    Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
    hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &signatureBlob, &errorBlob);
    if (FAILED(hr))
    {
        if (errorBlob)
        {
            Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
        }
        assert(false);
    }
    hr = device->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature_particle));
    assert(SUCCEEDED(hr));
}
void PipelineStateManager::InitializeRootSignature_block(ID3D12Device* device)
{
    HRESULT hr;

    D3D12_DESCRIPTOR_RANGE defaultTexture[1] = {};
    defaultTexture[0].BaseShaderRegister = 0; // t0
    defaultTexture[0].NumDescriptors = 1;
    defaultTexture[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    defaultTexture[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
    
    D3D12_DESCRIPTOR_RANGE addTexture[1] = {};
    addTexture[0].BaseShaderRegister = 1; // t1
    addTexture[0].NumDescriptors = 1;
    addTexture[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    addTexture[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

    D3D12_DESCRIPTOR_RANGE worldMatrixRange[1] = {};
    worldMatrixRange[0].BaseShaderRegister = 2; // t2
    worldMatrixRange[0].NumDescriptors = 1;
    worldMatrixRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    worldMatrixRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
    worldMatrixRange[0].RegisterSpace = 0;

    D3D12_DESCRIPTOR_RANGE colorRange[1] = {};
    colorRange[0].BaseShaderRegister = 3; // t3
    colorRange[0].NumDescriptors = 1;
    colorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    colorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
    colorRange[0].RegisterSpace = 0;

    D3D12_DESCRIPTOR_RANGE textureArrayIndexRange[1] = {};
    textureArrayIndexRange[0].BaseShaderRegister = 4; // t4
    textureArrayIndexRange[0].NumDescriptors = 1;
    textureArrayIndexRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    textureArrayIndexRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
    textureArrayIndexRange[0].RegisterSpace = 0;



    D3D12_ROOT_PARAMETER rootParameters[6]{};

    // ルートパラメータ0(t3): Color 
    rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; // VS, PS 両方からアクセス可能
	rootParameters[0].DescriptorTable.NumDescriptorRanges = _countof(colorRange);
	rootParameters[0].DescriptorTable.pDescriptorRanges = colorRange;
    
    // ルートパラメータ1(t2): WorldMatrix [SRV]  
    rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
    rootParameters[1].DescriptorTable.NumDescriptorRanges = _countof(worldMatrixRange);
    rootParameters[1].DescriptorTable.pDescriptorRanges = worldMatrixRange;

    // ルートパラメータ2(t0): Texture [SRV]
    rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PSからのみアクセス
    rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(defaultTexture);
    rootParameters[2].DescriptorTable.pDescriptorRanges = defaultTexture;

    // ルートパラメータ3(t1): Texture [SRV]
    rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PSからのみアクセス
    rootParameters[3].DescriptorTable.NumDescriptorRanges = _countof(addTexture);
    rootParameters[3].DescriptorTable.pDescriptorRanges = addTexture;

    // ルートパラメータ3(t4): Break Layer Index (PS)
    rootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
    rootParameters[4].DescriptorTable.NumDescriptorRanges = _countof(textureArrayIndexRange);
    rootParameters[4].DescriptorTable.pDescriptorRanges = textureArrayIndexRange;

    // ルートパラメータ5(b1): ViewProjectionMatrix 
    rootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
    rootParameters[5].Descriptor.ShaderRegister = 1;


    D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
    staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
    staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
    staticSamplers[0].ShaderRegister = 0; // s0 レジスタ
    staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

    D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
    rootSignatureDesc.pParameters = rootParameters;
    rootSignatureDesc.NumParameters = _countof(rootParameters);
    rootSignatureDesc.pStaticSamplers = staticSamplers;
    rootSignatureDesc.NumStaticSamplers = _countof(staticSamplers);
    rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;


    Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob = nullptr;
    Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
    hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &signatureBlob, &errorBlob);
    if (FAILED(hr))
    {
        if (errorBlob)
        {
            Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
        }
        assert(false);
    }
    hr = device->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature_block));
    assert(SUCCEEDED(hr));
}

void PipelineStateManager::CreateAllPSOs(ID3D12Device* device)
{
#pragma region ブレンド設定の定義
 
    // 不透明
    D3D12_BLEND_DESC blendOpaqueDesc{};
    blendOpaqueDesc.AlphaToCoverageEnable = FALSE;
    blendOpaqueDesc.IndependentBlendEnable = FALSE;
    blendOpaqueDesc.RenderTarget[0].BlendEnable = FALSE;
    blendOpaqueDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

    // アルファブレンド（半透明）
    D3D12_BLEND_DESC blendTransparentDesc{};
    blendTransparentDesc.AlphaToCoverageEnable = FALSE;
    blendTransparentDesc.IndependentBlendEnable = FALSE;
    blendTransparentDesc.RenderTarget[0].BlendEnable = TRUE;
    blendTransparentDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
    blendTransparentDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
    blendTransparentDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
    blendTransparentDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
    blendTransparentDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
    blendTransparentDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
    blendTransparentDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

    // 加算合成
    D3D12_BLEND_DESC blendAddDesc{};
    blendAddDesc.AlphaToCoverageEnable = FALSE;
    blendAddDesc.IndependentBlendEnable = FALSE;
    blendAddDesc.RenderTarget[0].BlendEnable = TRUE;
    blendAddDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
    blendAddDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
    blendAddDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
    blendAddDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
    blendAddDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
    blendAddDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
    blendAddDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

    // 減算合成
    D3D12_BLEND_DESC blendSubDesc{};
    blendSubDesc.AlphaToCoverageEnable = FALSE;
    blendSubDesc.IndependentBlendEnable = FALSE;
    blendSubDesc.RenderTarget[0].BlendEnable = TRUE;
    blendSubDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
    blendSubDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
    blendSubDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
    blendSubDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
    blendSubDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
    blendSubDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
    blendSubDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

    // 乗算合成
    D3D12_BLEND_DESC blendMulDesc{};
    blendMulDesc.AlphaToCoverageEnable = FALSE;
    blendMulDesc.IndependentBlendEnable = FALSE;
    blendMulDesc.RenderTarget[0].BlendEnable = TRUE;
    blendMulDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
    blendMulDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
    blendMulDesc.RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;
    blendMulDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
    blendMulDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
    blendMulDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
    blendMulDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

    // スクリーン合成
    D3D12_BLEND_DESC blendScreenDesc{};
    blendScreenDesc.AlphaToCoverageEnable = FALSE;
    blendScreenDesc.IndependentBlendEnable = FALSE;
    blendScreenDesc.RenderTarget[0].BlendEnable = TRUE;
    blendScreenDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
    blendScreenDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
    blendScreenDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
    blendScreenDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
    blendScreenDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
    blendScreenDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
    blendScreenDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
    
#pragma endregion

#pragma region ラスタライザ設定の定義

    D3D12_RASTERIZER_DESC rasterizerSolidDesc{};
    rasterizerSolidDesc.CullMode = D3D12_CULL_MODE_NONE;
    rasterizerSolidDesc.FillMode = D3D12_FILL_MODE_SOLID;

    D3D12_RASTERIZER_DESC rasterizerWireframeDesc{};
    rasterizerWireframeDesc.CullMode = D3D12_CULL_MODE_NONE;
    rasterizerWireframeDesc.FillMode = D3D12_FILL_MODE_WIREFRAME;

    D3D12_RASTERIZER_DESC rasterizerLineDesc{};
    rasterizerLineDesc.CullMode = D3D12_CULL_MODE_NONE;
    rasterizerLineDesc.FillMode = D3D12_FILL_MODE_SOLID;

#pragma endregion

#pragma region 深度設定の定義

    // 深度設定
    D3D12_DEPTH_STENCIL_DESC depthWriteDesc{};      // 通常
    depthWriteDesc.DepthEnable = TRUE;
    depthWriteDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
    depthWriteDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

    D3D12_DEPTH_STENCIL_DESC depthTestOnlyDesc= depthWriteDesc; // パーティクル
    depthTestOnlyDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;

#pragma endregion

#pragma region シェーダーコンパイル

    Microsoft::WRL::ComPtr<IDxcBlob> vsModel = GetOrCompileShader(L"resources/Shaders/Object3D.VS.hlsl", L"vs_6_0");
    Microsoft::WRL::ComPtr<IDxcBlob> psModel = GetOrCompileShader(L"resources/Shaders/Object3D.PS.hlsl", L"ps_6_0");
    Microsoft::WRL::ComPtr<IDxcBlob> vsParticle = GetOrCompileShader(L"resources/Shaders/Particle.VS.hlsl", L"vs_6_0");
    Microsoft::WRL::ComPtr<IDxcBlob> psParticle = GetOrCompileShader(L"resources/Shaders/Particle.PS.hlsl", L"ps_6_0");
    Microsoft::WRL::ComPtr<IDxcBlob> vsLine = GetOrCompileShader(L"resources/Shaders/Line.VS.hlsl", L"vs_6_0");
    Microsoft::WRL::ComPtr<IDxcBlob> psLine = GetOrCompileShader(L"resources/Shaders/Line.PS.hlsl", L"ps_6_0");
    Microsoft::WRL::ComPtr<IDxcBlob> vsBlock = GetOrCompileShader(L"resources/Shaders/Block.VS.hlsl", L"vs_6_0");
    Microsoft::WRL::ComPtr<IDxcBlob> psBlock = GetOrCompileShader(L"resources/Shaders/Block.PS.hlsl", L"ps_6_0");

#pragma endregion

#pragma region 入力レイアウト

    D3D12_INPUT_ELEMENT_DESC triangleInputElementDescs[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,      0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        {"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,   0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
    };
    D3D12_INPUT_ELEMENT_DESC particleInputElementDescs[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
    };
    D3D12_INPUT_ELEMENT_DESC lineInputElementDescs[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
    };
    D3D12_INPUT_ELEMENT_DESC blockInputElementDescs[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,      0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT,      0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        {"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,   0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
	};



#pragma endregion


#pragma region PSOの生成

    // Model
    trianglePSOs[BlendMode::kBlendModeNone] = CreatePipelineState(device, rootSignature_object.Get(), blendOpaqueDesc, rasterizerSolidDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, triangleInputElementDescs, _countof(triangleInputElementDescs), vsModel.Get(), psModel.Get(), depthWriteDesc);
    trianglePSOs[BlendMode::kBlendModeNormal] = CreatePipelineState(device, rootSignature_object.Get(), blendTransparentDesc, rasterizerSolidDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, triangleInputElementDescs, _countof(triangleInputElementDescs), vsModel.Get(), psModel.Get(), depthWriteDesc);
    trianglePSOs[BlendMode::kBlendModeAdd] = CreatePipelineState(device, rootSignature_object.Get(), blendAddDesc, rasterizerSolidDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, triangleInputElementDescs, _countof(triangleInputElementDescs), vsModel.Get(), psModel.Get(), depthWriteDesc);
    trianglePSOs[BlendMode::kBlendModeSub] = CreatePipelineState(device, rootSignature_object.Get(), blendSubDesc, rasterizerSolidDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, triangleInputElementDescs, _countof(triangleInputElementDescs), vsModel.Get(), psModel.Get(), depthWriteDesc);
    trianglePSOs[BlendMode::kBlendModeMul] = CreatePipelineState(device, rootSignature_object.Get(), blendMulDesc, rasterizerSolidDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, triangleInputElementDescs, _countof(triangleInputElementDescs), vsModel.Get(), psModel.Get(), depthWriteDesc);
    trianglePSOs[BlendMode::kBlendModeScreen] = CreatePipelineState(device, rootSignature_object.Get(), blendScreenDesc, rasterizerSolidDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, triangleInputElementDescs, _countof(triangleInputElementDescs), vsModel.Get(), psModel.Get(), depthWriteDesc);
    trianglePSOs[BlendMode::Wireframe] = CreatePipelineState(device, rootSignature_object.Get(), blendOpaqueDesc, rasterizerWireframeDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, triangleInputElementDescs, _countof(triangleInputElementDescs), vsModel.Get(), psModel.Get(), depthWriteDesc);

    // Particle
    particlePSOs[BlendMode::kBlendModeNone] = CreatePipelineState(device, rootSignature_particle.Get(), blendOpaqueDesc, rasterizerSolidDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, particleInputElementDescs, _countof(particleInputElementDescs), vsParticle.Get(), psParticle.Get(), depthTestOnlyDesc);
    particlePSOs[BlendMode::kBlendModeNormal] = CreatePipelineState(device, rootSignature_particle.Get(), blendTransparentDesc, rasterizerSolidDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, particleInputElementDescs, _countof(particleInputElementDescs), vsParticle.Get(), psParticle.Get(), depthTestOnlyDesc);
    particlePSOs[BlendMode::kBlendModeAdd] = CreatePipelineState(device, rootSignature_particle.Get(), blendAddDesc, rasterizerSolidDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, particleInputElementDescs, _countof(particleInputElementDescs), vsParticle.Get(), psParticle.Get(), depthTestOnlyDesc);
    particlePSOs[BlendMode::kBlendModeSub] = CreatePipelineState(device, rootSignature_particle.Get(), blendSubDesc, rasterizerSolidDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, particleInputElementDescs, _countof(particleInputElementDescs), vsParticle.Get(), psParticle.Get(), depthTestOnlyDesc);
    particlePSOs[BlendMode::kBlendModeMul] = CreatePipelineState(device, rootSignature_particle.Get(), blendMulDesc, rasterizerSolidDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, particleInputElementDescs, _countof(particleInputElementDescs), vsParticle.Get(), psParticle.Get(), depthTestOnlyDesc);
    particlePSOs[BlendMode::kBlendModeScreen] = CreatePipelineState(device, rootSignature_particle.Get(), blendScreenDesc, rasterizerSolidDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, particleInputElementDescs, _countof(particleInputElementDescs), vsParticle.Get(), psParticle.Get(), depthTestOnlyDesc);

    // block
    blockPSOs[BlendMode::kBlendModeNone] = CreatePipelineState(device, rootSignature_block.Get(), blendOpaqueDesc, rasterizerSolidDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, blockInputElementDescs, _countof(blockInputElementDescs), vsBlock.Get(), psBlock.Get(), depthWriteDesc);
	blockPSOs[BlendMode::kBlendModeNormal] = CreatePipelineState(device, rootSignature_block.Get(), blendTransparentDesc, rasterizerSolidDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, blockInputElementDescs, _countof(blockInputElementDescs), vsBlock.Get(), psBlock.Get(), depthWriteDesc);
	blockPSOs[BlendMode::kBlendModeAdd] = CreatePipelineState(device, rootSignature_block.Get(), blendAddDesc, rasterizerSolidDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, blockInputElementDescs, _countof(blockInputElementDescs), vsBlock.Get(), psBlock.Get(), depthWriteDesc);
	blockPSOs[BlendMode::kBlendModeSub] = CreatePipelineState(device, rootSignature_block.Get(), blendSubDesc, rasterizerSolidDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, blockInputElementDescs, _countof(blockInputElementDescs), vsBlock.Get(), psBlock.Get(), depthWriteDesc);
	blockPSOs[BlendMode::kBlendModeMul] = CreatePipelineState(device, rootSignature_block.Get(), blendMulDesc, rasterizerSolidDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, blockInputElementDescs, _countof(blockInputElementDescs), vsBlock.Get(), psBlock.Get(), depthWriteDesc);
	blockPSOs[BlendMode::kBlendModeScreen] = CreatePipelineState(device, rootSignature_block.Get(), blendScreenDesc, rasterizerSolidDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, blockInputElementDescs, _countof(blockInputElementDescs), vsBlock.Get(), psBlock.Get(), depthWriteDesc);

    // Line
    linePSOs[BlendMode::kBlendModeNone] = CreatePipelineState(device, rootSignature_object.Get(), blendOpaqueDesc, rasterizerLineDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE, lineInputElementDescs, _countof(lineInputElementDescs), vsLine.Get(), psLine.Get(), depthWriteDesc);
    linePSOs[BlendMode::kBlendModeNormal] = CreatePipelineState(device, rootSignature_object.Get(), blendTransparentDesc, rasterizerLineDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE, lineInputElementDescs, _countof(lineInputElementDescs), vsLine.Get(), psLine.Get(), depthWriteDesc);
    linePSOs[BlendMode::kBlendModeAdd] = CreatePipelineState(device, rootSignature_object.Get(), blendAddDesc, rasterizerLineDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE, lineInputElementDescs, _countof(lineInputElementDescs), vsLine.Get(), psLine.Get(), depthWriteDesc);
    linePSOs[BlendMode::kBlendModeSub] = CreatePipelineState(device, rootSignature_object.Get(), blendSubDesc, rasterizerLineDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE, lineInputElementDescs, _countof(lineInputElementDescs), vsLine.Get(), psLine.Get(), depthWriteDesc);
    linePSOs[BlendMode::kBlendModeMul] = CreatePipelineState(device, rootSignature_object.Get(), blendMulDesc, rasterizerLineDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE, lineInputElementDescs, _countof(lineInputElementDescs), vsLine.Get(), psLine.Get(), depthWriteDesc);
    linePSOs[BlendMode::kBlendModeScreen] = CreatePipelineState(device, rootSignature_object.Get(), blendScreenDesc, rasterizerLineDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE, lineInputElementDescs, _countof(lineInputElementDescs), vsLine.Get(), psLine.Get(), depthWriteDesc);

#pragma endregion

}

Microsoft::WRL::ComPtr<IDxcBlob> PipelineStateManager::GetOrCompileShader(const wchar_t* path, const wchar_t* target)
{
    std::wstring key = std::wstring(path) + L"|" + target;
    auto it = shaderCache_.find(key);
    if (it != shaderCache_.end())
    {
        return it->second;
    }
    Microsoft::WRL::ComPtr<IDxcBlob> blob = CompileShader(path, target);
    assert(blob);
    shaderCache_.emplace(std::move(key), blob);
    return blob;
}

Microsoft::WRL::ComPtr<ID3D12PipelineState> PipelineStateManager::CreatePipelineState(
    ID3D12Device* device, 
    ID3D12RootSignature* rs, 
    const D3D12_BLEND_DESC& blendDesc, 
    const D3D12_RASTERIZER_DESC& rasterizerDesc, 
    D3D12_PRIMITIVE_TOPOLOGY_TYPE primitiveType,
    const D3D12_INPUT_ELEMENT_DESC* inputElementDescs, 
    UINT numInputElements, 
    IDxcBlob* vsBlob, IDxcBlob* psBlob,
    const D3D12_DEPTH_STENCIL_DESC& depthStencilDesc)
{
    HRESULT hr;

                
    D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
    inputLayoutDesc.pInputElementDescs = inputElementDescs;
    inputLayoutDesc.NumElements = numInputElements;

    //D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
    //depthStencilDesc.DepthEnable = depthEnable;
    //depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
    //depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

    D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
    graphicsPipelineStateDesc.pRootSignature = rs;
    graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;
    graphicsPipelineStateDesc.BlendState = blendDesc;
    graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;
    graphicsPipelineStateDesc.VS = { vsBlob->GetBufferPointer(), vsBlob->GetBufferSize() };
    graphicsPipelineStateDesc.PS = { psBlob->GetBufferPointer(), psBlob->GetBufferSize() };
    graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
    graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
    graphicsPipelineStateDesc.NumRenderTargets = 1;
    graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    //graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    graphicsPipelineStateDesc.PrimitiveTopologyType = primitiveType;
    graphicsPipelineStateDesc.SampleDesc.Count = 1;
    graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

    Microsoft::WRL::ComPtr<ID3D12PipelineState> pso;
    hr = device->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&pso));
    assert(SUCCEEDED(hr));
    return pso;
}

Microsoft::WRL::ComPtr<IDxcBlob> PipelineStateManager::CompileShader(
    // CompileするShaderファイルへのパス
    const std::wstring& filePath,
    // Compilerに使用するProfile
    const wchar_t* profile)
{
    ///////////////////////////////////////
    //// 1 hlslファイルを読む
    ///////////////////////////////////////
    /// これからシェーダーをコンパイルする旨をログに出す
    Log(ConvertString(std::format(L"Begin CompileShader, path:{}, profile:{}", filePath, profile)));
    // hlslファイルを読む
    IDxcBlobEncoding* shaderSource = nullptr;
    HRESULT hr = dxcUtils->LoadFile(filePath.c_str(), nullptr, &shaderSource);
    // 読めなかったら停止する
    assert(SUCCEEDED(hr));
    // 読み込んだファイルの内容を設定する
    DxcBuffer shaderSourceBuffer;
    shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
    shaderSourceBuffer.Size = shaderSource->GetBufferSize();
    shaderSourceBuffer.Encoding = DXC_CP_UTF8;

    ///////////////////////////////////////
    //// 2 Compileする
    ///////////////////////////////////////
    LPCWSTR arguments[] = {
        filePath.c_str(),			// コンパイル対象のhlslファイル名
        L"-E", L"main",				// エントリーポイントの指定。基本的にmain
        L"-T", profile,				// ShaderProfileの設定
        L"-Zi", L"-Qembed_debug",	// デバック用の情報を埋め込む
        L"-Od",						// 最適化を外しておく
        L"-Zpr",					// 目盛レイアウトは行優先
    };
    // 実際にシェーダーをコンパイルする
    IDxcResult* shaderResult = nullptr;
    hr = dxcCompiler->Compile(
        &shaderSourceBuffer,	// 読み込んだファイル
        arguments,				// コンパイルオプション
        _countof(arguments),	// コンパイルオプションの数
        includeHandler.Get(),	// includeが含まれた諸々
        IID_PPV_ARGS(&shaderResult)// コンパイル結果
    );
    // コンパイルエラーではなくdxcが起動出来ないなど致命的な状況
    assert(SUCCEEDED(hr));

    ///////////////////////////////////////
    //// 3 警告・エラーが出ていないか確認する
    ///////////////////////////////////////
    /// 警告・エラーが出たらログにだして止める
    IDxcBlobUtf8* shaderError = nullptr;
    IDxcBlobUtf16* outputName = nullptr;
    shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), &outputName);

    if (shaderError != nullptr && shaderError->GetStringLength() != 0)
    {
        Log(shaderError->GetStringPointer());
        assert(false); // コンパイルエラーが発生した場合は停止
    }

    ///////////////////////////////////////
    //// 4 Compile結果を受け取って返す
    ///////////////////////////////////////
    // コンパイル結果から実行用のバイナリ部分を取得
    IDxcBlob* shaderBlob = nullptr;
    //hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
    if (shaderResult->HasOutput(DXC_OUT_OBJECT))
    {
        IDxcBlobWide* dummyOutputName = nullptr;
        hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), &dummyOutputName);
        //hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
    }
    else
    {
        hr = E_FAIL;
        shaderBlob = nullptr;
    }
    assert(SUCCEEDED(hr));
    // 成功したログを出す
    Log(ConvertString(std::format(L"Compile Succeeded. path:{}\n", filePath, profile)));
    // もう使わないリソースを解放
    shaderSource->Release();
    shaderResult->Release();
    // 実行用のバイナリを返却
    return shaderBlob;
}
