#include "RenderObject.h"
#include "engine/Utilities/functions.h"

int RenderObject::CreateCBV(size_t size, ShaderType type, std::string debugName)
{
	std::vector<BufferData> cbvBufferDatas;
	std::vector<D3D12_GPU_VIRTUAL_ADDRESS> cbvAddresses;

	// ダブルバッファリングになったら2回ループするようにする
	for (int i = 0; i < 1; ++i)
	{
		Resource res{};
		res.res.Attach(CreateConstantBufferResource(device_, size));	//256の倍数に揃える
		res.res->SetName(ConvertString(debugName).c_str());
		resources_.push_back(res);

		//マッピング
		BufferData bufferData{};
		bufferData.size = size;
		HRESULT hr = res.res->Map(0, nullptr, &bufferData.mapped);
		assert(SUCCEEDED(hr));
		cbvBufferDatas.push_back(bufferData);

		//CBVアドレスの取得
		cbvAddresses.push_back(res.res->GetGPUVirtualAddress());
	}

	bufferDatas_.push_back(cbvBufferDatas);
	cbvAddresses_.push_back(cbvAddresses);

	Log("%sのCBVを作成しました。サイズ: %zu", debugName.c_str(), size);

	if (type == ShaderType::VERTEX_SHADER)
	{
		psoConfig_.rootConfig.cbvNums.first++;
	}
	else
	{
		psoConfig_.rootConfig.cbvNums.second++;
	}

	//indexの交付
	return int(bufferDatas_.size() - 1);
}

int RenderObject::CreateSRV(size_t size, uint32_t num, ShaderType type, std::string debugName)
{
	std::vector<BufferData> srvBufferDatas;
	std::vector<std::unique_ptr<SRVHandle>> srvHandles;

	auto srv = device_->GetSRVManager();
	for (int i = 0; i < 1; ++i)
	{
		Resource res{};
		size_t bufferSize = size;
		res.res.Attach(CreateBufferResource(device_, bufferSize * num));
		res.res->SetName(ConvertString(debugName).c_str());
		resources_.push_back(res);

		//SRVハンドルの取得
		std::unique_ptr<SRVHandle> srvHandle = std::make_unique<SRVHandle>();
		srvHandle->UpdateHandle(srv);

		//ParticleDataのSRV作成
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		srvDesc.Buffer.FirstElement = 0;
		srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
		srvDesc.Buffer.NumElements = num;
		srvDesc.Buffer.StructureByteStride = UINT(bufferSize);

		device_->GetDevice()->CreateShaderResourceView(res.res.Get(), &srvDesc, srvHandle->GetCPU());

		srvHandles.push_back(std::move(srvHandle));

		//マッピング
		BufferData bufferData{};
		bufferData.size = size * num;
		res.res->Map(0, nullptr, &bufferData.mapped);
		srvBufferDatas.push_back(bufferData);
	}

	bufferDatas_.push_back(srvBufferDatas);
	srvHandles_.push_back(std::move(srvHandles));

	logger_->debug("SRV Created: {}", debugName_);
	logger_->debug("  Size: {}", srvBufferDatas.front().size);
	logger_->debug("  Num: {}", num);
	logger_->debug("  Index: {}", srvHandles_.back().back()->GetOffset());

	if (type == ShaderType::VERTEX_SHADER)
	{
		psoConfig_.rootConfig.srvNums.first++;
	}
	else
	{
		psoConfig_.rootConfig.srvNums.second++;
	}

	return int(bufferDatas_.size() - 1);
}

void RenderObject::CopyBufferData(int index, const void* data, size_t size)
{
	auto& gpuData = bufferDatas_[index][index_];

	if (size > gpuData.size)
	{
		logger_->error("=========== CopyBufferData || Size exceeds buffer size ===========");
		logger_->error("  RenderObject: {}\n  GPUSize: {}\n  DataSize: {}", debugName_, gpuData.size, size);
		assert(false && "RenderObject::CopyBufferData: Size exceeds buffer size");
		return;
	}

	std::memcpy(gpuData.mapped, data, size);
}