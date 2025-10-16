#include "DrawSystem.h"
#include "DirectX/DirectXManager.h"

DrawSystem::DrawSystem(DirectXManager* dxManager)
	:dxManager_(dxManager)
{
	// カメラマトリックス
	viewProjectionMatrix_ = Matrix4x4::MakeIdentity4x4();

	// ライト
	directionalLightResource_ = CreateConstantBufferResource(dxManager->GetDevice(), sizeof(DirectionalLight));
	directionalLightData_ = nullptr;
	directionalLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData_));
	directionalLightData_->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	directionalLightData_->direction = { 0.0f, -1.0f, 0.0f };
	directionalLightData_->intensity = 1.0f;

	// 描画コールカウント初期化
	drawCallIndex_ = 0;
	// 1フレームに呼び出せる描画コールの最大数
	kMaxDrawCallPerFrame_ = 1280;

	// 頂点リソース
	vertexResourceSize_ = static_cast<UINT>(sizeof(VertexData) * 1024); // 三角形
	vertexResource_ = CreateBufferResource(dxManager->GetDevice(), vertexResourceSize_);

	HRESULT hr = vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexMappedPtr_));
	assert(SUCCEEDED(hr));

	materialResources_.resize(kMaxDrawCallPerFrame_);
	materialData_.resize(kMaxDrawCallPerFrame_);
	wvpResources_.resize(kMaxDrawCallPerFrame_);
	wvpData_.resize(kMaxDrawCallPerFrame_);
	for (size_t i = 0; i < kMaxDrawCallPerFrame_; ++i)
	{
		materialResources_[i] = CreateBufferResource(dxManager->GetDevice(), sizeof(Material));
		materialResources_[i]->Map(0, nullptr, reinterpret_cast<void**>(&materialData_[i]));
		wvpResources_[i] = CreateBufferResource(dxManager->GetDevice(), sizeof(TransformationMatrix));
		wvpResources_[i]->Map(0, nullptr, reinterpret_cast<void**>(&wvpData_[i]));
	}

	instancingSrvIndex_ = dxManager_->GetDescriptorHeapManager()->AllocateSRVSlot();
	EnsureInstanceBuffer(kNumInstance_);
}

DrawSystem::~DrawSystem()
{
	for (size_t i = 0; i < kMaxDrawCallPerFrame_; ++i)
	{
		if (materialResources_[i])
		{
			materialResources_[i]->Unmap(0, nullptr);
			materialResources_[i].Reset();
			materialData_[i] = nullptr;
		}
		if (wvpResources_[i])
		{
			wvpResources_[i]->Unmap(0, nullptr);
			wvpResources_[i].Reset();
			wvpData_[i] = nullptr;
		}
	}

	if (instancingResource_)
	{
		instancingResource_->Unmap(0, nullptr);
		instancingResource_.Reset();
		instancingData_ = nullptr;
		instancingCapacity_ = 0;
		instancingSrvHandleCPU_ = {};
		instancingSrvHandleGPU_ = {};
	}

	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>>().swap(materialResources_);
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>>().swap(wvpResources_);

	vertexResource_.Reset();
	directionalLightResource_.Reset();
}

void DrawSystem::BeginFrame(Matrix4x4& viewProjectionMatrix)
{
	// 描画コールの初期化
	drawCallIndex_ = 0;

	// 三角形用頂点データの初期化
	vertexDataUsed_ = 0;

	// カメラマトリックスの更新
	viewProjectionMatrix_ = viewProjectionMatrix;

	// ライトの向きを正規化
	directionalLightData_->direction = (directionalLightData_->direction.Normalized());

}

