#pragma once
#include <string>
#include <vector>
#include "definition/definition.h"

struct PSOConfig
{
	/// @brief 頂点シェーダーファイル名
	std::string vs = "Object3d.VS.hlsl";
	/// @brief ピクセルシェーダーファイル名
	std::string ps = "Object3d.PS.hlsl";
	/// @brief ルートシグネチャ設定
	RootSignatureConfig rootConfig = {};
	/// @brief 入力レイアウトID
	InputLayoutID inputLayoutID = InputLayoutID::Default;
	/// @brief ブレンドステートID
	BlendStateID blendID = BlendStateID::Normal;
	/// @brief 深度ステンシルID
	DepthStencilID depthStencilID = DepthStencilID::Default;
	/// @brief ラスタライザーID
	RasterizerID rasterizerID = RasterizerID::Fill;
	/// @brief プリミティブトポロジー
	D3D12_PRIMITIVE_TOPOLOGY topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	/// @brief スワップチェーン用かどうか
	bool isSwapChain = false;

	bool operator==(const PSOConfig& other) const
	{
		return vs == other.vs &&
			ps == other.ps &&
			blendID == other.blendID &&
			depthStencilID == other.depthStencilID &&
			rasterizerID == other.rasterizerID &&
			rootConfig == other.rootConfig &&
			inputLayoutID == other.inputLayoutID &&
			topology == other.topology &&
			isSwapChain == other.isSwapChain;
	}
};

	enum class ShaderType
	{
		VERTEX_SHADER,  ///< 頂点シェーダー
		PIXEL_SHADER,   ///< ピクセルシェーダー
		COMPUTE_SHADER, ///< コンピュートシェーダー

		Count           ///< シェーダータイプの総数
	};


/// <summary>
///	何を描くか」と「シェーダに何を渡すか」をユーザーが宣言・更新できるようにする
///	ただし GPUメモリ実体（CB用 ID3D12Resource）は所有しない（B方式）
///	描画時に必要なバインド情報を Draw() でコマンドへ反映する（ただしCBの確保は外部Allocatorに依存）
/// </summary>
class RenderObject
{
public:
	RenderObject() {}
	virtual ~RenderObject() {}
	virtual void Update() = 0;

	void Initialize();

	void SetDrawData(const DrawData& data);


	/// <summary>
	/// CBVを作成
	/// </summary>
	/// <param name="size">バッファサイズ</param>
	/// <param name="type">シェーダータイプ</param>
	/// <param name="debugName">デバッグ用の名前</param>
	/// <returns>ルートパラメータのインデックス</returns>
	int32_t CreateCBV(size_t size, ShaderType type, std::string debugName = "");

	/// <summary>
	/// SRVを作成
	/// </summary>
	/// <param name="size">バッファサイズ</param>
	/// <param name="num">要素数</param>
	/// <param name="type">シェーダータイプ</param>
	/// <param name="debugName">デバッグ用の名前</param>
	/// <returns>ルートパラメータのインデックス</returns>
	int32_t CreateSRV(size_t size, uint32_t num, ShaderType type, std::string debugName = "");

	/// <summary>
	/// バッファにデータをセット
	/// </summary>
	/// <param name="index">ルートパラメータのインデックス</param>
	/// <param name="data">セットするデータのポインタ</param>
	/// <param name="size">セットするデータのサイズ</param>
	void SetBufferData(int index, const void* data, size_t size);


	void Draw();


	/// @brief パイプラインステート設定
	PSOConfig psoConfig_{};
	/// @brief インスタンス数
	uint32_t instanceNum_ = 1;

	/// @brief コピー禁止
	void operator=(const RenderObject& other) = delete;


	struct BufferData
	{
		/// @brief マップされたメモリへのポインタ
		void* mapped = nullptr;
		/// @brief バッファサイズ
		size_t size = 0;
	};
	/// @brief バッファデータのリスト（二重配列：外側=バッファ種類、内側=スワップチェーン対応）
	std::vector<std::vector<BufferData>> bufferDatas_{};

	/// @brief 定数バッファのGPUアドレスリスト
	std::vector<std::vector<D3D12_GPU_VIRTUAL_ADDRESS>> cbvAddresses_{};


	struct Resource
	{
		/// @brief ID3D12ResourceのComポインタ
		Microsoft::WRL::ComPtr<ID3D12Resource> res = nullptr;
	};
	/// @brief リソースのリスト
	std::vector<Resource> resources_{};
};

