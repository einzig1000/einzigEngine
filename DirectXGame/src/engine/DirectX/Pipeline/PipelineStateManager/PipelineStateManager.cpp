#include "PipelineStateManager.h"
#include <string>
#include <DirectX/Pipeline/ShaderReflectionHelper/ShaderReflectionHelper.h>
#include <Utilities/Logger/Logger.h>
#include <Utilities/Converter/StringConverter/StringConverter.h>
#include <filesystem>
#include <DrawSystem/RenderData/RenderObject.h>

#pragma comment(lib, "dxcompiler.lib")


namespace
{
    static size_t HashCombine(size_t seed, size_t v)
    {
        return seed ^ (v + 0x9e3779b97f4a7c15ULL + (seed << 6) + (seed >> 2));
    }

    static size_t HashString(const std::string& s)
    {
        return std::hash<std::string>{}(s);
    }

	static size_t HashRootLayout(const std::vector<RootParam>& params)
    {
        size_t h = 1469598103934665603ULL;
        h = HashCombine(h, params.size());
        for (size_t i = 0; i < params.size(); ++i)
        {
            h = HashCombine(h, static_cast<size_t>(params[i].paramType));
            h = HashCombine(h, static_cast<size_t>(params[i].shaderType));
			h = HashCombine(h, static_cast<size_t>(params[i].key));
            if (params[i].paramType == ParamType::CBV)
            {
                // CBVはサイズも考慮する
                h = HashCombine(h, params[i].sizeBytes);
			}
        }
        return h;
    }

    static size_t HashPsoConfig(const PSOConfig& c)
    {
        size_t h = 0;
        h = HashCombine(h, HashString(c.vs));
        h = HashCombine(h, HashString(c.ps));
        h = HashCombine(h, static_cast<size_t>(c.blendID));
        h = HashCombine(h, static_cast<size_t>(c.depthStencilID));
        h = HashCombine(h, static_cast<size_t>(c.rasterizerID));
        h = HashCombine(h, static_cast<size_t>(c.topology));
        h = HashCombine(h, static_cast<size_t>(c.isSwapChain ? 1 : 0));

        return h;
    }


    static D3D12_PRIMITIVE_TOPOLOGY_TYPE ToTopologyType(D3D12_PRIMITIVE_TOPOLOGY topo)
    {
        switch (topo)
        {
        case D3D_PRIMITIVE_TOPOLOGY_LINELIST:
            return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
        case D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST:
            return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        default:
            return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        }
    }

    static D3D12_BLEND_DESC MakeBlendDesc(BlendStateID id)
    {
        D3D12_BLEND_DESC d{};
        // 
        d.AlphaToCoverageEnable = FALSE;
        // 
        d.IndependentBlendEnable = FALSE;
        auto& rt = d.RenderTarget[0];
        rt.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

        switch (id)
        {
        case BlendStateID::Normal:
            rt.BlendEnable = TRUE;
            rt.SrcBlend = D3D12_BLEND_SRC_ALPHA;
            rt.BlendOp = D3D12_BLEND_OP_ADD;
            rt.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
            rt.SrcBlendAlpha = D3D12_BLEND_ONE;
            rt.DestBlendAlpha = D3D12_BLEND_ZERO;
            rt.BlendOpAlpha = D3D12_BLEND_OP_ADD;
            break;
        case BlendStateID::Add:
            rt.BlendEnable = TRUE;
            rt.SrcBlend = D3D12_BLEND_SRC_ALPHA;
            rt.BlendOp = D3D12_BLEND_OP_ADD;
            rt.DestBlend = D3D12_BLEND_ONE;
            rt.SrcBlendAlpha = D3D12_BLEND_ONE;
            rt.DestBlendAlpha = D3D12_BLEND_ZERO;
            rt.BlendOpAlpha = D3D12_BLEND_OP_ADD;
            break;
        case BlendStateID::Sub:
            rt.BlendEnable = TRUE;
            rt.SrcBlend = D3D12_BLEND_SRC_ALPHA;
            rt.BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
            rt.DestBlend = D3D12_BLEND_ONE;
            rt.SrcBlendAlpha = D3D12_BLEND_ONE;
            rt.DestBlendAlpha = D3D12_BLEND_ZERO;
            rt.BlendOpAlpha = D3D12_BLEND_OP_ADD;
            break;
        case BlendStateID::Mul:
            rt.BlendEnable = TRUE;
            rt.SrcBlend = D3D12_BLEND_DEST_COLOR;
            rt.BlendOp = D3D12_BLEND_OP_ADD;
            rt.DestBlend = D3D12_BLEND_ZERO;
            rt.SrcBlendAlpha = D3D12_BLEND_ONE;
            rt.DestBlendAlpha = D3D12_BLEND_ZERO;
            rt.BlendOpAlpha = D3D12_BLEND_OP_ADD;
            break;
        case BlendStateID::Screen:
            rt.BlendEnable = TRUE;
            rt.SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
            rt.BlendOp = D3D12_BLEND_OP_ADD;
            rt.DestBlend = D3D12_BLEND_ONE;
            rt.SrcBlendAlpha = D3D12_BLEND_ONE;
            rt.DestBlendAlpha = D3D12_BLEND_ZERO;
            rt.BlendOpAlpha = D3D12_BLEND_OP_ADD;
            break;
		case BlendStateID::Opaque:
			rt.BlendEnable = FALSE;
			break;
        }
        return d;
    }

