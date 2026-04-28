#include "RenderObject.h"
#include "Engine.h"
#include "DirectX/DirectXManager.h"
#include "DirectX/Resource/Dx12ResourceFactory.h"
#include <DirectX/Pipeline/ShaderReflectionHelper/ShaderReflectionHelper.h>
#include <Utilities/Converter/StringConverter/StringConverter.h>
#include <DrawSystem/DrawSystem.h>
#include <cstring>

void RenderObject::SetupFromShaders()
{
	std::wstring vsPath = StringConverter::Convert(psoConfig_.vs);
	std::wstring psPath = StringConverter::Convert(psoConfig_.ps);
	auto vsBlob = Engine::Instance().GetDirectXManager()->GetPipelineStateManager()->GetOrCompileShader(vsPath.c_str(), L"vs_6_0");
	auto psBlob = Engine::Instance().GetDirectXManager()->GetPipelineStateManager()->GetOrCompileShader(psPath.c_str(), L"ps_6_0");

	rootParams_.clear();
	cpuStorage_.clear();

	size_t cbvOffset = 0;
	uint32_t srvOffset = 0;

	// VS の CBV / SRV を反映
	ShaderReflection::BuildRootParamsFromShader(vsBlob.Get(), ShaderType::VertexShader, rootParams_, cbvOffset, srvOffset);

	// PS の CBV / SRV を反映
	ShaderReflection::BuildRootParamsFromShader(psBlob.Get(), ShaderType::PixelShader, rootParams_, cbvOffset, srvOffset);

	cpuStorage_.resize(cbvOffset);
}

void RenderObject::SetSBufferData(const std::string& key, ShaderType shaderType, const void* data, size_t elementSize, size_t elementCount)
{
	const size_t bytes = elementSize * elementCount;

	for (auto& param : rootParams_)
	{
		if (param.paramType == ParamType::SRV && param.shaderType == shaderType && param.key == key)
		{
			auto* dxManager = Engine::Instance().GetDirectXManager();
			// フレームインデックスを取得
			const uint32_t frameIndex = dxManager->GetSwapChain()->GetCurrentBackBufferIndex() % kMaxFramesInFlight;

			auto& srvData = dynamicSrvStorage_[param.srvIndex];
			if (srvData.mappedData[frameIndex])
			{
				std::memcpy(srvData.mappedData[frameIndex], data, bytes);
			}

			// GPUハンドルをこのフレーム用に更新しておく
			param.srvGpuHandle = srvData.srvAllocations[frameIndex].gpu;
			return;
		}
	}
}

void RenderObject::SetCBufferData(const std::string& key, ShaderType shaderType, const void* data)
{
	for (auto& param : rootParams_)
	{
		if (param.paramType == ParamType::CBV && param.shaderType == shaderType && param.key == key)
		{
			std::memcpy(cpuStorage_.data() + param.offsetBytes, data, param.sizeBytes);
			return;
		}
	}
}

void RenderObject::Draw() const
{
	Engine::Instance().GetDrawSystem()->AddDrawList(this);
}




