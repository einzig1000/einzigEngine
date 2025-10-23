#include "DrawSystem.h"
#include "DirectX/DirectXManager.h"
#include "Window/WindowManager.h"
#include "Utilities/functions.h"
#include "Camera/CameraController.h"

DrawSystem::DrawSystem(DirectXManager* dxManager)
	:dxManager_(dxManager)
{
	// 正射影行列
	orthoProjectionMatrix_ = Matrix4x4::MakeOrthographicMatrix(
		0.0f, 0.0f,
		static_cast<float>(WindowManager::winWidth_),
		static_cast<float>(WindowManager::winHeight_),
		0.0f, 100.0f);

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

	// 頂点リソース
	vertexResourceSize_ = static_cast<UINT>(sizeof(VertexData) * 1024); // 三角形
	vertexResource_ = CreateBufferResource(dxManager->GetDevice(), vertexResourceSize_);
	HRESULT hr = vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexMappedPtr_));
	assert(SUCCEEDED(hr));

	kMaxDrawCallPerFrame_ = 1024;
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
	vertexDataUsed_ = 0;

	// パーティクル用：単一の連続バッファ + SRV(t1)
	{
		//const uint32_t capacity = kMaxInstanceCount_;
		instancingResource_ = CreateBufferResource(dxManager_->GetDevice(), sizeof(ParticleInf) * kMaxInstanceCount_);
		assert(instancingResource_);

		HRESULT hr = instancingResource_->Map(0, nullptr, reinterpret_cast<void**>(&instancingMappedPtr_));
		assert(SUCCEEDED(hr) && instancingMappedPtr_);
		instancingDataUsed_ = 0;

		D3D12_SHADER_RESOURCE_VIEW_DESC srv{};
		srv.Format = DXGI_FORMAT_UNKNOWN;
		srv.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srv.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		srv.Buffer.FirstElement = 0;
		srv.Buffer.NumElements = kMaxInstanceCount_;
		srv.Buffer.StructureByteStride = sizeof(ParticleInf);

		instancingSrvIndex_ = dxManager_->GetDescriptorHeapManager()->AllocateSRVSlot();
		assert(instancingSrvIndex_ != UINT32_MAX);
		instancingSrvHandleCPU_ = dxManager_->GetDescriptorHeapManager()->GetCPUHandleAt(instancingSrvIndex_);
		instancingSrvHandleGPU_ = dxManager_->GetDescriptorHeapManager()->GetGPUHandleAt(instancingSrvIndex_);
		dxManager_->GetDevice()->CreateShaderResourceView(instancingResource_.Get(), &srv, instancingSrvHandleCPU_);
	}

	//kMaxInstanceCount_ = 2048;
	//instancingResource_.resize(kMaxInstanceCount_);
	//instancingData_.resize(kMaxInstanceCount_);
	//for (size_t i = 0; i < kMaxInstanceCount_; ++i)
	//{
	//	instancingResource_[i] = CreateBufferResource(dxManager->GetDevice(), sizeof(ParticleInf));
	//	instancingResource_[i]->Map(0, nullptr, reinterpret_cast<void**>(&instancingData_[i]));
	//}
	//instancingDataUsed_ = 0;

	//{
	//	// 1つの連続バッファで ParticleInf を capacity 個分確保
	//	const uint32_t capacity = kMaxInstanceCount_;
	//	Microsoft::WRL::ComPtr<ID3D12Resource> instancingBuffer =
	//		CreateBufferResource(dxManager_->GetDevice(), sizeof(ParticleInf) * capacity);
	//
	//	ParticleInf* mapped = nullptr;
	//	hr = instancingBuffer->Map(0, nullptr, reinterpret_cast<void**>(&mapped));
	//	assert(SUCCEEDED(hr));
	//	// 既存のベクタ使いではなく、単一バッファ＋先頭ポインタに置き換える設計を推奨
	//	// 例: instancingResource_.clear(); instancingResource_.shrink_to_fit();
	//	//     instancingResource_.push_back(instancingBuffer);
	//	//     instancingData_.clear(); instancingData_.push_back(mapped);
	//
	//	// SRVを作成
	//	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	//	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	//	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	//	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	//	srvDesc.Buffer.FirstElement = 0;
	//	srvDesc.Buffer.NumElements = capacity;
	//	srvDesc.Buffer.StructureByteStride = sizeof(ParticleInf);
	//
	//	instancingSrvIndex_ = dxManager_->GetDescriptorHeapManager()->AllocateSRVSlot();
	//	auto cpu = dxManager_->GetDescriptorHeapManager()->GetCPUHandleAt(instancingSrvIndex_);
	//	dxManager_->GetDevice()->CreateShaderResourceView(instancingBuffer.Get(), &srvDesc, cpu);
	//}
	//EnsureInstanceBuffer(kNumInstance_);


	// インデックスリソース
	indexResource = CreateBufferResource(dxManager->GetDevice(), sizeof(uint32_t) * 6);
	uint32_t* indexData = nullptr;
	indexResource->Map(0, nullptr, reinterpret_cast<void**>(&indexData));
	indexData[0] = 0;
	indexData[1] = 1;
	indexData[2] = 2;
	indexData[3] = 1;
	indexData[4] = 3;
	indexData[5] = 2;
	indexResource->Unmap(0, nullptr);

	// リソースの先頭のアドレスから使う
	indexBufferView.BufferLocation = indexResource->GetGPUVirtualAddress();
	// 仕様するリソースのサイズはインデックス６つ分のサイズ
	indexBufferView.SizeInBytes = sizeof(uint32_t) * 6;
	// インデックスはuint32_tとする
	indexBufferView.Format = DXGI_FORMAT_R32_UINT;
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
		instancingMappedPtr_ = nullptr;
	}

	instancingSrvHandleCPU_ = {};
	instancingSrvHandleGPU_ = {};

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

	// パーティクル用データの初期化
	instancingDataUsed_ = 0;

	// カメラマトリックスの更新
	viewProjectionMatrix_ = viewProjectionMatrix;

	// ライトの向きを正規化
	directionalLightData_->direction = (directionalLightData_->direction.Normalized());
}