    static D3D12_RASTERIZER_DESC MakeRasterizerDesc(RasterizerID id)
    {
        D3D12_RASTERIZER_DESC d{};
        d.CullMode = D3D12_CULL_MODE_NONE;
        d.FillMode = (id == RasterizerID::Fill) ? D3D12_FILL_MODE_SOLID : D3D12_FILL_MODE_WIREFRAME;
        d.DepthClipEnable = TRUE;
        return d;
    }

    static D3D12_DEPTH_STENCIL_DESC MakeDepthStencilDesc(DepthStencilID id)
    {
		D3D12_DEPTH_STENCIL_DESC d{};

        switch (id)
        {
        case DepthStencilID::Default:
            d.DepthEnable = TRUE;
            d.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
            d.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
            break;

        case DepthStencilID::TestOnly:
            d.DepthEnable = TRUE;
            d.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
            d.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
            break;

        case DepthStencilID::Disable:
            d.DepthEnable = FALSE;
            d.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
            d.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
            break;
        }


        return d;
    }
}

PipelineStateManager::PipelineStateManager(ID3D12Device* device)
	: device_(device)
{
    InitializeDxc();
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

Microsoft::WRL::ComPtr<ID3D12RootSignature> PipelineStateManager::GetOrCreateRootSignature(const std::vector<RootParam>& params)
{
	// ハッシュキーを生成
	const size_t key = HashRootLayout(params);

	// キャッシュにあればそれを返す
    auto it = rootSignatureCache_.find(key);
    if (it != rootSignatureCache_.end())
    {
        return it->second;
    }

	// ルートシグネチャ生成
    auto rs = CreateRootSignature(params);
	assert(rs);
	Log("ルートシグネチャ新規生成成功: キー %zu", key);

	// キャッシュに保存してから返す
    rootSignatureCache_.emplace(key, rs);
	return rs;
}

Microsoft::WRL::ComPtr<ID3D12PipelineState> PipelineStateManager::GetOrCreateGraphicsPipelineState(const PSOConfig& psoConfig, const std::vector<RootParam>& params)
{
	// ハッシュキーを生成
    const size_t rootKey = HashRootLayout(params);
    const size_t psoKey = HashCombine(HashPsoConfig(psoConfig), rootKey);

	// キャッシュにあればそれを返す
    auto it = psoCache_.find(psoKey);
    if (it != psoCache_.end())
    {
        return it->second;
    }

	// パイプラインステート生成
	auto pso = CreatePipelineState(psoConfig, params);
    assert(pso);
	Log("PSO新規生成成功: キー %zu", psoKey);

	// キャッシュに保存してから返す
    psoCache_.emplace(psoKey, pso);
	return pso;
}

Microsoft::WRL::ComPtr<IDxcBlob> PipelineStateManager::GetOrCompileShader(const wchar_t* path, const wchar_t* target)
{
    // キーを生成
    std::wstring key = std::wstring(path) + L"|" + target;

	// キャッシュにあればそれを返す
    auto it = shaderCache_.find(key);
    if (it != shaderCache_.end())
    {
        return it->second;
    }

	// blob生成
    auto blob = CompileShader(path, target);
    assert(blob);
	Log("シェーダー新規コンパイル成功: キー %s", key.c_str());

	// キャッシュに保存してから返す
    shaderCache_.emplace(std::move(key), blob);
    return blob;
}



Microsoft::WRL::ComPtr<ID3D12RootSignature> PipelineStateManager::CreateRootSignature(const std::vector<RootParam>& params)
{
    //auto* srvMgr = Engine::Instance()
    //    .GetDirectXManager()
    //    ->GetDescriptorHeapManager()
    //    ->GetSRV_UAVManager();

    //const UINT srvCapacity = srvMgr->GetTextureCapacity() + srvMgr->GetBufferCapacity();


    size_t srvCount = 0;
	size_t cbvCount = 0;
    for (const auto& p : params)
    {
        if (p.paramType == ParamType::CBV)
        {
            ++cbvCount;
		}
        else if (p.paramType == ParamType::SRV)
        {
            ++srvCount;
        }
        else
        {
            assert(false && "Invalid RootParam");
		}
    }

    std::vector<D3D12_ROOT_PARAMETER> rootParams;
	rootParams.resize(params.size());

    std::vector<D3D12_DESCRIPTOR_RANGE> srvRanges;
	srvRanges.resize(srvCount);

    size_t srvIndex = 0;

    for (size_t i = 0; i < params.size(); ++i)
    {
        D3D12_ROOT_PARAMETER rootParam{};

        if (params[i].paramType == ParamType::CBV)
        {
            rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
			const UINT reg = static_cast<UINT>(params[i].key);
            rootParam.Descriptor.ShaderRegister = reg;
            rootParam.Descriptor.RegisterSpace = 0;
            rootParam.ShaderVisibility = GetShaderVisibilityFromShaderType(params[i].shaderType);
        }
        else if (params[i].paramType == ParamType::SRV)
        {
            assert(srvIndex < srvRanges.size());

            D3D12_DESCRIPTOR_RANGE& range = srvRanges[srvIndex++];

            range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
            const UINT reg = static_cast<UINT>(params[i].key);
            range.BaseShaderRegister = reg;
            if (params[i].isBindless)
            {
                range.NumDescriptors = UINT_MAX;
            }
            else
            {
                range.NumDescriptors = 1;
            }
            range.RegisterSpace = 0;
            range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

            rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
            rootParam.DescriptorTable.NumDescriptorRanges = 1;
            rootParam.DescriptorTable.pDescriptorRanges = &range;
            rootParam.ShaderVisibility = GetShaderVisibilityFromShaderType(params[i].shaderType);
        }
        else
        {
            assert(false && "Invalid RootParam");
		}

        rootParams[i] = rootParam;
    }

    D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
    staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
    staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
    staticSamplers[0].ShaderRegister = 0; // s0
    staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

    D3D12_ROOT_SIGNATURE_DESC desc{};
    desc.NumParameters = static_cast<UINT>(rootParams.size());
    desc.pParameters = rootParams.empty() ? nullptr : rootParams.data();
    desc.NumStaticSamplers = _countof(staticSamplers);
    desc.pStaticSamplers = staticSamplers;
    desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob = nullptr;
    Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
    HRESULT hr = D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1_0, &signatureBlob, &errorBlob);
    if (FAILED(hr))
    {
        if (errorBlob)
        {
            Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
        }
        assert(false);
        return nullptr;
    }

    // ルートシグネチャ生成
    Microsoft::WRL::ComPtr<ID3D12RootSignature> rs;
    hr = device_->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rs));
    assert(SUCCEEDED(hr));

    return rs;
}

