#pragma once
#include "definition/definition.h"
#include <sstream>


class ModelManager
{
public:
	ModelManager();
	~ModelManager();



	ModelData LoadModelFile(const std::string& directoryPath, const std::string& filename);

	uint32_t LoadModel(
		const std::string& directoryPath, 
		const std::string& filename,
		ID3D12Device* device);
    Object3D* GetModel(uint32_t modelID);
	std::vector<Object3D> objects;

private:
	std::vector<AABB> LoadAABBFromCSV(const std::string& csvPath);

	void SaveAABBToCSV(const std::string& csvPath, const std::vector<AABB>& aabbs);

	std::string LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);

	std::vector<AABB> LoadAABB(const std::string& csvPath, const ModelData& model);
	AABB CreateLocalAABB(const ModelData& model);


};

