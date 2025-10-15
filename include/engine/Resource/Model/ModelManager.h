#pragma once
#include "definition/definition.h"


class ModelManager
{
public:
	ModelManager();
	~ModelManager();


    uint32_t LoadModel(
		const std::string& directoryPath, 
		const std::string& filename,
		ID3D12Device* device);
    Object3D* GetModel(uint32_t modelID);
	std::vector<Object3D> objects;

private:

	std::vector<AABB> LoadAABB(const std::string& csvPath, const ModelData& model);
	AABB CreateLocalAABB(const ModelData& model);


};

