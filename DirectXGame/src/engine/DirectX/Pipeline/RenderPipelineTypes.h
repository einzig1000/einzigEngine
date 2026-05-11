#pragma once
#include <cstdint>
#include <d3d12.h>

enum class ShaderType
{
	None,
    VertexShader,
    PixelShader,
    ComputeShader,
};

enum class ParamType
{
	None,
	CBV,
	SRV,
};

struct InputElement
{
    std::string semanticName;
	UINT semanticIndex;
    D3D12_INPUT_ELEMENT_DESC desc;
};



struct RootParam
{
	ParamType paramType = ParamType::None;
	ShaderType shaderType = ShaderType::None;
    uint32_t key = 0;           // "b0", "b1" など
	size_t vectorIndex = 0;     // RootParameterの配列内インデックス。

	// CBuffer用
	uint32_t sizeBytes = 0;     // 自身のサイズ。CBuffer用ストレージ内でどれだけのサイズが必要か。
	uint32_t offsetBytes = 0;   // cpuStorage_ 内オフセット

	// SRV用
	uint32_t srvStorageIndex = 0;   // そのSRVがdynamicSrvStorage_のどこにあるか。
	uint32_t srvAllocIndex = UINT32_MAX;    // SRVのスロットインデックス(0であればBindlessテクスチャ配列、1以上UINT32_MAX未満であればリソース確保済みのSRV)
};

// ブレンドステートの種類
enum class BlendStateID : uint8_t
{
    Opaque,
    Normal,
    Add,
    Sub,
    Mul,
    Screen,
};

// 深度ステンシルの種類
enum class DepthStencilID : uint8_t
{
    Default,      // depth write/test
    TestOnly,     // depth test only (write off)
    Disable,      // depth off
};

// ラスタライザの種類
enum class RasterizerID : uint8_t
{
    Fill,
    Wireframe,
};

struct PSOConfig
{
    /// 頂点シェーダーファイル名
    std::string vs = "Object3d.VS.hlsl";
    /// ピクセルシェーダーファイル名
    std::string ps = "Object3d.PS.hlsl";
    /// ブレンドステートID
    BlendStateID blendID = BlendStateID::Normal;
    /// 深度ステンシルID
    DepthStencilID depthStencilID = DepthStencilID::Default;
    /// ラスタライザーID
    RasterizerID rasterizerID = RasterizerID::Fill;
    /// プリミティブトポロジ
    D3D12_PRIMITIVE_TOPOLOGY topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    /// スワップチェーン用かどうか
    bool isSwapChain = false;

    bool operator==(const PSOConfig& other) const
    {
        return vs == other.vs &&
            ps == other.ps &&
            blendID == other.blendID &&
            depthStencilID == other.depthStencilID &&
            rasterizerID == other.rasterizerID &&
            topology == other.topology &&
            isSwapChain == other.isSwapChain;
    }
};

