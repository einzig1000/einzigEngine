#pragma once
#include "definition/definition.h"
#include <sstream>

/// <summary>
/// モデル管理クラス
/// </summary>
class ModelManager
{
public:
	ModelManager(ID3D12Device* device);
	~ModelManager();

	int32_t LoadModel(
		const std::string& directoryPath, 
		const std::string& filename);
	Object3D* GetModelData(int32_t modelID);

	// モデル数を取得
	size_t GetModelCount() const { return objects.size(); }

	// モデルリストを取得
	std::vector<Object3D>& GetModelList() { return objects; }



private:
	ID3D12Device* device_;

	// モデルデータを詰める
	std::vector<Object3D> objects;

	// mtlファイルを読み込む関数
	std::string LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);

	// objファイルを読み込む関数
	ModelData LoadModelFile(const std::string& directoryPath, const std::string& filename);

	

	// １，AABB読み込み
	std::vector<AABB> LoadAABB(const std::string& csvPath, const ModelData& model);

	// ２、AABB.csvがあれば読み込み
	std::vector<AABB> LoadAABBFromCSV(const std::string& csvPath);
	
	// ２，AABB.csvがなければモデルデータから作成
	AABB CreateLocalAABB(const ModelData& model);

	// ３、AABBをCSVに保存
	void SaveAABBToCSV(const std::string& csvPath, const std::vector<AABB>& aabbs);


};