void DrawSystem::EndFrame()
{
	// パーティクルの更新
	Update_ParticleInstanceData();
}

void DrawSystem::Update_ParticleInstanceData()
{
	for (uint32_t i = 0; i < activeInstanceCount_; ++i)
	{
		bool isAlive = true;

		/// 寿命管理
		if (instancingMappedPtr_[i].liveTime > 0)
		{
			instancingMappedPtr_[i].liveTime -= 1;
			if (instancingMappedPtr_[i].liveTime <= 0)isAlive = false;
		}


		/// 拡縮管理
		instancingMappedPtr_[i].scale.velocity += instancingMappedPtr_[i].scale.acceleration * dxManager_->GetDeltaTime();
		instancingMappedPtr_[i].scale.value += instancingMappedPtr_[i].scale.velocity * dxManager_->GetDeltaTime();
		if (instancingMappedPtr_[i].scale.value.x <= 0.0f ||
			instancingMappedPtr_[i].scale.value.y <= 0.0f ||
			instancingMappedPtr_[i].scale.value.z <= 0.0f)  // XYZ 全てが0以下になったら消す
		{
			isAlive = false;
		}

		/// 座標管理
		instancingMappedPtr_[i].translate.velocity += instancingMappedPtr_[i].translate.acceleration * dxManager_->GetDeltaTime();
		instancingMappedPtr_[i].translate.value += instancingMappedPtr_[i].translate.velocity * dxManager_->GetDeltaTime();

		/// 回転管理
		instancingMappedPtr_[i].rotate.velocity += instancingMappedPtr_[i].rotate.acceleration * dxManager_->GetDeltaTime();
		instancingMappedPtr_[i].rotate.value += instancingMappedPtr_[i].rotate.velocity * dxManager_->GetDeltaTime();
		if (instancingMappedPtr_[i].isBillboard) /// ビルボード
		{
			Vector3 direction = (Game::GetCamera()->transform_.translate - instancingMappedPtr_[i].translate.value).Normalized();
			float yaw = std::atan2(direction.x, direction.z); // Y軸回り
			float pitch = std::asin(-direction.y);            // X軸回り
			instancingMappedPtr_[i].rotate.value = { pitch, yaw, instancingMappedPtr_[i].rotate.value.z };
		}

		/// 行列再計算
		const Vector3& sc = instancingMappedPtr_[i].scale.value;
		const Vector3& rt = instancingMappedPtr_[i].rotate.value;
		const Vector3& tr = instancingMappedPtr_[i].translate.value;

		Matrix4x4 world = Matrix4x4::MakeAffineMatrix(sc, rt, tr);
		Matrix4x4 wvp = world * viewProjectionMatrix_;
		instancingMappedPtr_[i].World = world;
		instancingMappedPtr_[i].WVP = wvp;


		/// 死んでたら削除
		if (!isAlive)
		{
			// 末尾とスワップして削除
			const uint32_t last = activeInstanceCount_ - 1;
			if (i != last)
			{
				instancingMappedPtr_[i] = instancingMappedPtr_[last];
			}
			activeInstanceCount_--;
			// スワップで入ってきた粒子を同じ i で再評価するため i++ しない
			continue;
		}
	}
}

