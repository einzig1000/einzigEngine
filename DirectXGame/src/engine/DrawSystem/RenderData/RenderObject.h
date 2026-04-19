#pragma once
#include <string>
#include <vector>
#include "DirectX/PipeLine/RenderPipelineTypes.h"


//// 例：Material/WVP/Light を RenderObject に登録
// 
// Material material{};
// material.color = { 1.0f, 0.0f, 0.0f, 1.0f };
// 
// Matrix4x4 wvp{};
// wvp = Matrix4x4::MakeIdentity();
// 
// DirectionalLight light{};
// light.color = { 1.0f, 1.0f, 1.0f, 1.0f };
// 
//int cbMaterial = ro.CreateCBV(sizeof(Material), RenderObject::ShaderTypxaderType::VERTEX_SHADER, "WVP");
//int cbLight = ro.CreateCBV(sizeof(DirectionalLight), RenderObject::ShaderType::PIXEL_SHADER, "Light");
//
//// 毎フレーム（or 更新時）にスナップショットをセット
//ro.SetBufferDataT(cbMaterial, material);
//ro.SetBufferDataT(cbWvp, wvp);
//ro.SetBufferDataT(cbLight, light);
//
//// 描画側
//ro.ApplyRootParams(cmdList, cbAllocators_[GetFrameIndex()]);



/// <summary>
// ・PSO 設定（どのシェーダ・どのブレンド・どのラスタライザか）
// ・ルートパラメータ（CBV / SRV）とその中身
// だけを持つ、描画オブジェクトの基底クラス。描画に必要な情報はここに集約するイメージ。
/// </summary>
class RenderObject
{
public:
	RenderObject() = default;
	virtual ~RenderObject() = default;

	RenderObject(const RenderObject&) = delete;
	RenderObject& operator=(const RenderObject&) = delete;

	virtual void Update() = 0;


	// rootIndex を払い出す（=RootSignature上のスロット番号とは別。ここではRenderObject内のID）
	int32_t CreateCBV(size_t sizeBytes, ShaderType shaderType, std::string debugName = "");
	int32_t CreateSRV(ShaderType shaderType, std::string debugName = "");

	// CBVデータをCPUスナップショットへコピー（GPUへは書かない）
	void SetBufferData(int index, const void* data);

	void SetSRVHandle(int index, D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle);

	const std::vector<RootParam>& GetRootParams() const { return rootParams_; }
	const std::vector<uint8_t>& GetCpuStorage() const { return cpuStorage_; }

	// SRVはGPUハンドルを保存（GPUバッファの所有はここではしない
public:
	// PSO設定
	PSOConfig psoConfig_{};
	// インスタンス数
	uint32_t instanceNum_ = 1;

private:
	// RootParameterにいれるものリスト。CBVもSRVもここで管理する
	std::vector<RootParam> rootParams_{};

	// CBVの内容をuint8_tのただのバイト列で保持。読みとる時はreinterpret_castで型を戻すイメージ。すべての情報を型に依存せずまとめて管理するためのもの。
	std::vector<uint8_t> cpuStorage_{};

	// デバッグ用
	std::vector<std::string> debugNames_{};
};