Microsoft::WRL::ComPtr<ID3D12PipelineState> PipelineStateManager::CreatePipelineState(const PSOConfig& cfg, const std::vector<RootParam>& params)
{
    HRESULT hr;

    // シェーダー取得（なければコンパイル）
    std::wstring vsPath = StringConverter::Convert(cfg.vs);
    std::wstring psPath = StringConverter::Convert(cfg.ps);
    auto vsBlob = GetOrCompileShader(vsPath.c_str(), L"vs_6_0");
    auto psBlob = GetOrCompileShader(psPath.c_str(), L"ps_6_0");

    // ルートシグネチャ取得（なければ生成）
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rs = GetOrCreateRootSignature(params);

    // 各IDからDESCを取得
    const D3D12_BLEND_DESC blendDesc = MakeBlendDesc(cfg.blendID);
    const D3D12_RASTERIZER_DESC rasterizerDesc = MakeRasterizerDesc(cfg.rasterizerID);
    const D3D12_DEPTH_STENCIL_DESC depthStencilDesc = MakeDepthStencilDesc(cfg.depthStencilID);
    const D3D12_PRIMITIVE_TOPOLOGY_TYPE topoType = ToTopologyType(cfg.topology);

    std::vector<InputElement> inputLayout = ShaderReflection::GetInputLayoutFromShader(vsBlob.Get());
    for (auto& elem : inputLayout)
    {
		elem.desc.SemanticName = elem.semanticName.c_str();
		elem.desc.SemanticIndex = elem.semanticIndex;
	}

    std::vector<D3D12_INPUT_ELEMENT_DESC> inputElementDescs;
    inputElementDescs.reserve(inputLayout.size());
    for (auto& elem : inputLayout)
    {
        inputElementDescs.push_back(elem.desc);
    }

    D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
    graphicsPipelineStateDesc.pRootSignature = rs.Get();
    graphicsPipelineStateDesc.InputLayout.pInputElementDescs = inputElementDescs.data();
	graphicsPipelineStateDesc.InputLayout.NumElements = static_cast<UINT>(inputElementDescs.size());
    graphicsPipelineStateDesc.BlendState = blendDesc;
    graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;
    graphicsPipelineStateDesc.VS = { vsBlob->GetBufferPointer(), vsBlob->GetBufferSize() };
    graphicsPipelineStateDesc.PS = { psBlob->GetBufferPointer(), psBlob->GetBufferSize() };
    graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
    graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
    graphicsPipelineStateDesc.NumRenderTargets = 1;
    graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    graphicsPipelineStateDesc.PrimitiveTopologyType = topoType;
    graphicsPipelineStateDesc.SampleDesc.Count = 1;
    graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

    Microsoft::WRL::ComPtr<ID3D12PipelineState> pso;
    hr = device_->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&pso));
    assert(SUCCEEDED(hr));
    return pso;
}