void DrawSystem::DrawParticle(RenderData_Particle& renderData)
{
	if (drawCallIndex_ >= kMaxDrawCallPerFrame_) return;

	 
	// モデルの検索
	Object3D* obj = dxManager_->GetResourceManager()->GetModelManager()->GetModel(renderData.model);
	if (!obj) return;
	
	// テクスチャの検索
	const TextureData* tex = dxManager_->GetResourceManager()->GetTextureManager()->GetTexture(renderData.texture);
	if (!tex) return;
	
	// RootSignatureとPSOを設定
	dxManager_->GetCommandContextManager()->GetCommandList()->SetGraphicsRootSignature(dxManager_->GetPipelineStateManager()->GetRootSignature_particle()); // 共通のルートシグネチャ
	if (wireframeMode_)
	{	// ワイヤーフレーム用PSOを設定
		dxManager_->GetCommandContextManager()->GetCommandList()->SetPipelineState(dxManager_->GetPipelineStateManager()->GetParticlePipelineState(BlendMode::kBlendModeNormal));
	}
	else
	{	// Triangle用PSOを設定
		dxManager_->GetCommandContextManager()->GetCommandList()->SetPipelineState(dxManager_->GetPipelineStateManager()->GetParticlePipelineState(BlendMode::kBlendModeAdd));
	}
	
	// 頂点数の取得
	const uint32_t kSumVertex = static_cast<uint32_t>(obj->modelData.vertices.size());
	
	// マテリアルデータ
	Vector4 color = ConvertUintToVector4(renderData.color);
	materialData_[drawCallIndex_]->color = color;
	materialData_[drawCallIndex_]->enableLighting = true;
	materialData_[drawCallIndex_]->uvTransform = Matrix4x4::MakeIdentity4x4();

	// =========================
	// エミッター毎のプール確保 or 取得
	// =========================
	//struct EmitterPool
	//{
	//	Microsoft::WRL::ComPtr<ID3D12Resource> buffer;
	//	ParticleInf* mapped = nullptr;
	//	uint32_t capacity = 0;
	//	uint32_t activeCount = 0;
	//	uint32_t srvIndex = UINT32_MAX;
	//};
	//
	//auto& pool = particlePools_[&renderData];
	//if (!pool.buffer)
	//{
	//	pool.capacity = kMaxInstanceCount_;
	//	pool.buffer = CreateBufferResource(dxManager_->GetDevice(), sizeof(ParticleInf) * pool.capacity);
	//	assert(pool.buffer);
	//
	//	HRESULT hr = pool.buffer->Map(0, nullptr, reinterpret_cast<void**>(&pool.mapped));
	//	assert(SUCCEEDED(hr) && pool.mapped);
	//
	//	// SRV作成
	//	D3D12_SHADER_RESOURCE_VIEW_DESC srv{};
	//	srv.Format = DXGI_FORMAT_UNKNOWN;
	//	srv.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	//	srv.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	//	srv.Buffer.FirstElement = 0;
	//	srv.Buffer.NumElements = pool.capacity;
	//	srv.Buffer.StructureByteStride = sizeof(ParticleInf);
	//
	//	pool.srvIndex = dxManager_->GetDescriptorHeapManager()->AllocateSRVSlot();
	//	assert(pool.srvIndex != UINT32_MAX);
	//	auto cpu = dxManager_->GetDescriptorHeapManager()->GetCPUHandleAt(pool.srvIndex);
	//	dxManager_->GetDevice()->CreateShaderResourceView(pool.buffer.Get(), &srv, cpu);
	//}


	// 発生間隔が１未満なら１に補正
	if (renderData.emissionDelay < 1)renderData.emissionDelay = 1;
	if (renderData.frame % renderData.emissionDelay == 0)
	{
		// 発生数が０未満なら０に補正
		if (renderData.particlesPerEmission < 0)renderData.particlesPerEmission = 0;
		// 描画可能数を超えないように補正
		if (activeInstanceCount_ + renderData.particlesPerEmission <= kMaxInstanceCount_)
		{
			// パーティクル生成
			for (int i = 0; i < renderData.particlesPerEmission; ++i)
			{
				// 位置
				AABB aabb = renderData.emitterAABB;
				renderData.emitterAABB.min.x = my_min(aabb.min.x, aabb.max.x);
				renderData.emitterAABB.max.x = my_max(aabb.min.x, aabb.max.x);
				renderData.emitterAABB.min.y = my_min(aabb.min.y, aabb.max.y);
				renderData.emitterAABB.max.y = my_max(aabb.min.y, aabb.max.y);
				renderData.emitterAABB.min.z = my_min(aabb.min.z, aabb.max.z);
				renderData.emitterAABB.max.z = my_max(aabb.min.z, aabb.max.z);
				particleSRT translate = renderData.translate;
				translate.value.x = RandomFloat(renderData.emitterAABB.min.x, renderData.emitterAABB.max.x);
				translate.value.y = RandomFloat(renderData.emitterAABB.min.y, renderData.emitterAABB.max.y);
				translate.value.z = RandomFloat(renderData.emitterAABB.min.z, renderData.emitterAABB.max.z);

				// 拡縮
				particleSRT scale = renderData.scale;

				// 回転
				particleSRT rotate = renderData.rotate;
				if (renderData.isBillboard)	// ビルボード
				{
					Vector3 direction = (Game::GetCamera()->transform_.translate - instancingMappedPtr_[i].translate.value).Normalized();
					float yaw = std::atan2(direction.x, direction.z); // Y軸回り
					float pitch = std::asin(-direction.y);            // X軸回り
					rotate.value = { pitch, yaw, 0.0f };
				}

				// データセット
				instancingMappedPtr_[activeInstanceCount_ + i].scale = scale;
				instancingMappedPtr_[activeInstanceCount_ + i].rotate = rotate;
				instancingMappedPtr_[activeInstanceCount_ + i].translate = translate;

				Matrix4x4 world = Matrix4x4::MakeAffineMatrix(scale.value, rotate.value, translate.value);
				Matrix4x4 wvp = world * viewProjectionMatrix_;
				instancingMappedPtr_[activeInstanceCount_ + i].World = world;
				instancingMappedPtr_[activeInstanceCount_ + i].WVP = wvp;

				instancingMappedPtr_[activeInstanceCount_ + i].liveTime = renderData.liveMax;
				instancingMappedPtr_[activeInstanceCount_ + i].color = color;
				instancingMappedPtr_[activeInstanceCount_ + i].isBillboard = renderData.isBillboard;
			}

			// アクティブ数を増やす
			activeInstanceCount_ += renderData.particlesPerEmission;
		}
	}

	// SRVをバインド（RP4: t1 VS可視）
	dxManager_->GetCommandContextManager()->GetCommandList()->SetGraphicsRootDescriptorTable(4, dxManager_->GetDescriptorHeapManager()->GetGPUHandleAt(instancingSrvIndex_));
	// テクスチャ（RP2: t0 PS）
	dxManager_->GetCommandContextManager()->GetCommandList()->SetGraphicsRootDescriptorTable(2, tex->textureSrvHandleGPU);

	// 描画
	dxManager_->GetCommandContextManager()->GetCommandList()->IASetVertexBuffers(0, 1, &obj->vertexBufferView);
	dxManager_->GetCommandContextManager()->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	dxManager_->GetCommandContextManager()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResources_[drawCallIndex_]->GetGPUVirtualAddress());
	dxManager_->GetCommandContextManager()->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource_->GetGPUVirtualAddress());
	dxManager_->GetCommandContextManager()->GetCommandList()->DrawInstanced(kSumVertex, activeInstanceCount_, 0, 0);

	// renderData.currentSum =  
	renderData.frame++;

	drawCallIndex_++;
}