void DrawSystem::DrawParticle(Game::RenderData_Particle& renderData)
{
	//{
	//	// 描画回数上限
	//	if (drawCallIndex_ >= kMaxDrawCallPerFrame_) return;
	//
	//	// モデルの検索
	//	Object3D* obj = dxManager_->GetResourceManager()->GetModelManager()->GetModel(renderData.model);
	//	if (!obj) return;
	//
	//	// テクスチャの検索
	//	const TextureData* tex = dxManager_->GetResourceManager()->GetTextureManager()->GetTexture(renderData.texture);
	//	if (!tex) return;
	//
	//	// RootSignatureとPSOを設定
	//	dxManager_->GetCommandList()->SetGraphicsRootSignature(dxManager_->GetPipelineStateManager()->GetRootSignature_particle()); // 共通のルートシグネチャ
	//	if (renderData.options.wireframe || wireframeMode_)
	//	{	// ワイヤーフレーム用PSOを設定
	//		dxManager_->GetCommandList()->SetPipelineState(dxManager_->GetPipelineStateManager()->GetParticlePipelineState(BlendMode::kBlendModeNormal));
	//	}
	//	else
	//	{	// Triangle用PSOを設定
	//		dxManager_->GetCommandList()->SetPipelineState(dxManager_->GetPipelineStateManager()->GetParticlePipelineState(renderData.options.blendMode));
	//	}
	//
	//	// 頂点数の取得
	//	const uint32_t kSumVertex = static_cast<uint32_t>(obj->modelData.vertices.size());
	//
	//	// WVP行列
	//	for (uint32_t i = 0; i < kNumInstance_; ++i)
	//	{
	//		Matrix4x4 world = renderData.transforms.World;
	//		instancingData_[i].World = world;
	//		instancingData_[i].WVP = world * viewProjectionMatrix_;
	//	}
	//
	//	// マテリアル
	//	Vector4 color = ConvertUintToVector4(renderData.color);
	//	materialData_[drawCallIndex_]->color = color;
	//	materialData_[drawCallIndex_]->enableLighting = renderData.options.enableLighting;
	//	Matrix4x4 uvTransformMatrix = Matrix4x4::MakeIdentity4x4();
	//	uvTransformMatrix = (uvTransformMatrix * Matrix4x4::MakeScaleMatrix(renderData.uvTransform.scale));
	//	uvTransformMatrix = (uvTransformMatrix * Matrix4x4::MakeRotateZMatrix(renderData.uvTransform.rotate.z));
	//	uvTransformMatrix = (uvTransformMatrix * Matrix4x4::MakeTranslateMatrix(renderData.uvTransform.translate));
	//	materialData_[drawCallIndex_]->uvTransform = uvTransformMatrix;
	//
	//	// 頂点バッファをバインド（描画に使う頂点データを指定）
	//	dxManager_->GetCommandList()->IASetVertexBuffers(0, 1, &obj->vertexBufferView);
	//	// プリミティブトポロジ（描画する形状の種類：三角形リスト）を設定
	//	dxManager_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//	// ルートパラメータ0にマテリアル用定数バッファ（色・ライティング情報など）をバインド
	//	dxManager_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResources_[drawCallIndex_]->GetGPUVirtualAddress());
	//	// ルートパラメータ1にWVP（ワールド・ビュー・プロジェクション）用定数バッファをバインド
	//	//dxManager_->GetCommandList()->SetGraphicsRootConstantBufferView(1, instancingResource_->GetGPUVirtualAddress());
	//	// ルートパラメータ3にディレクショナルライト用定数バッファをバインド
	//	dxManager_->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource_->GetGPUVirtualAddress());
	//
	//	// ルートパラメータ4にインスタンシング用SRVをバインド
	//	dxManager_->GetCommandList()->SetGraphicsRootDescriptorTable(4, instancingSrvHandleGPU_);
	//	// ルートパラメータ2にテクスチャのSRV（シェーダリソースビュー）をバインド
	//	dxManager_->GetCommandList()->SetGraphicsRootDescriptorTable(2, tex->textureSrvHandleGPU);
	//	// 頂点数分のインスタンス描画を実行（実際に描画コマンドを発行）
	//	dxManager_->GetCommandList()->DrawInstanced(kSumVertex, kNumInstance_, 0, 0);
	//
	//	drawCallIndex_++;
	//}
	// 描画回数上限
	if (drawCallIndex_ >= kMaxDrawCallPerFrame_) return;
	
	// モデルの検索
	Object3D* obj = dxManager_->GetResourceManager()->GetModelManager()->GetModel(renderData.mono.model);
	if (!obj) return;
	
	// テクスチャの検索
	const TextureData* tex = dxManager_->GetResourceManager()->GetTextureManager()->GetTexture(renderData.mono.texture);
	if (!tex) return;
	
	// RootSignatureとPSOを設定
	dxManager_->GetCommandList()->SetGraphicsRootSignature(dxManager_->GetPipelineStateManager()->GetRootSignature_particle()); // 共通のルートシグネチャ
	if (renderData.mono.options.wireframe || wireframeMode_)
	{	// ワイヤーフレーム用PSOを設定
		dxManager_->GetCommandList()->SetPipelineState(dxManager_->GetPipelineStateManager()->GetParticlePipelineState(BlendMode::kBlendModeNormal));
	}
	else
	{	// Triangle用PSOを設定
		dxManager_->GetCommandList()->SetPipelineState(dxManager_->GetPipelineStateManager()->GetParticlePipelineState(renderData.mono.options.blendMode));
	}
	
	// 頂点数の取得
	const uint32_t kSumVertex = static_cast<uint32_t>(obj->modelData.vertices.size());
	
	
	Vector4 color = ConvertUintToVector4(renderData.mono.color);
	materialData_[drawCallIndex_]->color = color;
	materialData_[drawCallIndex_]->enableLighting = renderData.mono.options.enableLighting;
	Matrix4x4 uvTransformMatrix = Matrix4x4::MakeIdentity4x4();
	uvTransformMatrix = (uvTransformMatrix * Matrix4x4::MakeScaleMatrix(renderData.mono.uvTransform.scale));
	uvTransformMatrix = (uvTransformMatrix * Matrix4x4::MakeRotateZMatrix(renderData.mono.uvTransform.rotate.z));
	uvTransformMatrix = (uvTransformMatrix * Matrix4x4::MakeTranslateMatrix(renderData.mono.uvTransform.translate));
	materialData_[drawCallIndex_]->uvTransform = uvTransformMatrix;
	
	// WVP行列
	for (uint32_t i = 0; i < kNumInstance_; ++i)
	{
		Matrix4x4 world = renderData.mono.transforms.World;
		instancingData_[i].World = world;
		instancingData_[i].World.m[3][0] += i * 20;
		instancingData_[i].WVP = world * viewProjectionMatrix_;
	}

	// 頂点バッファをバインド（描画に使う頂点データを指定）
	dxManager_->GetCommandList()->IASetVertexBuffers(0, 1, &obj->vertexBufferView);
	// プリミティブトポロジ（描画する形状の種類：三角形リスト）を設定
	dxManager_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// ルートパラメータ0にマテリアル用定数バッファ（色・ライティング情報など）をバインド
	dxManager_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResources_[drawCallIndex_]->GetGPUVirtualAddress());
	// ルートパラメータ1にWVP（ワールド・ビュー・プロジェクション）用定数バッファをバインド
	//dxManager_->GetCommandList()->SetGraphicsRootConstantBufferView(1, instancingResource_->GetGPUVirtualAddress());
	// ルートパラメータ3にディレクショナルライト用定数バッファをバインド
	dxManager_->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource_->GetGPUVirtualAddress());

	// ルートパラメータ4にインスタンシング用SRVをバインド
	dxManager_->GetCommandList()->SetGraphicsRootDescriptorTable(4, dxManager_->GetDescriptorHeapManager()->GetGPUHandleAt(instancingSrvIndex_));
	// ルートパラメータ2にテクスチャのSRV（シェーダリソースビュー）をバインド
	dxManager_->GetCommandList()->SetGraphicsRootDescriptorTable(2, tex->textureSrvHandleGPU);
	// 頂点数分のインスタンス描画を実行（実際に描画コマンドを発行）
	dxManager_->GetCommandList()->DrawInstanced(kSumVertex, kNumInstance_, 0, 0);

	drawCallIndex_++;
}

