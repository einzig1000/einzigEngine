#include "Resource/Model/ModelManager.h"
#include "Utilities/functions.h"
#include <filesystem> 


ModelManager::ModelManager()
{}

ModelManager::~ModelManager()
{}

uint32_t ModelManager::LoadModel(const std::string & directoryPath, const std::string & filename, ID3D12Device* device)
{
	// ボックスを作成
	Object3D obj;
	// モデルデータ
	obj.modelData = LoadModelFile(directoryPath, filename);
	// 変換行列
	obj.transform = { {1.0f,1.0f,1.0f}, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f} };
	// AABB .obj → .csv へ拡張子を変換して渡す
	std::string csvFilename = filename;
	size_t dotPos = csvFilename.rfind('.');
	if (dotPos != std::string::npos)
		csvFilename.replace(dotPos, csvFilename.length() - dotPos, ".csv");
	else
		csvFilename += ".csv";
	csvFilename = directoryPath + csvFilename;
	obj.aabb = LoadAABB(csvFilename, obj.modelData);
	// 識別ナンバー
	obj.number = static_cast<uint32_t>(objects.size());

	// まず空のObject3Dをvectorに追加し、参照を取得
	objects.push_back(obj);
	Object3D& ref = objects.back();

	// 頂点バッファ作成
	ref.vertexBufferSize = sizeof(VertexData) * UINT(ref.modelData.vertices.size());
	ref.vertexBuffer = CreateBufferResource(device, ref.vertexBufferSize);
	VertexData* vData = nullptr;
	ref.vertexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&vData));
	std::memcpy(vData, ref.modelData.vertices.data(), ref.vertexBufferSize);
	ref.vertexBuffer->Unmap(0, nullptr);

	ref.vertexBufferView.BufferLocation = ref.vertexBuffer->GetGPUVirtualAddress();
	ref.vertexBufferView.SizeInBytes = static_cast<UINT>(ref.vertexBufferSize);
	ref.vertexBufferView.StrideInBytes = sizeof(VertexData);

	return ref.number;
}

Object3D* ModelManager::GetModel(uint32_t modelID)
{
	if (modelID < objects.size())
	{
		return &objects[modelID];
	}
	else
	{
		Log("存在しないテクスチャIDです:%d", modelID);
		return nullptr;
	}
}

std::vector<AABB> ModelManager::LoadAABB(const std::string& csvPath, const ModelData& model)
{
	std::vector<AABB> aabbs;
	if (std::filesystem::exists(csvPath))
	{
		aabbs = LoadAABBFromCSV(csvPath);
	}
	else
	{
		// 今までの方法でAABBを1つ作成
		AABB aabb = CreateLocalAABB(model);
		aabbs.push_back(aabb);
		SaveAABBToCSV(csvPath, aabbs);
	}
	return aabbs;
}

AABB ModelManager::CreateLocalAABB(const ModelData& model)
{
	AABB localAABB;

	// 最小値と最大値を初期化
	localAABB.min.x = (std::numeric_limits<float>::max)();
	localAABB.min.y = (std::numeric_limits<float>::max)();
	localAABB.min.z = (std::numeric_limits<float>::max)();

	localAABB.max.x = std::numeric_limits<float>::lowest();

	// 頂点データ空だったらエラー出すべきだけどunityシステムあるかもだから落とさない
	if (model.vertices.empty())
	{
		localAABB.min = { 0.0f, 0.0f, 0.0f };
		localAABB.max = { 0.0f, 0.0f, 0.0f };
		return localAABB;
	}

	// 全ての頂点を調べてAABBの最小値と最大値を更新
	for (const auto& vertex : model.vertices)
	{
		// 各軸の最小値を更新
		if (vertex.position.x < localAABB.min.x) localAABB.min.x = vertex.position.x;
		if (vertex.position.y < localAABB.min.y) localAABB.min.y = vertex.position.y;
		if (vertex.position.z < localAABB.min.z) localAABB.min.z = vertex.position.z;

		// 各軸の最大値を更新
		if (vertex.position.x > localAABB.max.x) localAABB.max.x = vertex.position.x;
		if (vertex.position.y > localAABB.max.y) localAABB.max.y = vertex.position.y;
		if (vertex.position.z > localAABB.max.z) localAABB.max.z = vertex.position.z;
	}

	return localAABB;
}