void DrawSystem::DrawModel(RenderData_Model& renderData)
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
	dxManager_->GetCommandContextManager()->GetCommandList()->SetGraphicsRootSignature(dxManager_->GetPipelineStateManager()->GetRootSignature()); // 共通のルートシグネチャ
	if (renderData.options.wireframe || wireframeMode_)
	{	// ワイヤーフレーム用PSOを設定
		dxManager_->GetCommandContextManager()->GetCommandList()->SetPipelineState(dxManager_->GetPipelineStateManager()->GetPipelineState(BlendMode::Wireframe, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE));
	}
	else
	{	// Triangle用PSOを設定
		dxManager_->GetCommandContextManager()->GetCommandList()->SetPipelineState(dxManager_->GetPipelineStateManager()->GetPipelineState(renderData.options.blendMode, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE));
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
	dxManager_->GetCommandContextManager()->GetCommandList()->IASetVertexBuffers(0, 1, &obj->vertexBufferView);
	// プリミティブトポロジ（描画する形状の種類：三角形リスト）を設定
	dxManager_->GetCommandContextManager()->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// ルートパラメータ0にマテリアル用定数バッファ（色・ライティング情報など）をバインド
	dxManager_->GetCommandContextManager()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResources_[drawCallIndex_]->GetGPUVirtualAddress());
	// ルートパラメータ1にWVP（ワールド・ビュー・プロジェクション）用定数バッファをバインド
	dxManager_->GetCommandContextManager()->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResources_[drawCallIndex_]->GetGPUVirtualAddress());
	// ルートパラメータ2にテクスチャのSRV（シェーダリソースビュー）をバインド
	dxManager_->GetCommandContextManager()->GetCommandList()->SetGraphicsRootDescriptorTable(2, tex->textureSrvHandleGPU);
	// ルートパラメータ3にディレクショナルライト用定数バッファをバインド
	dxManager_->GetCommandContextManager()->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource_->GetGPUVirtualAddress());
	// 頂点数分のインスタンス描画を実行（実際に描画コマンドを発行）
	dxManager_->GetCommandContextManager()->GetCommandList()->DrawInstanced(kSumVertex, 1, 0, 0);

	drawCallIndex_++;
}