void DrawSystem::DrawModel(Game::RenderData_Model& renderData)
{
	// 画面内か判定
	if (!renderData.inPicture)return;

	// 描画回数上限
	if (drawCallIndex_ >= kMaxDrawCallPerFrame_) return;

	// モデルの検索
	Object3D* obj = dxManager_->GetResourceManager()->GetModelManager()->GetModel(renderData.model);
	if (!obj) return;

	// テクスチャの検索
	const TextureData* tex = dxManager_->GetResourceManager()->GetTextureManager()->GetTexture(renderData.texture);
	if (!tex) return;

	// RootSignatureとPSOを設定
	dxManager_->GetCommandList()->SetGraphicsRootSignature(dxManager_->GetPipelineStateManager()->GetRootSignature()); // 共通のルートシグネチャ
	if (renderData.options.wireframe || wireframeMode_)
	{	// ワイヤーフレーム用PSOを設定
		dxManager_->GetCommandList()->SetPipelineState(dxManager_->GetPipelineStateManager()->GetPipelineState(BlendMode::Wireframe, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE));
	}
	else
	{	// Triangle用PSOを設定
		dxManager_->GetCommandList()->SetPipelineState(dxManager_->GetPipelineStateManager()->GetPipelineState(renderData.options.blendMode, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE));
	}

	// 頂点数の取得
	const uint32_t kSumVertex = static_cast<uint32_t>(obj->modelData.vertices.size());

	// WVP行列
	wvpData_[drawCallIndex_]->World = renderData.transforms.World;
	wvpData_[drawCallIndex_]->WVP = renderData.transforms.World * viewProjectionMatrix_;

	// マテリアル
	Vector4 color = ConvertUintToVector4(renderData.color);
	materialData_[drawCallIndex_]->color = color;
	materialData_[drawCallIndex_]->enableLighting = renderData.options.enableLighting;
	Matrix4x4 uvTransformMatrix = Matrix4x4::MakeIdentity4x4();
	uvTransformMatrix = (uvTransformMatrix * Matrix4x4::MakeScaleMatrix(renderData.uvTransform.scale));
	uvTransformMatrix = (uvTransformMatrix * Matrix4x4::MakeRotateZMatrix(renderData.uvTransform.rotate.z));
	uvTransformMatrix = (uvTransformMatrix * Matrix4x4::MakeTranslateMatrix(renderData.uvTransform.translate));
	materialData_[drawCallIndex_]->uvTransform = uvTransformMatrix;

	// 頂点バッファをバインド（描画に使う頂点データを指定）
	dxManager_->GetCommandList()->IASetVertexBuffers(0, 1, &obj->vertexBufferView);
	// プリミティブトポロジ（描画する形状の種類：三角形リスト）を設定
	dxManager_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// ルートパラメータ0にマテリアル用定数バッファ（色・ライティング情報など）をバインド
	dxManager_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResources_[drawCallIndex_]->GetGPUVirtualAddress());
	// ルートパラメータ1にWVP（ワールド・ビュー・プロジェクション）用定数バッファをバインド
	dxManager_->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResources_[drawCallIndex_]->GetGPUVirtualAddress());
	// ルートパラメータ2にテクスチャのSRV（シェーダリソースビュー）をバインド
	dxManager_->GetCommandList()->SetGraphicsRootDescriptorTable(2, tex->textureSrvHandleGPU);
	// ルートパラメータ3にディレクショナルライト用定数バッファをバインド
	dxManager_->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource_->GetGPUVirtualAddress());
	// 頂点数分のインスタンス描画を実行（実際に描画コマンドを発行）
	dxManager_->GetCommandList()->DrawInstanced(kSumVertex, 1, 0, 0);

	drawCallIndex_++;
}


