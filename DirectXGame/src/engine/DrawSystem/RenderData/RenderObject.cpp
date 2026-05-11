#include "RenderObject.h"
#include "Engine.h"
#include "DirectX/DirectXManager.h"
#include "DirectX/Resource/Dx12ResourceFactory.h"
#include <DirectX/Pipeline/ShaderReflectionHelper/ShaderReflectionHelper.h>
#include <Utilities/Converter/StringConverter/StringConverter.h>
#include <DrawSystem/DrawSystem.h>
#include <cstring>
#include <cstdint>

void RenderObject::SetupFromShaders()
{
	std::wstring vsPath = StringConverter::Convert(psoConfig_.vs);
	std::wstring psPath = StringConverter::Convert(psoConfig_.ps);
	auto vsBlob = Engine::Instance().GetDirectXManager()->GetPipelineStateManager()->GetOrCompileShader(vsPath.c_str(), L"vs_6_0");
	auto psBlob = Engine::Instance().GetDirectXManager()->GetPipelineStateManager()->GetOrCompileShader(psPath.c_str(), L"ps_6_0");

	rootParams_.clear();
	cpuStorage_.clear();
	dynamicSrvStorage_.clear();

	size_t cbvSizeOffset = 0;
	uint32_t srvIndexOffset = 0;

	// VS の CBV / SRV を反映
	ShaderReflection::BuildRootParamsFromShader(vsBlob.Get(), ShaderType::VertexShader, rootParams_, cbvSizeOffset, srvIndexOffset);
	// PS の CBV / SRV を反映
	ShaderReflection::BuildRootParamsFromShader(psBlob.Get(), ShaderType::PixelShader, rootParams_, cbvSizeOffset, srvIndexOffset);

	cpuStorage_.resize(cbvSizeOffset);
	dynamicSrvStorage_.resize(srvIndexOffset);
}

void RenderObject::SetSBufferData(const uint32_t key, ShaderType shaderType, const void* data, size_t elementSize, size_t elementCount)
{
	const size_t bytes = elementSize * elementCount;
	const uint32_t hash = ShaderReflection::HashRootParam(ParamType::SRV, shaderType, key);
	const auto& it = rootParams_.find(hash);
	if (it == rootParams_.end()) return;

	auto& param = it->second;
	if (param.paramType == ParamType::SRV && param.shaderType == shaderType && param.key == key)
	{
		auto* dxManager = Engine::Instance().GetDirectXManager();
		// フレームインデックスを取得
		const uint32_t frameIndex = dxManager->GetSwapChain()->GetCurrentBackBufferIndex() % kMaxFramesInFlight;

		auto& srvData = dynamicSrvStorage_.at(param.srvStorageIndex);
		auto& alloc = srvData.srvAllocations[frameIndex];

		bool needsNewBuffer = (srvData.buffers[frameIndex] == nullptr) || (bytes > srvData.buffers[frameIndex]->GetDesc().Width);

		// まだSRV用のバッファが確保されていない場合は確保する
		if (needsNewBuffer)
		{
			srvData.buffers[frameIndex] = Dx12ResourceFactory::CreateBufferResource(dxManager->GetDevice(), bytes);
			srvData.buffers[frameIndex]->Map(0, nullptr, &srvData.mappedData[frameIndex]);
		}

		// まだSRVAllocationがない場合は作成する。
		if (alloc.index == UINT32_MAX)
		{
			// SRVを作成
			alloc = dxManager->GetDescriptorHeapManager()->GetSRV_UAVManager()->CreateSRVforStructuredBuffer(
				srvData.buffers[frameIndex].Get(),
				static_cast<UINT>(elementCount),
				static_cast<UINT>(elementSize)
			);
		}
		else
		{
			dxManager->GetDescriptorHeapManager()->GetSRV_UAVManager()->RewriteSRVforStructuredBuffer(
				alloc, srvData.buffers[frameIndex].Get(),
				static_cast<UINT>(elementCount),
				static_cast<UINT>(elementSize));
		}

		// CPU->GPU(Upload heap) へ書き込み
		assert(srvData.mappedData[frameIndex]);
		std::memcpy(srvData.mappedData[frameIndex], data, bytes);

		// RootParamにSRVのスロットインデックスを保存
		param.srvAllocIndex = alloc.index;
		return;
	}
}

void RenderObject::SetCBufferData(const uint32_t key, ShaderType shaderType, const void* data)
{
	uint32_t hash = ShaderReflection::HashRootParam(ParamType::CBV, shaderType, key);
	const auto& it = rootParams_.find(hash);
	if (it == rootParams_.end()) return;
	const auto& param = it->second;

	if (param.paramType == ParamType::CBV && param.shaderType == shaderType && param.key == key)
	{
		std::memcpy(cpuStorage_.data() + param.offsetBytes, data, param.sizeBytes);
		return;
	}
}

void RenderObject::Draw() const
{
	Engine::Instance().GetDrawSystem()->AddDrawList(this);
}