void DrawSystem::DrawTriangle(RenderData_Triangle& renderData)
{
	// 描画回数上限
	if (drawCallIndex_ >= kMaxDrawCallPerFrame_) return;

	// テクスチャの検索
	const TextureData* tex = dxManager_->GetResourceManager()->GetTextureManager()->GetTexture(renderData.texture);
	if (!tex) return;

	// RootSignatureとPSOを設定
	dxManager_->GetCommandContextManager()->GetCommandList()->SetGraphicsRootSignature(dxManager_->GetPipelineStateManager()->GetRootSignature()); // 共通のルートシグネチャ
	if (renderData.options.wireframe || wireframeMode_)
	{	// ワイヤーフレーム用PSOを設定
		dxManager_->GetCommandContextManager()->GetCommandList()->SetPipelineState(dxManager_->GetPipelineStateManager()->GetPipelineState(BlendMode::Wireframe, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE));
	}
	else
	{	// Triangle用PSOを設定
		dxManager_->GetCommandContextManager()->GetCommandList()->SetPipelineState(dxManager_->GetPipelineStateManager()->GetPipelineState(renderData.options.blendMode, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE));
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
	Matrix4x4 world = Matrix4x4::MakeAffineMatrix(renderData.transforms.scale, renderData.transforms.rotate, renderData.transforms.translate);
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
	dxManager_->GetCommandContextManager()->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);
	// 形状を設定
	dxManager_->GetCommandContextManager()->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// CBVを設定する マテリアル用のCBufferの場所を設定
	dxManager_->GetCommandContextManager()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResources_[drawCallIndex_]->GetGPUVirtualAddress());
	// CBVを設定する wvp用のCBufferの場所を設定
	dxManager_->GetCommandContextManager()->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResources_[drawCallIndex_]->GetGPUVirtualAddress());
	// SRVのDescriptorTableの先頭を設定。２はrootParameters[2]。
	dxManager_->GetCommandContextManager()->GetCommandList()->SetGraphicsRootDescriptorTable(2, tex->textureSrvHandleGPU);
	// CBVを設定する ディレクショナルライト用のCBufferの場所を設定
	dxManager_->GetCommandContextManager()->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource_->GetGPUVirtualAddress());

	// 描画
	dxManager_->GetCommandContextManager()->GetCommandList()->DrawInstanced(3, 1, 0, 0);


	drawCallIndex_++;
	vertexDataUsed_ += kSumVertex;
}