void DrawSystem::DrawTriangle(Game::RenderData_Triangle& renderData)
{
	// 描画回数上限
	if (drawCallIndex_ >= kMaxDrawCallPerFrame_) return;

	// テクスチャの検索
	const TextureData* tex = dxManager_->GetResourceManager()->GetTextureManager()->GetTexture(renderData.texture);
	if (!tex) return;

	// RootSignatureとPSOを設定
	dxManager_->GetCommandList()->SetGraphicsRootSignature(dxManager_->GetPipelineStateManager()->GetRootSignature()); // 共通のルートシグネチャ
	if (renderData.options.wireframe || wireframeMode_)
	{	// ワイヤーフレーム用PSOを設定
		dxManager_->GetCommandList()->SetPipelineState(dxManager_->GetPipelineStateManager()->GetPipelineState(BlendMode::Wireframe, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE));
	}
	else
	{	// Triangle用PSOを設定
		dxManager_->GetCommandList()->SetPipelineState(dxManager_->GetPipelineStateManager()->GetPipelineState(renderData.options.blendMode, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE));
	}

	// 頂点数の取得
	const uint32_t kSumVertex = 3;
	// 必要な頂点数分配列を拡張
	if (vertexDataUsed_ + kSumVertex > vertexData_.size())
	{
		vertexData_.resize(vertexDataUsed_ + kSumVertex);
	}

	// 上
	vertexData_[vertexDataUsed_ + 0].position = { renderData.pos1.x, renderData.pos1.y, renderData.pos1.z, 1.0f };
	vertexData_[vertexDataUsed_ + 0].texcoord = { 0.5f, 0.0f };
	vertexData_[vertexDataUsed_ + 0].normal = { 0.0f, 0.0f, -1.0f };

	// 右下
	vertexData_[vertexDataUsed_ + 1].position = { renderData.pos2.x, renderData.pos2.y, renderData.pos2.z, 1.0f };
	vertexData_[vertexDataUsed_ + 1].texcoord = { 1.0f, 1.0f };
	vertexData_[vertexDataUsed_ + 1].normal = { 0.0f, 0.0f, -1.0f };

	// 左下
	vertexData_[vertexDataUsed_ + 2].position = { renderData.pos3.x,renderData.pos3.y, renderData.pos3.z, 1.0f };
	vertexData_[vertexDataUsed_ + 2].texcoord = { 0.0f, 1.0f };
	vertexData_[vertexDataUsed_ + 2].normal = { 0.0f, 0.0f, -1.0f };

	// WVP行列
	Matrix4x4 world = Matrix4x4::MakeAffineMatrix(renderData.transform.scale, renderData.transform.rotate, renderData.transform.translate);
	Matrix4x4 wvpMatrix = world * viewProjectionMatrix_;

	wvpData_[drawCallIndex_]->World = world;
	wvpData_[drawCallIndex_]->WVP = wvpMatrix;

	// マテリアル
	Vector4 color = ConvertUintToVector4(renderData.color);
	materialData_[drawCallIndex_]->color = color;
	materialData_[drawCallIndex_]->enableLighting = renderData.options.enableLighting;
	Matrix4x4 uvTransformMatrix = Matrix4x4::MakeIdentity4x4();
	uvTransformMatrix = (uvTransformMatrix * Matrix4x4::MakeScaleMatrix(renderData.uvTransform.scale));
	uvTransformMatrix = (uvTransformMatrix * Matrix4x4::MakeRotateZMatrix(renderData.uvTransform.rotate.z));
	uvTransformMatrix = (uvTransformMatrix * Matrix4x4::MakeTranslateMatrix(renderData.uvTransform.translate));
	materialData_[drawCallIndex_]->uvTransform = uvTransformMatrix;

	// 頂点リソース
	VertexData* vData = nullptr;
	HRESULT hr = vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vData));
	if (FAILED(hr) || vData == nullptr) return;
	std::memcpy(vData + vertexDataUsed_, &vertexData_[vertexDataUsed_], sizeof(VertexData) * kSumVertex);
	vertexResource_->Unmap(0, nullptr);

	// 動的頂点バッファを確保
	if (!EnsureDynamicVB(vertexDataUsed_ + kSumVertex)) return;
	memcpy(vertexMappedPtr_ + vertexDataUsed_, &vertexData_[vertexDataUsed_], sizeof(VertexData) * kSumVertex);

	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	vertexBufferView.BufferLocation = vertexResource_->GetGPUVirtualAddress() + sizeof(VertexData) * (vertexDataUsed_);
	vertexBufferView.SizeInBytes = sizeof(VertexData) * static_cast<UINT>(kSumVertex);
	vertexBufferView.StrideInBytes = sizeof(VertexData);


	// RootSignatureを設定。
	dxManager_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);
	// 形状を設定
	dxManager_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// CBVを設定する マテリアル用のCBufferの場所を設定
	dxManager_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResources_[drawCallIndex_]->GetGPUVirtualAddress());
	// CBVを設定する wvp用のCBufferの場所を設定
	dxManager_->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResources_[drawCallIndex_]->GetGPUVirtualAddress());
	// SRVのDescriptorTableの先頭を設定。２はrootParameters[2]。
	dxManager_->GetCommandList()->SetGraphicsRootDescriptorTable(2, tex->textureSrvHandleGPU);
	// CBVを設定する ディレクショナルライト用のCBufferの場所を設定
	dxManager_->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource_->GetGPUVirtualAddress());

	// 描画
	dxManager_->GetCommandList()->DrawInstanced(3, 1, 0, 0);


	drawCallIndex_++;
	vertexDataUsed_ += kSumVertex;
}

