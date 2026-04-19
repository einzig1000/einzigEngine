#include "RenderObject.h"
#include <cassert>
#include <cstring>

int32_t RenderObject::CreateCBV(size_t sizeBytes, ShaderType shaderType, std::string debugName)
{
	assert(sizeBytes > 0);

	// cpuStorage_ に領域確保
	const size_t offset = cpuStorage_.size();	// これまでのサイズ分がオフセットになる
	const size_t newSize = offset + sizeBytes;	// 新しいサイズは、これまでのサイズ + 今回要求されたサイズ
	cpuStorage_.resize(newSize);				// リサイズ

	RootParam p{};
	// CBVである
	p.paramType = ParamType::CBV;
	// CBVをどのシェーダーステージで使うか
	p.shaderType = shaderType;
	// CBVのサイズ。GPUに書くときはこのサイズ分だけ書くことになる。
	p.sizeBytes = static_cast<uint32_t>(sizeBytes);
	// cpuStorage_ 内のどこに書くかのオフセット。
	p.offsetBytes = static_cast<uint32_t>(offset);

	rootParams_.push_back(p);
	debugNames_.push_back(std::move(debugName));

	// ユーザーがSetBufferDataで渡すIDを返す
	return static_cast<int32_t>(rootParams_.size() - 1);
}

int32_t RenderObject::CreateSRV(ShaderType shaderType, std::string debugName)
{
	RootParam p{};
	// SRVである
	p.paramType = ParamType::SRV;
	// SRVをどのシェーダーステージで使うか
	p.shaderType = shaderType;
	// SRVはGPUハンドルを保存する。
	p.srvGpuHandle = {};

	rootParams_.push_back(p);
	debugNames_.push_back(std::move(debugName));

	// ユーザーがSetBufferDataで渡すIDを返す
	return static_cast<int32_t>(rootParams_.size() - 1);
}

void RenderObject::SetBufferData(int index, const void* data)
{
	assert(index >= 0);
	assert(static_cast<size_t>(index) < rootParams_.size());
	assert(data);

	auto& p = rootParams_[static_cast<size_t>(index)];
	assert(p.paramType == ParamType::CBV);

	const size_t dstOffset = p.offsetBytes;
	assert(dstOffset + p.sizeBytes <= cpuStorage_.size());

	std::memcpy(cpuStorage_.data() + dstOffset, data, p.sizeBytes);
}

void RenderObject::SetSRVHandle(int index, D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle)
{
	assert(index >= 0);
	assert(static_cast<size_t>(index) < rootParams_.size());

	auto& p = rootParams_[static_cast<size_t>(index)];
	assert(p.paramType == ParamType::SRV);

	p.srvGpuHandle = gpuHandle;
}