void DrawSystem::DrawSprite(RenderData_Sprite& renderData)
{
	// 描画回数上限
	if (drawCallIndex_ >= kMaxDrawCallPerFrame_) return;

	// テクスチャの検索
	const TextureData* tex = dxManager_->GetResourceManager()->GetTextureManager()->GetTexture(renderData.texture);
	if (!tex) return;

	// RootSignatureとPSOを設定 - Triangle
	dxManager_->GetCommandContextManager()->GetCommandList()->SetGraphicsRootSignature(dxManager_->GetPipelineStateManager()->GetRootSignature()); // 共通のルートシグネチャ
	dxManager_->GetCommandContextManager()->GetCommandList()->SetPipelineState(dxManager_->GetPipelineStateManager()->GetPipelineState(renderData.options.blendMode, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE)); // Triangle用PSOを設定

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
	Matrix4x4 world = Matrix4x4::MakeAffineMatrix(renderData.transforms.scale, renderData.transforms.rotate, renderData.transforms.translate);
	Matrix4x4 wvpMatrix = (world * orthoProjectionMatrix_);

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
	
	left *= float(WindowManager::winWidth_) / 1280.0f;
	right *= float(WindowManager::winWidth_) / 1280.0f;
	top *= float(WindowManager::winHeight_) / 720.0f;
	bottom *= float(WindowManager::winHeight_) / 720.0f;

	// マウス座標取得
	Vector2 mousePos = Game::GetMousePosition();
	// マウス座標は仮想座標へ変換してから衝突判定に使う。
	float windowWidth = float(WindowManager::winWidth_);
	float windowHeight = float(WindowManager::winHeight_);

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
	dxManager_->GetCommandContextManager()->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);
	dxManager_->GetCommandContextManager()->GetCommandList()->IASetIndexBuffer(&indexBufferView);
	// 形状を設定
	dxManager_->GetCommandContextManager()->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// CBVを設定する マテリアル用のCBufferの場所を設定
	dxManager_->GetCommandContextManager()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResources_[drawCallIndex_]->GetGPUVirtualAddress());
	// CBVを設定する wvp用のCBufferの場所を設定
	dxManager_->GetCommandContextManager()->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResources_[drawCallIndex_]->GetGPUVirtualAddress());
	// SRVのDescriptorTableの先頭を設定。２はrootParameters[2]。
	dxManager_->GetCommandContextManager()->GetCommandList()->SetGraphicsRootDescriptorTable(2, tex->textureSrvHandleGPU);
	// CBVを設定する ディレクショナルライト用のCBufferの場所を設定
	dxManager_->GetCommandContextManager()->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource_->GetGPUVirtualAddress());

	// 描画
	dxManager_->GetCommandContextManager()->GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);

	drawCallIndex_++;
	vertexDataUsed_ += kSumVertex;
}