void DrawSystem::DrawSprite(Game::RenderData_Sprite& renderData)
{
	// 描画回数上限
	if (drawCallIndex_ >= kMaxDrawCallPerFrame_) return;

	// テクスチャの検索
	const TextureData* tex = dxManager_->GetResourceManager()->GetTextureManager()->GetTexture(renderData.texture);
	if (!tex) return;

	// RootSignatureとPSOを設定 - Triangle
	dxManager_->GetCommandList()->SetGraphicsRootSignature(dxManager_->GetPipelineStateManager()->GetRootSignature()); // 共通のルートシグネチャ
	dxManager_->GetCommandList()->SetPipelineState(dxManager_->GetPipelineStateManager()->GetPipelineState(renderData.options.blendMode, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE)); // Triangle用PSOを設定

	// 必要な頂点数
	const uint32_t kSumVertex = 4;
	// 必要な頂点数分配列を拡張
	if (vertexDataUsed_ + kSumVertex > vertexData_.size())
	{
		vertexData_.resize(vertexDataUsed_ + kSumVertex);
	}

	// 頂点
	float halfWidth = static_cast<float>(tex->metadata.width) * 0.5f;
	float halfHeight = static_cast<float>(tex->metadata.height) * 0.5f;

	// 左下 (index 0)
	vertexData_[vertexDataUsed_ + 0].position = { -halfWidth, -halfHeight, 0.0f, 1.0f };
	vertexData_[vertexDataUsed_ + 0].texcoord = { 0.0f, 0.0f };
	vertexData_[vertexDataUsed_ + 0].normal = { 0.0f, 0.0f, -1.0f };

	// 左上 (index 1)
	vertexData_[vertexDataUsed_ + 1].position = { halfWidth, -halfHeight, 0.0f, 1.0f };
	vertexData_[vertexDataUsed_ + 1].texcoord = { 1.0f, 0.0f };
	vertexData_[vertexDataUsed_ + 1].normal = { 0.0f, 0.0f, -1.0f };

	// 右下 (index 2)
	vertexData_[vertexDataUsed_ + 2].position = { -halfWidth, halfHeight, 0.0f, 1.0f };
	vertexData_[vertexDataUsed_ + 2].texcoord = { 0.0f, 1.0f };
	vertexData_[vertexDataUsed_ + 2].normal = { 0.0f, 0.0f, -1.0f };

	// 右上 (index 3)
	vertexData_[vertexDataUsed_ + 3].position = { halfWidth, halfHeight, 0.0f, 1.0f };
	vertexData_[vertexDataUsed_ + 3].texcoord = { 1.0f, 1.0f };
	vertexData_[vertexDataUsed_ + 3].normal = { 0.0f, 0.0f, -1.0f };

	// アンカーによる位置調整
	switch (renderData.anker)
	{
	case Anker::Center:
	{
		// 左下 (index 0)
		vertexData_[vertexDataUsed_ + 0].position.x;
		vertexData_[vertexDataUsed_ + 0].position.y;
		// 左上 (index 1)
		vertexData_[vertexDataUsed_ + 1].position.x;
		vertexData_[vertexDataUsed_ + 1].position.y;
		// 右下 (index 2)
		vertexData_[vertexDataUsed_ + 2].position.x;
		vertexData_[vertexDataUsed_ + 2].position.y;
		// 右上 (index 3)
		vertexData_[vertexDataUsed_ + 3].position.x;
		vertexData_[vertexDataUsed_ + 3].position.y;
		break;
	}
	case Anker::CenterLeft:
	{
		// 左下 (index 0)
		vertexData_[vertexDataUsed_ + 0].position.x += halfWidth;
		vertexData_[vertexDataUsed_ + 0].position.y;
		// 左上 (index 1)
		vertexData_[vertexDataUsed_ + 1].position.x += halfWidth;
		vertexData_[vertexDataUsed_ + 1].position.y;
		// 右下 (index 2)
		vertexData_[vertexDataUsed_ + 2].position.x += halfWidth;
		vertexData_[vertexDataUsed_ + 2].position.y;
		// 右上 (index 3)
		vertexData_[vertexDataUsed_ + 3].position.x += halfWidth;
		vertexData_[vertexDataUsed_ + 3].position.y;
		break;
	}
	case Anker::CenterRight:
	{
		// 左下 (index 0)
		vertexData_[vertexDataUsed_ + 0].position.x += -halfWidth;
		vertexData_[vertexDataUsed_ + 0].position.y;
		// 左上 (index 1)
		vertexData_[vertexDataUsed_ + 1].position.x += -halfWidth;
		vertexData_[vertexDataUsed_ + 1].position.y;
		// 右下 (index 2)
		vertexData_[vertexDataUsed_ + 2].position.x += -halfWidth;
		vertexData_[vertexDataUsed_ + 2].position.y;
		// 右上 (index 3)
		vertexData_[vertexDataUsed_ + 3].position.x += -halfWidth;
		vertexData_[vertexDataUsed_ + 3].position.y;
		break;
	}
	case Anker::CenterTop:
	{
		// 左下 (index 0)
		vertexData_[vertexDataUsed_ + 0].position.x;
		vertexData_[vertexDataUsed_ + 0].position.y += halfHeight;
		// 左上 (index 1)
		vertexData_[vertexDataUsed_ + 1].position.x;
		vertexData_[vertexDataUsed_ + 1].position.y += halfHeight;
		// 右下 (index 2)
		vertexData_[vertexDataUsed_ + 2].position.x;
		vertexData_[vertexDataUsed_ + 2].position.y += halfHeight;
		// 右上 (index 3)
		vertexData_[vertexDataUsed_ + 3].position.x;
		vertexData_[vertexDataUsed_ + 3].position.y += halfHeight;
		break;
	}
	case Anker::CenterDown:
	{
		// 左下 (index 0)
		vertexData_[vertexDataUsed_ + 0].position.x;
		vertexData_[vertexDataUsed_ + 0].position.y += -halfHeight;
		// 左上 (index 1)
		vertexData_[vertexDataUsed_ + 1].position.x;
		vertexData_[vertexDataUsed_ + 1].position.y += -halfHeight;
		// 右下 (index 2)
		vertexData_[vertexDataUsed_ + 2].position.x;
		vertexData_[vertexDataUsed_ + 2].position.y += -halfHeight;
		// 右上 (index 3)
		vertexData_[vertexDataUsed_ + 3].position.x;
		vertexData_[vertexDataUsed_ + 3].position.y += -halfHeight;
		break;
	}
	case Anker::LeftTop:
	{
		// 左下 (index 0)
		vertexData_[vertexDataUsed_ + 0].position.x += halfWidth;
		vertexData_[vertexDataUsed_ + 0].position.y += halfHeight;
		// 左上 (index 1)
		vertexData_[vertexDataUsed_ + 1].position.x += halfWidth;
		vertexData_[vertexDataUsed_ + 1].position.y += halfHeight;
		// 右下 (index 2)
		vertexData_[vertexDataUsed_ + 2].position.x += halfWidth;
		vertexData_[vertexDataUsed_ + 2].position.y += halfHeight;
		// 右上 (index 3)
		vertexData_[vertexDataUsed_ + 3].position.x += halfWidth;
		vertexData_[vertexDataUsed_ + 3].position.y += halfHeight;
		break;
	}
	case Anker::RightTop:
	{
		// 左下 (index 0)
		vertexData_[vertexDataUsed_ + 0].position.x += -halfWidth;
		vertexData_[vertexDataUsed_ + 0].position.y += halfHeight;
		// 左上 (index 1)
		vertexData_[vertexDataUsed_ + 1].position.x += -halfWidth;
		vertexData_[vertexDataUsed_ + 1].position.y += halfHeight;
		// 右下 (index 2)
		vertexData_[vertexDataUsed_ + 2].position.x += -halfWidth;
		vertexData_[vertexDataUsed_ + 2].position.y += halfHeight;
		// 右上 (index 3)
		vertexData_[vertexDataUsed_ + 3].position.x += -halfWidth;
		vertexData_[vertexDataUsed_ + 3].position.y += halfHeight;
		break;
	}
	case Anker::LeftDown:
	{
		// 左下 (index 0)
		vertexData_[vertexDataUsed_ + 0].position.x += halfWidth;
		vertexData_[vertexDataUsed_ + 0].position.y += -halfHeight;
		// 左上 (index 1)
		vertexData_[vertexDataUsed_ + 1].position.x += halfWidth;
		vertexData_[vertexDataUsed_ + 1].position.y += -halfHeight;
		// 右下 (index 2)
		vertexData_[vertexDataUsed_ + 2].position.x += halfWidth;
		vertexData_[vertexDataUsed_ + 2].position.y += -halfHeight;
		// 右上 (index 3)
		vertexData_[vertexDataUsed_ + 3].position.x += halfWidth;
		vertexData_[vertexDataUsed_ + 3].position.y += -halfHeight;
		break;
	}
	case Anker::RightDown:
	{
		// 左下 (index 0)
		vertexData_[vertexDataUsed_ + 0].position.x += -halfWidth;
		vertexData_[vertexDataUsed_ + 0].position.y += -halfHeight;
		// 左上 (index 1)
		vertexData_[vertexDataUsed_ + 1].position.x += -halfWidth;
		vertexData_[vertexDataUsed_ + 1].position.y += -halfHeight;
		// 右下 (index 2)
		vertexData_[vertexDataUsed_ + 2].position.x += -halfWidth;
		vertexData_[vertexDataUsed_ + 2].position.y += -halfHeight;
		// 右上 (index 3)
		vertexData_[vertexDataUsed_ + 3].position.x += -halfWidth;
		vertexData_[vertexDataUsed_ + 3].position.y += -halfHeight;
		break;
	}
	default:
		break;
	}


	// WVP行列
	Matrix4x4 orthoProjectionMatrix = Matrix4x4::MakeOrthographicMatrix(
		0.0f, 0.0f,
		//static_cast<float>(windowManager->Getwidth()),
		//static_cast<float>(windowManager->Getheight()),
		static_cast<float>(1280),
		static_cast<float>(720),
		0.0f, 100.0f);
	Matrix4x4 world = Matrix4x4::MakeAffineMatrix(renderData.transforms.scale, renderData.transforms.rotate, renderData.transforms.translate);
	Matrix4x4 wvpMatrix = (world * orthoProjectionMatrix);

	wvpData_[drawCallIndex_]->World = world;
	wvpData_[drawCallIndex_]->WVP = wvpMatrix;


	// マテリアル
	float uvCenterX = (renderData.pivot.x) - (halfWidth);
	float uvCenterY = (renderData.pivot.y) - (halfHeight);
	uvCenterX = uvCenterX / halfWidth / 2;
	uvCenterY = uvCenterY / halfHeight / 2;

	Matrix4x4 toCenter = Matrix4x4::MakeTranslateMatrix({ -uvCenterX, -uvCenterY, 0.0f });
	Matrix4x4 fromCenter = Matrix4x4::MakeTranslateMatrix({ uvCenterX, uvCenterY, 0.0f });
	Matrix4x4 uvTransformMatrix = Matrix4x4::MakeIdentity4x4();
	uvTransformMatrix = (uvTransformMatrix * Matrix4x4::MakeScaleMatrix(renderData.uvTransform.scale));
	uvTransformMatrix = (uvTransformMatrix * Matrix4x4::MakeRotateZMatrix(renderData.uvTransform.rotate.z));

	uvTransformMatrix = (fromCenter * (uvTransformMatrix * toCenter));

	uvTransformMatrix = (uvTransformMatrix * Matrix4x4::MakeTranslateMatrix(renderData.uvTransform.translate));


	Vector4 color = ConvertUintToVector4(renderData.color);
	materialData_[drawCallIndex_]->color = color;
	materialData_[drawCallIndex_]->enableLighting = false;
	materialData_[drawCallIndex_]->uvTransform = uvTransformMatrix;

	// スプライトの中心座標
	Vector2 center = { renderData.transforms.translate.x, renderData.transforms.translate.y };


	halfWidth *= renderData.transforms.scale.x;
	halfHeight *= renderData.transforms.scale.y;

	// アンカーに応じて中心座標を補正
	switch (renderData.anker)
	{
	case Anker::CenterLeft:
		center.x += halfWidth;
		break;
	case Anker::CenterRight:
		center.x -= halfWidth;
		break;
	case Anker::CenterTop:
		center.y += halfHeight;
		break;
	case Anker::CenterDown:
		center.y -= halfHeight;
		break;
	case Anker::LeftTop:
		center.x += halfWidth;
		center.y += halfHeight;
		break;
	case Anker::RightTop:
		center.x -= halfWidth;
		center.y += halfHeight;
		break;
	case Anker::LeftDown:
		center.x += halfWidth;
		center.y -= halfHeight;
		break;
	case Anker::RightDown:
		center.x -= halfWidth;
		center.y -= halfHeight;
		break;
	default:
		break;
	}

	// スプライトのAABB
	float left = center.x - halfWidth;
	float right = center.x + halfWidth;
	float top = center.y - halfHeight;
	float bottom = center.y + halfHeight;

	// マウス座標取得
	Vector2 mousePos = Game::GetMousePosition();

	// 当たり判定
	renderData.isCollisionMouseRay = (mousePos.x >= left && mousePos.x <= right && mousePos.y >= top && mousePos.y <= bottom);


	// 動的頂点バッファを確保
	if (!EnsureDynamicVB(vertexDataUsed_ + kSumVertex)) return;
	memcpy(vertexMappedPtr_ + vertexDataUsed_, &vertexData_[vertexDataUsed_], sizeof(VertexData) * kSumVertex);

	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	vertexBufferView.BufferLocation = vertexResource_->GetGPUVirtualAddress() + sizeof(VertexData) * (vertexDataUsed_);
	vertexBufferView.SizeInBytes = sizeof(VertexData) * static_cast<UINT>(kSumVertex);
	vertexBufferView.StrideInBytes = sizeof(VertexData);

	// Spriteの描画
	dxManager_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);
	//dxManager_->GetCommandList()->IASetIndexBuffer(&indexBufferView_);
	// 形状を設定
	dxManager_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// CBVを設定する マテリアル用のCBufferの場所を設定
	dxManager_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResources_[drawCallIndex_]->GetGPUVirtualAddress());
	// CBVを設定する wvp用のCBufferの場所を設定
	dxManager_->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResources_[drawCallIndex_]->GetGPUVirtualAddress());
	// SRVのDescriptorTableの先頭を設定。２はrootParameters[2]。
	dxManager_->GetCommandList()->SetGraphicsRootDescriptorTable(2, tex->textureSrvHandleGPU);
	// CBVを設定する ディレクショナルライト用のCBufferの場所を設定
	dxManager_->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource_->GetGPUVirtualAddress());

	// 描画
	dxManager_->GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);

	drawCallIndex_++;
	vertexDataUsed_ += kSumVertex;
}