//void RenderObject::SetBufferData(int index, const void* data)
//{
//	assert(index >= 0);
//	assert(static_cast<size_t>(index) < rootParams_.size());
//	assert(data);
//
//	auto& p = rootParams_[static_cast<size_t>(index)];
//
//	const size_t bytes = static_cast<size_t>(p.sizeBytes) * static_cast<size_t>(p.arraySize);
//	assert(bytes > 0);
//
//	if (p.paramType == ParamType::SRV)
//	{
//		auto* dxManager = Engine::Instance().GetDirectXManager();
//		// フレームインデックスを取得
//		const uint32_t frameIndex = dxManager->GetSwapChain()->GetCurrentBackBufferIndex() % kMaxFramesInFlight;
//
//		auto& srvData = dynamicSrvStorage_[p.offsetBytes];
//		if (srvData.mappedData[frameIndex])
//		{
//			std::memcpy(srvData.mappedData[frameIndex], data, bytes);
//		}
//
//		// GPUハンドルをこのフレーム用に更新しておく
//		p.srvGpuHandle = srvData.srvAllocations[frameIndex].gpu;
//	}
//	else if (p.paramType == ParamType::CBV)
//	{
//		const size_t dstOffset = p.offsetBytes;
//		assert(dstOffset + bytes <= cpuStorage_.size());
//
//		std::memcpy(cpuStorage_.data() + dstOffset, data, bytes);
//	}
//}
//
//int32_t RenderObject::CreateCBV(size_t sizeBytes, ShaderType shaderType, std::string debugName)
//{
//	assert(sizeBytes > 0);
//
//	// cpuStorage_ に領域確保
//	const size_t offset = cpuStorage_.size();	// これまでのサイズ分がオフセットになる
//	const size_t newSize = offset + sizeBytes;	// 新しいサイズは、これまでのサイズ + 今回要求されたサイズ
//	cpuStorage_.resize(newSize);				// リサイズ
//
//	RootParam p{};
//	// CBVである
//	p.paramType = ParamType::CBV;
//	// CBVをどのシェーダーステージで使うか
//	p.shaderType = shaderType;
//	// CBVに渡すデータの単位サイズ。構造体一個分のサイズと認識したってかまわない。
//	p.sizeBytes = static_cast<uint32_t>(sizeBytes);
//	// 配列サイズはCBVなら1
//	p.arraySize = 1;
//	// cpuStorage_ 内のどこに書くかのオフセット。
//	p.offsetBytes = static_cast<uint32_t>(offset);
//
//	rootParams_.push_back(p);
//	debugNames_.push_back(std::move(debugName));
//
//	// ユーザーがSetBufferDataで渡すIDを返す
//	return static_cast<int32_t>(rootParams_.size() - 1);
//}
//
//int32_t RenderObject::CreateSRV(size_t sizeBytes, size_t arraySize, ShaderType shaderType, std::string debugName)
//{
//	assert(sizeBytes > 0);
//	assert(arraySize > 0);
//
//	auto* dxManager = Engine::Instance().GetDirectXManager();
//	auto* device = dxManager->GetDevice();
//	auto* srvManager = dxManager->GetDescriptorHeapManager()->GetSRV_UAVManager();
//
//	const size_t totalBytes = sizeBytes * arraySize;
//
//	DynamicSRVData data{};
//	for (uint32_t i = 0; i < kMaxFramesInFlight; ++i)
//	{
//		data.buffers[i] = Dx12ResourceFactory::CreateBufferResource(device, totalBytes);
//		data.buffers[i]->Map(0, nullptr, &data.mappedData[i]);
//
//		// SRVを作成 (StructuredBufferとして)
//		data.srvAllocations[i] = srvManager->CreateSRVforStructuredBuffer(
//			data.buffers[i].Get(), 
//			static_cast<UINT>(arraySize), 
//			static_cast<UINT>(sizeBytes)
//		);
//	}
//
//	const size_t storageIndex = dynamicSrvStorage_.size();
//	dynamicSrvStorage_.push_back(std::move(data));
//
//	RootParam p{};
//	// SRVである
//	p.paramType = ParamType::SRV;
//	// SRVをどのシェーダーステージで使うか
//	p.shaderType = shaderType;
//	// 単位サイズ。構造体一個分のサイズと認識したってかまわない。
//	p.sizeBytes = static_cast<uint32_t>(sizeBytes);
//	// 配列サイズ。
//	p.arraySize = static_cast<uint32_t>(arraySize);
//	// dynamicSrvStorage_ 内のインデックスとして使用
//	p.offsetBytes = static_cast<uint32_t>(storageIndex);
//
//	rootParams_.push_back(p);
//	debugNames_.push_back(std::move(debugName));
//
//	return static_cast<int32_t>(rootParams_.size() - 1);
//}