void DrawSystem::DrawLine(RenderData_Line& renderData)
{
	if (drawCallIndex_ >= kMaxDrawCallPerFrame_) return;

	// 2点未満
	if (renderData.points.size() < 2) return;

	// RootSignatureとPSOを設定 - Line
	dxManager_->GetCommandContextManager()->GetCommandList()->SetGraphicsRootSignature(dxManager_->GetPipelineStateManager()->GetRootSignature()); // 共通のルートシグネチャ
	dxManager_->GetCommandContextManager()->GetCommandList()->SetPipelineState(dxManager_->GetPipelineStateManager()->GetPipelineState(BlendMode::kBlendModeNormal, D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE)); // Line用PSOを設定

	// 指定された点
	std::vector<Vector3> mainPoints;
	for (size_t i = 1; i < renderData.points.size(); ++i)
	{
		mainPoints.push_back(renderData.points[i]);
	}
	// 曲線だった場合の補完点も含めた点リスト(points同士を直線で結ぶ)
	std::vector<Vector3> points;

	// 補完の分割数
	const int subdivision = my_max(uint32_t(1), renderData.kSubdivision);

	switch (renderData.lineType)
	{
	case LineType::Line:
	{
		points = mainPoints;
		break;
	}
	case LineType::BezierCurve:
	{
		if (mainPoints.size() >= 4)
		{
			for (size_t i = 0; i + 3 < mainPoints.size(); i += 3)
			{
				const Vector3& p0 = mainPoints[i];
				const Vector3& p1 = mainPoints[i + 1];
				const Vector3& p2 = mainPoints[i + 2];
				const Vector3& p3 = mainPoints[i + 3];

				if (i == 0)
				{
					points.push_back(p0); // 最初だけ始点を追加
				}
				for (int j = 0; j <= subdivision; ++j)
				{
					float t = static_cast<float>(j) / static_cast<float>(subdivision);
					Vector3 point =
						(p0 * std::powf(1.0f - t, 3)) +
						(p1 * (3.0f * std::powf(1.0f - t, 2) * t)) +
						(p2 * (3.0f * (1.0f - t) * t * t)) +
						(p3 * (t * t * t));
					points.push_back(point);
				}
			}
		}
		else if (mainPoints.size() == 3)
		{
			const Vector3& p0 = mainPoints[0];
			const Vector3& p1 = mainPoints[1];
			const Vector3& p2 = mainPoints[2];

			for (int j = 0; j <= subdivision; ++j)
			{
				float t = static_cast<float>(j) / static_cast<float>(subdivision);
				Vector3 a01 = p0 + (p1 - p0) * t;
				Vector3 a12 = p1 + (p2 - p1) * t;
				Vector3 point = a01 + (a12 - a01) * t;

				points.push_back(point);
			}
		}
		else
		{
			points = mainPoints;
			Log("DrawLine制御点不足  ID:%d  name:%s", renderData.ID, renderData.name);
			return;
		}
		break;
	}
	case LineType::SplineCurve:
	{
		if (mainPoints.size() >= 3)
		{
			for (size_t i = 0; i < mainPoints.size() - 1; ++i)
			{
				const Vector3& p1 = mainPoints[i];
				const Vector3& p2 = mainPoints[i + 1];
				const Vector3& p0 = (i == 0) ? p1 : mainPoints[i - 1];
				const Vector3& p3 = (i + 2 < mainPoints.size()) ? mainPoints[i + 2] : p2;

				if (i == 0)
				{
					points.push_back(p1); // 最初だけ始点を追加
				}

				for (int j = 1; j <= subdivision; ++j)
				{
					float t = static_cast<float>(j) / static_cast<float>(subdivision);
					Vector3 point =
						(((p1 * 2.0f) +
							(-p0 + p2) * t +
							((p0 * 2.0f) - (p1 * 5.0f) + (p2 * 4.0f) - p3) * t * t +
							(-p0 + (p1 * 3.0f) - (p2 * 3.0f) + p3) * t * t * t)) * 0.5f;
					points.push_back(point);
				}
			}
		}
		else
		{
			points = mainPoints;
			Log("DrawLine制御点不足  ID:%d  name:%s", renderData.ID, renderData.name);
			return;
		}
		break;
	}
	default:
	{
		break;
	}
	}

	// [p0,p1,p1,p2,p2,p3,...] の形に展開
	std::vector<Vector3> out;

	out.reserve((points.size() - 1) * 2);
	for (size_t i = 0; i + 1 < points.size(); ++i)
	{
		out.push_back(points[i]);
		out.push_back(points[i + 1]);
	}


	// 頂点数の取得
	const uint32_t kSumVertex = static_cast<uint32_t>(out.size());
	// 必要な頂点数分配列を拡張
	if (vertexDataUsed_ + kSumVertex > vertexData_.size())
	{
		vertexData_.resize(vertexDataUsed_ + kSumVertex);
	}

	for (size_t i = 0; i < out.size(); i++)
	{
		vertexData_[vertexDataUsed_ + i].position = { out[i].x, out[i].y, out[i].z, 1.0f };
	}

	// WVP行列
	wvpData_[drawCallIndex_]->World = Matrix4x4::MakeIdentity4x4();
	wvpData_[drawCallIndex_]->WVP = viewProjectionMatrix_;

	// マテリアル定数バッファの更新
	Vector4 color = ConvertUintToVector4(renderData.color);
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
	dxManager_->GetCommandContextManager()->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);
	// 形状を設定
	dxManager_->GetCommandContextManager()->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
	// CBVを設定する マテリアル用のCBufferの場所を設定
	dxManager_->GetCommandContextManager()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResources_[drawCallIndex_]->GetGPUVirtualAddress());// b1にバインド
	// CBVを設定する wvp用のCBufferの場所を設定
	dxManager_->GetCommandContextManager()->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResources_[drawCallIndex_]->GetGPUVirtualAddress()); // b0にバインド
	// 描画コマンドの発行
	dxManager_->GetCommandContextManager()->GetCommandList()->DrawInstanced(kSumVertex, 1, 0, 0);


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