// 引数にstd::vector<Vector3>とベジエとか直線とか選択できるenumを渡して複数の線を描画できるようにするのもありかも
void DrawSystem::DrawLine(const Vector3& start, const Vector3& end, const uint32_t& materialColor)
{
	if (drawCallIndex_ >= kMaxDrawCallPerFrame_) return;

	// RootSignatureとPSOを設定 - Line
	dxManager_->GetCommandList()->SetGraphicsRootSignature(dxManager_->GetPipelineStateManager()->GetRootSignature()); // 共通のルートシグネチャ
	dxManager_->GetCommandList()->SetPipelineState(dxManager_->GetPipelineStateManager()->GetPipelineState(BlendMode::kBlendModeNormal, D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE)); // Line用PSOを設定

	// 頂点数の取得
	const uint32_t kSumVertex = 2;
	// 必要な頂点数分配列を拡張
	if (vertexDataUsed_ + kSumVertex > vertexData_.size())
	{
		vertexData_.resize(vertexDataUsed_ + kSumVertex);
	}

	// 始点
	vertexData_[vertexDataUsed_ + 0].position = { start.x, start.y, start.z, 1.0f };

	// 終点
	vertexData_[vertexDataUsed_ + 1].position = { end.x, end.y, end.z, 1.0f };

	// WVP行列
	Matrix4x4 world = Matrix4x4::MakeIdentity4x4();
	Matrix4x4 wvpMatrix = viewProjectionMatrix_;

	wvpData_[drawCallIndex_]->World = world;
	wvpData_[drawCallIndex_]->WVP = wvpMatrix;

	// マテリアル定数バッファの更新
	Vector4 color = ConvertUintToVector4(materialColor);
	materialData_[drawCallIndex_]->color = color;
	materialData_[drawCallIndex_]->enableLighting = 0;
	materialData_[drawCallIndex_]->uvTransform = Matrix4x4::MakeIdentity4x4(); // 線にUV変換いらない

	// 頂点リソース
	VertexData* vData = nullptr;
	HRESULT hr = vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vData));
	if (FAILED(hr) || vData == nullptr) return;
	std::memcpy(vData + vertexDataUsed_, &vertexData_[vertexDataUsed_], sizeof(VertexData) * kSumVertex);
	vertexResource_->Unmap(0, nullptr);

	// 動的頂点バッファを確保
	if (!EnsureDynamicVB(vertexDataUsed_ + kSumVertex)) return;
	memcpy(vertexMappedPtr_ + vertexDataUsed_, &vertexData_[vertexDataUsed_], sizeof(VertexData) * kSumVertex);

	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	vertexBufferView.BufferLocation = vertexResource_->GetGPUVirtualAddress() + sizeof(VertexData) * (vertexDataUsed_);
	vertexBufferView.SizeInBytes = sizeof(VertexData) * static_cast<UINT>(kSumVertex);
	vertexBufferView.StrideInBytes = sizeof(VertexData);

	// RootSignatureを設定
	dxManager_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);
	// 形状を設定
	dxManager_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
	// CBVを設定する マテリアル用のCBufferの場所を設定
	dxManager_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResources_[drawCallIndex_]->GetGPUVirtualAddress());// b1にバインド
	// CBVを設定する wvp用のCBufferの場所を設定
	dxManager_->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResources_[drawCallIndex_]->GetGPUVirtualAddress()); // b0にバインド
	// 描画コマンドの発行
	dxManager_->GetCommandList()->DrawInstanced(2, 1, 0, 0);


	drawCallIndex_++;
	vertexDataUsed_ += kSumVertex;
}





