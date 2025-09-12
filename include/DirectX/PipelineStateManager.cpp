#include "DirectX/PipelineStateManager.h"
#include <string>
#include "Utilities/functions.h"

#pragma comment(lib, "dxcompiler.lib")



PipelineStateManager::PipelineStateManager(ID3D12Device* device)
{
    InitializeDxc();
    InitializeRootSignatureInternal(device);
    CreateAllPSOs(device);

    Log("コンストラクタ実行成功 : PipelineStateManager");
}

PipelineStateManager::~PipelineStateManager()
{
    Log("デストラクタ実行成功 : PipelineStateManager");
}

ID3D12PipelineState* PipelineStateManager::GetPipelineState(BlendMode mode, D3D12_PRIMITIVE_TOPOLOGY_TYPE type) const
{
    if (type == D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE)
    {
        auto it = trianglePSOs.find(mode);
        if (it != trianglePSOs.end())
        {
            return it->second.Get();
        }
    }
    else if (type == D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE)
    {
        auto it = linePSOs.find(mode);
        if (it != linePSOs.end())
        {
            return it->second.Get();
        }
    }
    return nullptr;
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

void PipelineStateManager::InitializeRootSignatureInternal(ID3D12Device* device)
{
    HRESULT hr;
    // DescriptorRange for SRV (t0)
    D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
    descriptorRange[0].BaseShaderRegister = 0; // t0 レジスタ
    descriptorRange[0].NumDescriptors = 1;
    descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;


    D3D12_ROOT_PARAMETER rootParameters[4] = {};

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
    rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange;
    rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);

    // ルートパラメータ3: DirectionalLight (color, direction, intensity) (register b2)
    rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; // VS, PS 両方からアクセス可能
    rootParameters[3].Descriptor.ShaderRegister = 2; // b2


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
    hr = device->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
    assert(SUCCEEDED(hr));
}

void PipelineStateManager::CreateAllPSOs(ID3D12Device* device)
{
    // ----------------------------------------------------
    // ブレンド設定の定義
    // ----------------------------------------------------
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

    // ----------------------------------------------------
    // ラスタライザ設定の定義
    // ----------------------------------------------------
    D3D12_RASTERIZER_DESC rasterizerSolidDesc{};
    rasterizerSolidDesc.CullMode = D3D12_CULL_MODE_BACK;
    rasterizerSolidDesc.FillMode = D3D12_FILL_MODE_SOLID;

    D3D12_RASTERIZER_DESC rasterizerWireframeDesc{};
    rasterizerWireframeDesc.CullMode = D3D12_CULL_MODE_BACK;
    rasterizerWireframeDesc.FillMode = D3D12_FILL_MODE_WIREFRAME;

    D3D12_RASTERIZER_DESC rasterizerLineDesc{};
    rasterizerLineDesc.CullMode = D3D12_CULL_MODE_NONE;
    rasterizerLineDesc.FillMode = D3D12_FILL_MODE_SOLID;


    // ----------------------------------------------------
    // PSOの生成
    // ----------------------------------------------------
    trianglePSOs[BlendMode::kBlendModeNone] = CreatePipelineState(device, blendOpaqueDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, rasterizerSolidDesc);
    trianglePSOs[BlendMode::kBlendModeNormal] = CreatePipelineState(device, blendTransparentDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, rasterizerSolidDesc);
    trianglePSOs[BlendMode::kBlendModeAdd] = CreatePipelineState(device, blendAddDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, rasterizerSolidDesc);
    trianglePSOs[BlendMode::kBlendModeSub] = CreatePipelineState(device, blendSubDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, rasterizerSolidDesc);
    trianglePSOs[BlendMode::kBlendModeMul] = CreatePipelineState(device, blendMulDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, rasterizerSolidDesc);
    trianglePSOs[BlendMode::kBlendModeScreen] = CreatePipelineState(device, blendScreenDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, rasterizerSolidDesc);

    linePSOs[BlendMode::kBlendModeNone] = CreatePipelineState(device, blendOpaqueDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE, rasterizerLineDesc);
    linePSOs[BlendMode::kBlendModeNormal] = CreatePipelineState(device, blendTransparentDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE, rasterizerLineDesc);
    linePSOs[BlendMode::kBlendModeAdd] = CreatePipelineState(device, blendAddDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE, rasterizerLineDesc);
    linePSOs[BlendMode::kBlendModeSub] = CreatePipelineState(device, blendSubDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE, rasterizerLineDesc);
    linePSOs[BlendMode::kBlendModeMul] = CreatePipelineState(device, blendMulDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE, rasterizerLineDesc);
    linePSOs[BlendMode::kBlendModeScreen] = CreatePipelineState(device, blendScreenDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE, rasterizerLineDesc);

    // ワイヤーフレーム用は通常ブレンドのみ
    trianglePSOs[BlendMode::Wireframe] = CreatePipelineState(device, blendOpaqueDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, rasterizerWireframeDesc);
}

Microsoft::WRL::ComPtr<ID3D12PipelineState> PipelineStateManager::CreatePipelineState(ID3D12Device* device, const D3D12_BLEND_DESC& blendDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE primitiveType, const D3D12_RASTERIZER_DESC& rasterizerDesc)
{
    HRESULT hr;

    Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob;
    Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob;
    if (primitiveType == D3D12_PRIMITIVE_TOPOLOGY_TYPE::D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE)
    {
        pixelShaderBlob = CompileShader(L"include/Shaders/Object3D.PS.hlsl", L"ps_6_0", dxcUtils.Get(), dxcCompiler.Get(), includeHandler.Get());
        vertexShaderBlob = CompileShader(L"include/Shaders/Object3D.VS.hlsl", L"vs_6_0", dxcUtils.Get(), dxcCompiler.Get(), includeHandler.Get());
    }
    else if (primitiveType == D3D12_PRIMITIVE_TOPOLOGY_TYPE::D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE)
    {
        pixelShaderBlob = CompileShader(L"include/Shaders/Line.PS.hlsl", L"ps_6_0", dxcUtils.Get(), dxcCompiler.Get(), includeHandler.Get());
        vertexShaderBlob = CompileShader(L"include/Shaders/Line.VS.hlsl", L"vs_6_0", dxcUtils.Get(), dxcCompiler.Get(), includeHandler.Get());
    }
    assert(pixelShaderBlob != nullptr);
    assert(vertexShaderBlob != nullptr);



    D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
    };
    D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
    inputLayoutDesc.pInputElementDescs = inputElementDescs;
    inputLayoutDesc.NumElements = _countof(inputElementDescs);

    D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
    depthStencilDesc.DepthEnable = true;
    depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

    D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
    graphicsPipelineStateDesc.pRootSignature = rootSignature.Get();
    graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;
    graphicsPipelineStateDesc.BlendState = blendDesc;
    graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;
    graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize() };
    graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize() };
    graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
    graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
    graphicsPipelineStateDesc.NumRenderTargets = 1;
    graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    graphicsPipelineStateDesc.PrimitiveTopologyType = primitiveType;
    graphicsPipelineStateDesc.SampleDesc.Count = 1;
    graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

    Microsoft::WRL::ComPtr<ID3D12PipelineState> pso;
    hr = device->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&pso));
    assert(SUCCEEDED(hr));
    return pso;
}