//bool DrawSystem::EnsureInstanceBuffer(size_t requiredInstanceCount)
//{
//	// 既存容量で足りる場合
//	if (instancingResource_ && instancingCapacity_ >= requiredInstanceCount)
//	{
//		return true;
//	}
//
//	// 新しい容量（2倍成長＋最低64）
//	uint32_t newCapacity = static_cast<uint32_t>(
//		std::max<size_t>(requiredInstanceCount, instancingCapacity_ ? instancingCapacity_ * 2ull : 64ull)
//		);
//	size_t newSizeBytes = sizeof(ParticleInf) * static_cast<size_t>(newCapacity);
//
//	// 新リソース作成（Uploadバッファ）
//	auto newResource = CreateBufferResource(dxManager_->GetDevice() , newSizeBytes);
//	if (!newResource) return false;
//
//	ParticleInf* newMapped = nullptr;
//	HRESULT hr = newResource->Map(0, nullptr, reinterpret_cast<void**>(&newMapped));
//	if (FAILED(hr) || !newMapped) return false;
//
//	// 古いリソースを解放
//	if (instancingResource_)
//	{
//		instancingResource_->Unmap(0, nullptr);
//		instancingResource_.Reset();
//		instancingData_ = nullptr;
//	}
//
//	instancingResource_ = newResource;
//	instancingData_ = newMapped;
//	instancingCapacity_ = newCapacity;
//
//	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
//	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
//	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
//	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
//	srvDesc.Buffer.FirstElement = 0;
//	srvDesc.Buffer.NumElements = newCapacity;
//	srvDesc.Buffer.StructureByteStride = sizeof(ParticleInf);
//
//	// 現在のヒープにおける該当インデックスのハンドルを取得して作り直す
//	instancingSrvIndex_ = dxManager_->GetDescriptorHeapManager()->AllocateSRVSlot();
//	instancingSrvHandleCPU_ = dxManager_->GetDescriptorHeapManager()->GetCPUHandleAt(instancingSrvIndex_);
//	instancingSrvHandleGPU_ = dxManager_->GetDescriptorHeapManager()->GetGPUHandleAt(instancingSrvIndex_);
//
//	dxManager_->GetDevice()->CreateShaderResourceView(instancingResource_.Get(), &srvDesc, instancingSrvHandleCPU_);
//
//
//	return true;
//}
////
//// パーティクル作成時の初期化
//bool DrawSystem::CreateNewParticle(uint32_t sum, ParticleInf inf)
//{
//	//assert(instanceDataUsed_ + sum <= instancingCapacity_);
//
//	//// 初期化
//	//for (uint32_t i = instanceDataUsed_; i < instanceDataUsed_ + sum; ++i)
//	//{
//	//	instancingData_[i].transform.scale = inf.transform.scale;
//	//	instancingData_[i].transform.rotate = inf.transform.rotate;
//	//	instancingData_[i].transform.translate = inf.transform.translate;
//	//	instancingData_[i].velocity = inf.velocity;
//	//	instancingData_[i].liveTime = inf.liveTime;
//	//}
//
//	//return true;
//}