bool DrawSystem::EnsureDynamicVB(size_t requiredVertexCount)
{
	// まだMapしていなければここで永続Map
	if (!vertexMappedPtr_ && vertexResource_)
	{
		HRESULT hr0 = vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexMappedPtr_));
		if (FAILED(hr0) || !vertexMappedPtr_) return false;
	}

	size_t requiredBytes = requiredVertexCount * sizeof(VertexData);
	if (requiredBytes <= vertexResourceSize_) return true;

	// 2倍成長でリサイズ
	size_t currentCount = vertexResourceSize_ / sizeof(VertexData);
	size_t newCount = my_max(requiredVertexCount, currentCount ? currentCount * 2 : size_t(1024));
	UINT newSizeBytes = static_cast<UINT>(newCount * sizeof(VertexData));

	auto newResource = CreateBufferResource(dxManager_->GetDevice(), newSizeBytes);
	if (!newResource) return false;

	// 新リソースを永続Map
	VertexData* newMapped = nullptr;
	HRESULT hr = newResource->Map(0, nullptr, reinterpret_cast<void**>(&newMapped));
	if (FAILED(hr) || !newMapped) return false;

	// 旧内容をコピー
	if (vertexMappedPtr_ && vertexDataUsed_ > 0)
	{
		memcpy(newMapped, vertexMappedPtr_, vertexDataUsed_ * sizeof(VertexData));
		vertexResource_->Unmap(0, nullptr);
	}

	vertexResource_ = newResource;
	vertexMappedPtr_ = newMapped;
	vertexResourceSize_ = newSizeBytes;
	return true;
}


