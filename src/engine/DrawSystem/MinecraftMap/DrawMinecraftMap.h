#pragma once
#include "definition/definition.h"

class RenderData_MinecraftMap;
class DirectXManager;

class DrawMinecraftMap
{
public:
	DrawMinecraftMap(DirectXManager* dxManager);
	~DrawMinecraftMap();

	void Draw();
	void UpdateMapInstanceData(RenderData_MinecraftMap& renderData);

	/////////////// マップ描画用 ///////////////////
	TransformationMatrix mapWVPMatrix_;
	const uint32_t knumInstance_Map = MAX_BLOCK_X * MAX_BLOCK_Y * MAX_BLOCK_Z;
	std::vector<Transforms> mapTransforms_;
	Microsoft::WRL::ComPtr<ID3D12Resource> mapInstanceResource;
	TransformationMatrix* mapInstanceData;
	SRVAllocation srvAlloc;


private:

	// DirectXマネージャー
	DirectXManager* dxManager_ = nullptr;

	// カメラビュープロジェクション行列
	Matrix4x4 viewProjectionMatrix_;


};