Microsoft::WRL::ComPtr<IDxcBlob> PipelineStateManager::CompileShader(const std::wstring& filePath, const wchar_t* profile)
{
    ///////////////////////////////////////
    //// 1 hlslファイルを読む
    ///////////////////////////////////////
    /// これからシェーダーをコンパイルする旨をログに出す
    Log(StringConverter::Convert(std::format(L"Begin CompileShader, path:{}, profile:{}", filePath, profile)));
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
    Log(StringConverter::Convert(std::format(L"Compile Succeeded. path:{}\n", filePath, profile)));
    // もう使わないリソースを解放
    shaderSource->Release();
    shaderResult->Release();
    // 実行用のバイナリを返却
    return shaderBlob;
}



D3D12_SHADER_VISIBILITY PipelineStateManager::GetShaderVisibilityFromShaderType(ShaderType shaderType)
{
    switch (shaderType)
    {
    case ShaderType::VertexShader:
        return D3D12_SHADER_VISIBILITY_VERTEX;
    case ShaderType::PixelShader:
        return D3D12_SHADER_VISIBILITY_PIXEL;
	case ShaderType::ComputeShader:
		return D3D12_SHADER_VISIBILITY_ALL;

    default:
        assert(false);
        return D3D12_SHADER_VISIBILITY_ALL;
	}
}