bool DrawSystem::EnsureInstanceBuffer(size_t requiredInstanceCount)
{
	// 既存容量で足りる場合
	if (instancingResource_ && instancingCapacity_ >= requiredInstanceCount)
	{
		return true;
	}

	// 新しい容量（2倍成長＋最低64）
	uint32_t newCapacity = static_cast<uint32_t>(
		std::max<size_t>(requiredInstanceCount, instancingCapacity_ ? instancingCapacity_ * 2ull : 64ull)
		);
	size_t newSizeBytes = sizeof(TransformationMatrix) * static_cast<size_t>(newCapacity);

	// 新リソース作成（Uploadバッファ）
	auto newResource = CreateBufferResource(dxManager_->GetDevice() , newSizeBytes);
	if (!newResource) return false;

	TransformationMatrix* newMapped = nullptr;
	HRESULT hr = newResource->Map(0, nullptr, reinterpret_cast<void**>(&newMapped));
	if (FAILED(hr) || !newMapped) return false;

	// 古いリソースを解放
	if (instancingResource_)
	{
		instancingResource_->Unmap(0, nullptr);
		instancingResource_.Reset();
		instancingData_ = nullptr;
	}

	instancingResource_ = newResource;
	instancingData_ = newMapped;
	instancingCapacity_ = newCapacity;

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = newCapacity;
	srvDesc.Buffer.StructureByteStride = sizeof(TransformationMatrix);

	// 現在のヒープにおける該当インデックスのハンドルを取得して作り直す
	//dxManager->GetDescriptorHeapManager()->GetCPUHandleAt(dxManager->GetDescriptorHeapManager()->AllocateSRVSlot()),
	instancingSrvHandleCPU_ = dxManager_->GetDescriptorHeapManager()->GetCPUHandleAt(instancingSrvIndex_);
	instancingSrvHandleGPU_ = dxManager_->GetDescriptorHeapManager()->GetGPUHandleAt(instancingSrvIndex_);

	dxManager_->GetDevice()->CreateShaderResourceView(instancingResource_.Get(), &srvDesc, instancingSrvHandleCPU_);
	return true;
}