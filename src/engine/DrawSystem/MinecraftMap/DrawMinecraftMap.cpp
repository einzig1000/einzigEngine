#include "DrawSystem/MinecraftMap/DrawMinecraftMap.h"
#include "DirectX/DirectXManager.h"
#include "Window/WindowManager.h"
#include "Utilities/functions.h"
#include "Engine.h"
#include "Block/Block.h"


DrawMinecraftMap::DrawMinecraftMap(DirectXManager* dxManager)
{
	dxManager_ = dxManager;

	//// インスタンスデータ用リソースの作成
	//const uint32_t instanceDataSize = static_cast<uint32_t>(sizeof(TransformationMatrix) * knumInstance_Map);
	//mapInstanceResource = CreateBufferResource(dxManager_->GetDevice(), instanceDataSize, D3D12_RESOURCE_FLAG_NONE,
	//	D3D12_RESOURCE_STATE_GENERIC_READ, true);
	//HRESULT hr = mapInstanceResource->Map(0, nullptr, reinterpret_cast<void**>(&mapInstanceData));
	//assert(SUCCEEDED(hr));
	//// SRVの作成
	//srvAlloc = dxManager_->GetDescriptorHeapManager()->CreateSRV(
	//	mapInstanceResource.Get(),
	//	D3D12_RESOURCE_DIMENSION_BUFFER,
	//	instanceDataSize,
	//	sizeof(TransformationMatrix));
	//// インスタンス変換行列配列の初期化
	//mapTransforms_.resize(knumInstance_Map);
	//for (uint32_t x = 0; x < MAX_BLOCK_X; ++x)
	//{
	//	for (uint32_t y = 0; y < MAX_BLOCK_Y; ++y)
	//	{
	//		for (uint32_t z = 0; z < MAX_BLOCK_Z; ++z)
	//		{
	//			uint32_t index = x + y * MAX_BLOCK_X + z * MAX_BLOCK_X * MAX_BLOCK_Y;
	//			mapTransforms_[index].scale = { 1.0f, 1.0f, 1.0f };
	//			mapTransforms_[index].rotate = { 0.0f, 0.0f, 0.0f };
	//			mapTransforms_[index].translate = { static_cast<float>(x), static_cast<float>(y), static_cast<float>(z) };
	//		}
	//	}
	//}
}

void DrawMinecraftMap::Draw()
{
	//Engine::Instance().DrawMinecraftMap(*this);
}

void DrawMinecraftMap::UpdateMapInstanceData(RenderData_MinecraftMap& renderData)
{
	for (uint32_t i = 0; i < knumInstance_Map; ++i)
	{
		Matrix4x4 world = Matrix4x4::MakeAffineMatrix(
			mapTransforms_[i].scale,
			mapTransforms_[i].rotate,
			mapTransforms_[i].translate);
		Matrix4x4 wvp = world * viewProjectionMatrix_;
		mapInstanceData[i].World = world;
		mapInstanceData[i].WVP = wvp;
	}
}

//void DrawSystem::DrawMap(RenderData_MinecraftMap& renderData)
//{
//	if (drawCallIndex_ >= kMaxDrawCallPerFrame_) return;
//
//	// モデルの検索
//	Object3D* obj = dxManager_->GetResourceManager()->GetModelManager()->GetModelData(ResourceID::blockModelIDs_[uint32_t(ModelID::Cube)]);
//	if (!obj) return;
//
//	// テクスチャの検索
//	const TextureData* tex = dxManager_->GetResourceManager()->GetTextureManager()->GetTextureData(renderData.texture_);
//	if (!tex) return;
//
//	// RootSignatureを設定
//	dxManager_->GetCommandContextManager()->GetCommandList()->SetGraphicsRootSignature(dxManager_->GetPipelineStateManager()->GetRootSignature_minecraftMap());
//	// Triangle用PSOを設定
//	dxManager_->GetCommandContextManager()->GetCommandList()->SetPipelineState(dxManager_->GetPipelineStateManager()->GetMinecraftMapPipelineState(BlendMode::kBlendModeNormal));
//
//	// ライトの設定
//	lightData_[drawCallIndex_]->color = { 0xFF, 0xFF, 0xFF, 0xFF };
//	lightData_[drawCallIndex_]->mode = LightMode::None;
//	lightData_[drawCallIndex_]->phong = false;
//
//	// 頂点数の取得
//	const uint32_t kSumVertex = static_cast<uint32_t>(obj->modelData.vertices.size());
//
//	// マテリアルデータ
//	Vector4 color = ConvertUintToVector4(0xFFFFFFFF);
//	materialData_[drawCallIndex_]->color = color;
//	materialData_[drawCallIndex_]->shininess = 1.0f;
//	materialData_[drawCallIndex_]->uvTransform = Matrix4x4::MakeIdentity4x4();
//
//
//	// ルートパラメータ[0]にマテリアル用CBVを設定
//	dxManager_->GetCommandContextManager()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResources_[drawCallIndex_]->GetGPUVirtualAddress());
//	// ルートパラメータ[1]にWVP用CBVを設定
//	dxManager_->GetCommandContextManager()->GetCommandList()->SetGraphicsRootConstantBufferView(1, mapInstanceResource->GetGPUVirtualAddress());
//	// ルートパラメータ[2]にテクスチャSRVを設定
//	dxManager_->GetCommandContextManager()->GetCommandList()->SetGraphicsRootDescriptorTable(2, tex->textureSrvHandleGPU);
//	// ルートパラメータ[3]にライト用CBVを設定
//	dxManager_->GetCommandContextManager()->GetCommandList()->SetGraphicsRootConstantBufferView(3, lightResources_[drawCallIndex_]->GetGPUVirtualAddress());
//	// ルートパラメータ[4]にインスタンス用SRVを設定
//	dxManager_->GetCommandContextManager()->GetCommandList()->SetGraphicsRootDescriptorTable(4, srvAlloc.gpu);
//
//	// VertexBufferViewを設定
//	dxManager_->GetCommandContextManager()->GetCommandList()->IASetVertexBuffers(0, 1, &obj->vertexBufferView);
//	// 描画形状を設定
//	dxManager_->GetCommandContextManager()->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//	// 描画コールの発行
//	dxManager_->GetCommandContextManager()->GetCommandList()->DrawInstanced(kSumVertex, knumInstance_Map, 0, 0);
//
//	drawCallIndex_++;
//}
