#include "DrawSystem/DrawSystem.h"
#include "DirectX/DirectXManager.h"
#include "Window/WindowManager.h"
#include "Utilities/functions.h"
#include "Block/Block.h"

DrawSystem::DrawSystem(DirectXManager* dxManager)
	:dxManager_(dxManager)
{
	// 正射影行列
	this->orthoProjectionMatrix_ = Matrix4x4::MakeOrthographicMatrix(
		0.0f, 0.0f,
		static_cast<float>(WindowManager::winWidth_),
		static_cast<float>(WindowManager::winHeight_),
		0.0f, 100.0f);

	// カメラマトリックス
	this->viewProjectionMatrix_ = Matrix4x4::MakeIdentity4x4();

	// ライト
	this->directionalLightResource_ = CreateConstantBufferResource(dxManager->GetDevice(), sizeof(DirectionalLight));
	this->directionalLightData_ = nullptr;
	this->directionalLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData_));
	this->directionalLightData_->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	this->directionalLightData_->direction = { 0.0f, -1.0f, 0.0f };
	this->directionalLightData_->intensity = 1.0f;
	this->directionalLightData_->mode = LightMode::HalfLambert;
	this->directionalLightData_->phong = false;

	cameraResource_ = CreateConstantBufferResource(dxManager->GetDevice(), sizeof(CameraForGPU));
	cameraData_ = nullptr;
	cameraResource_->Map(0, nullptr, reinterpret_cast<void**>(&cameraData_));

	// 描画コールカウント初期化
	drawCallIndex_ = 0;

	// 頂点リソース
	vertexResourceSize_ = static_cast<UINT>(sizeof(VertexData) * 4096); // 三角形
	vertexResource_ = CreateBufferResource(dxManager->GetDevice(), vertexResourceSize_);
	HRESULT hr = vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexMappedPtr_));
	assert(SUCCEEDED(hr));

	kMaxDrawCallPerFrame_ = 4096;
	materialResources_.resize(kMaxDrawCallPerFrame_);
	materialData_.resize(kMaxDrawCallPerFrame_);
	wvpResources_.resize(kMaxDrawCallPerFrame_);
	wvpData_.resize(kMaxDrawCallPerFrame_);
	lightResources_.resize(kMaxDrawCallPerFrame_);
	lightData_.resize(kMaxDrawCallPerFrame_);
	for (size_t i = 0; i < kMaxDrawCallPerFrame_; ++i)
	{
		materialResources_[i] = CreateConstantBufferResource(dxManager->GetDevice(), sizeof(Material));
		materialResources_[i]->Map(0, nullptr, reinterpret_cast<void**>(&materialData_[i]));
		wvpResources_[i] = CreateConstantBufferResource(dxManager->GetDevice(), sizeof(TransformationMatrix));
		wvpResources_[i]->Map(0, nullptr, reinterpret_cast<void**>(&wvpData_[i]));
		lightResources_[i] = CreateConstantBufferResource(dxManager->GetDevice(), sizeof(DirectionalLight));
		lightResources_[i]->Map(0, nullptr, reinterpret_cast<void**>(&lightData_[i]));
		lightData_[i]->color = { 1.0f, 1.0f, 1.0f, 1.0f };
		lightData_[i]->direction = { 0.0f, -1.0f, 0.0f };
		lightData_[i]->intensity = 1.0f;
		lightData_[i]->mode = LightMode::HalfLambert;
		lightData_[i]->phong = true;
	}
	vertexDataUsed_ = 0;

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
		if (lightResources_[i])
		{
			lightResources_[i]->Unmap(0, nullptr);
			lightResources_[i].Reset();
			lightData_[i] = nullptr;
		}
	}

	for (auto& it : s_particlePools)
	{
		if (it.second.buffer)
		{
			it.second.buffer->Unmap(0, nullptr);
			it.second.buffer.Reset();
			it.second.mapped = nullptr;
		}
	}
	s_particlePools.clear();

	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>>().swap(materialResources_);
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>>().swap(wvpResources_);
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>>().swap(lightResources_);

	vertexResource_.Reset();
	directionalLightResource_.Reset();
}

void DrawSystem::Update()
{
	// 前フレームの不要VBを解放（EndFrame→Present→WaitForGPU 後）
	vbHoldUntilSubmit_.clear();

	// 描画コールの初期化
	drawCallIndex_ = 0;

	// 三角形用頂点データの初期化
	vertexDataUsed_ = 0;

	// ライトの向きを正規化
	directionalLightData_->direction.Normalize();

	// カメラデータの更新
	cameraData_->worldPosition = Game::Camera::Getter::GetCurrentTranslate();

	// ドローリストの初期化
	modelDrawList_.clear();
	triangleDrawList_.clear();
	rectDrawList_.clear();
	spriteDrawList_.clear();
	lineDrawList_.clear();

	// パーティクルの更新
	Update_ParticleInstanceData();
}

void DrawSystem::Update_ParticleInstanceData()
{
	//for (auto& it : s_particlePools)
	//{
	//	EmitterPool& pool = it.second;
	//	if (!pool.mapped || pool.activeCount == 0) continue;
	//
	//	for (uint32_t i = 0; i < pool.activeCount; /* 手動で増減 */)
	//	{
	//		bool isAlive = true;
	//
	//		// 寿命
	//		if (pool.mapped[i].liveTime > 0)
	//		{
	//			pool.mapped[i].liveTime -= 1;
	//			if (pool.mapped[i].liveTime <= 0) isAlive = false;
	//		}
	//
	//		// 拡縮
	//		pool.mapped[i].scale.velocity += pool.mapped[i].scale.acceleration * dxManager_->GetDeltaTime();
	//		pool.mapped[i].scale.value += pool.mapped[i].scale.velocity * dxManager_->GetDeltaTime();
	//		if (pool.mapped[i].scale.value.x <= 0.0f ||
	//			pool.mapped[i].scale.value.y <= 0.0f ||
	//			pool.mapped[i].scale.value.z <= 0.0f)
	//		{
	//			isAlive = false;
	//		}
	//
	//		// 平行移動
	//		pool.mapped[i].translate.velocity += pool.mapped[i].translate.acceleration * dxManager_->GetDeltaTime();
	//		pool.mapped[i].translate.value += pool.mapped[i].translate.velocity * dxManager_->GetDeltaTime();
	//
	//		// 回転
	//		pool.mapped[i].rotate.velocity += pool.mapped[i].rotate.acceleration * dxManager_->GetDeltaTime();
	//		pool.mapped[i].rotate.value += pool.mapped[i].rotate.velocity * dxManager_->GetDeltaTime();
	//
	//		// ビルボード
	//		if (pool.mapped[i].isBillboard)
	//		{
	//			Vector3 direction = (Game::Camera::Getter::GetTranslate("ReleaseCamera") - pool.mapped[i].translate.value).Normalized();
	//			float yaw = std::atan2(direction.x, direction.z); // Y軸
	//			float pitch = std::asin(-direction.y);              // X軸
	//			pool.mapped[i].rotate.value = { pitch, yaw, pool.mapped[i].rotate.value.z };
	//		}
	//
	//		// 行列
	//		const Vector3& sc = pool.mapped[i].scale.value;
	//		const Vector3& rt = pool.mapped[i].rotate.value;
	//		const Vector3& tr = pool.mapped[i].translate.value;
	//
	//		Matrix4x4 world = Matrix4x4::MakeAffineMatrix(sc, rt, tr);
	//		Matrix4x4 wvp = world * viewProjectionMatrix_;
	//		pool.mapped[i].World = world;
	//		pool.mapped[i].WVP = wvp;
	//
	//		// 死亡なら末尾とスワップして詰める
	//		if (!isAlive)
	//		{
	//			const uint32_t last = pool.activeCount - 1;
	//			if (i != last)
	//			{
	//				pool.mapped[i] = pool.mapped[last];
	//			}
	//			pool.activeCount--;
	//			continue; // iは増やさない（入れ替わった要素を再評価）
	//		}
	//		++i;
	//	}
	//}

}

void DrawSystem::Draw()
{
	// モデル描画
	DrawModel();

	// 三角形描画
	DrawTriangle();

	// 矩形描画
	DrawRect();

	// スプライト描画
	DrawSprite();

	// 線描画
	DrawLine();
}


void DrawSystem::AddTriangleDrawList(RenderData_Triangle& renderData)
{
	triangleDrawList_.emplace_back(&renderData);
}
void DrawSystem::AddModelDrawList(RenderData_Model& renderData)
{
	modelDrawList_.emplace_back(&renderData);
}
void DrawSystem::AddRectDrawList(RenderData_Rect& renderData)
{
	rectDrawList_.emplace_back(&renderData);
}
void DrawSystem::AddSpriteDrawList(RenderData_Sprite & renderData)
{
	spriteDrawList_.emplace_back(&renderData);
}
void DrawSystem::AddLineDrawList(RenderData_Line & renderData)
{
	lineDrawList_.emplace_back(&renderData);
}

void DrawSystem::DrawModel()
{
	for (auto& renderData : modelDrawList_)
	{
		// 画面内か判定
		if (!renderData->inPicture)continue;

		// 描画回数上限
		if (drawCallIndex_ >= kMaxDrawCallPerFrame_)continue;

		// モデルの検索
		const Object3D* obj = dxManager_->GetResourceManager()->GetModelManager()->GetModelData(renderData->model);
		if (!obj)continue;

		// テクスチャの検索
		const TextureData* tex = dxManager_->GetResourceManager()->GetTextureManager()->GetTextureData(renderData->texture);
		if (!tex)continue;
		const TextureData* tex2 = dxManager_->GetResourceManager()->GetTextureManager()->GetTextureData(renderData->additionalTexture);
		if (!tex2)continue;

		// ルートシグネチャを設定
		dxManager_->GetCommandContextManager()->GetCommandList()->SetGraphicsRootSignature(dxManager_->GetPipelineStateManager()->GetRootSignature());
		if (renderData->options.wireframe || wireframeMode_)
			// ワイヤーフレーム用PSOを設定
			dxManager_->GetCommandContextManager()->GetCommandList()->SetPipelineState(dxManager_->GetPipelineStateManager()->GetPipelineState(BlendMode::Wireframe, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE));
		else
			// Triangle用PSOを設定
			dxManager_->GetCommandContextManager()->GetCommandList()->SetPipelineState(dxManager_->GetPipelineStateManager()->GetPipelineState(renderData->options.blendMode, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE));

		// 頂点数の取得
		const uint32_t kSumVertex = static_cast<uint32_t>(obj->modelData.vertices.size());

		// WVP行列
		wvpData_[drawCallIndex_]->World = renderData->GetWorldMatrix();
		wvpData_[drawCallIndex_]->WVP = renderData->GetWorldMatrix() * viewProjectionMatrix_;

		// マテリアル
		Vector4 color = { renderData->color.x / 255.0f, renderData->color.y / 255.0f, renderData->color.z / 255.0f, renderData->color.w / 255.0f };
		materialData_[drawCallIndex_]->color = color;
		materialData_[drawCallIndex_]->shininess = 1.0f;
		Matrix4x4 uvTransformMatrix = Matrix4x4::MakeIdentity4x4();
		uvTransformMatrix = (uvTransformMatrix * Matrix4x4::MakeScaleMatrix(renderData->uvTransform.scale));
		uvTransformMatrix = (uvTransformMatrix * Matrix4x4::MakeRotateZMatrix(renderData->uvTransform.rotate.z));
		uvTransformMatrix = (uvTransformMatrix * Matrix4x4::MakeTranslateMatrix(renderData->uvTransform.translate));
		materialData_[drawCallIndex_]->uvTransform = uvTransformMatrix;

		// ライト
		if (!renderData->options.useOwnLight)
		{
			*lightData_[drawCallIndex_] = *directionalLightData_;
		}
		else
		{
			lightData_[drawCallIndex_]->color = renderData->options.dirLight.color;
			lightData_[drawCallIndex_]->direction = renderData->options.dirLight.direction.Normalized();
			lightData_[drawCallIndex_]->intensity = renderData->options.dirLight.intensity;
			lightData_[drawCallIndex_]->mode = renderData->options.dirLight.mode;
			lightData_[drawCallIndex_]->phong = renderData->options.dirLight.phong;
		}

		// カメラデータ
		cameraData_->worldPosition = Game::Camera::Getter::GetCurrentTranslate();

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
		// ルートパラメータ2にテクスチャのSRV（シェーダリソースビュー）をバインド
		//dxManager_->GetCommandContextManager()->GetCommandList()->SetGraphicsRootDescriptorTable(3, tex2->textureSrvHandleGPU);
		// ルートパラメータ3にライト用定数バッファをバインド
		dxManager_->GetCommandContextManager()->GetCommandList()->SetGraphicsRootConstantBufferView(3, lightResources_[drawCallIndex_]->GetGPUVirtualAddress());
		// ルートパラメータ4にスペキュラライト用定数バッファをバインド
		dxManager_->GetCommandContextManager()->GetCommandList()->SetGraphicsRootConstantBufferView(4, cameraResource_->GetGPUVirtualAddress());
		// 頂点数分のインスタンス描画を実行（実際に描画コマンドを発行）
		dxManager_->GetCommandContextManager()->GetCommandList()->DrawInstanced(kSumVertex, 1, 0, 0);

		drawCallIndex_++;
	}
}
void DrawSystem::DrawTriangle()
{
	for (auto& renderData : triangleDrawList_)
	{
		// 描画回数上限
		if (drawCallIndex_ >= kMaxDrawCallPerFrame_)continue;

		// テクスチャの検索
		const TextureData* tex = dxManager_->GetResourceManager()->GetTextureManager()->GetTextureData(renderData->texture);
		if (!tex) continue;

		// RootSignatureとPSOを設定
		dxManager_->GetCommandContextManager()->GetCommandList()->SetGraphicsRootSignature(dxManager_->GetPipelineStateManager()->GetRootSignature()); // 共通のルートシグネチャ
		if (renderData->options.wireframe || wireframeMode_)
		{	// ワイヤーフレーム用PSOを設定
			dxManager_->GetCommandContextManager()->GetCommandList()->SetPipelineState(dxManager_->GetPipelineStateManager()->GetPipelineState(BlendMode::Wireframe, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE));
		}
		else
		{	// Triangle用PSOを設定
			dxManager_->GetCommandContextManager()->GetCommandList()->SetPipelineState(dxManager_->GetPipelineStateManager()->GetPipelineState(renderData->options.blendMode, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE));
		}

		// 頂点数の取得
		const uint32_t kSumVertex = 3;
		// 必要な頂点数分配列を拡張
		if (vertexDataUsed_ + kSumVertex > vertexData_.size())
		{
			vertexData_.resize(vertexDataUsed_ + kSumVertex);
		}

		// 上
		vertexData_[vertexDataUsed_ + 0].position = { renderData->pos1.x, renderData->pos1.y, renderData->pos1.z, 1.0f };
		vertexData_[vertexDataUsed_ + 0].texcoord = { 0.5f, 0.0f };
		vertexData_[vertexDataUsed_ + 0].normal = { 0.0f, 0.0f, -1.0f };

		// 右下
		vertexData_[vertexDataUsed_ + 1].position = { renderData->pos2.x, renderData->pos2.y, renderData->pos2.z, 1.0f };
		vertexData_[vertexDataUsed_ + 1].texcoord = { 1.0f, 1.0f };
		vertexData_[vertexDataUsed_ + 1].normal = { 0.0f, 0.0f, -1.0f };

		// 左下
		vertexData_[vertexDataUsed_ + 2].position = { renderData->pos3.x,renderData->pos3.y, renderData->pos3.z, 1.0f };
		vertexData_[vertexDataUsed_ + 2].texcoord = { 0.0f, 1.0f };
		vertexData_[vertexDataUsed_ + 2].normal = { 0.0f, 0.0f, -1.0f };

		// WVP行列
		Matrix4x4 world = Matrix4x4::MakeAffineMatrix(renderData->transforms.scale, renderData->transforms.rotate, renderData->transforms.translate);
		Matrix4x4 wvpMatrix = world * viewProjectionMatrix_;

		wvpData_[drawCallIndex_]->World = world;
		wvpData_[drawCallIndex_]->WVP = wvpMatrix;

		// ライトの設定
		*lightData_[drawCallIndex_] = *directionalLightData_;

		// マテリアル
		Vector4 color = ConvertUintToVector4(renderData->color);
		materialData_[drawCallIndex_]->color = color;
		materialData_[drawCallIndex_]->shininess = 1.0f;
		Matrix4x4 uvTransformMatrix = Matrix4x4::MakeIdentity4x4();
		uvTransformMatrix = (uvTransformMatrix * Matrix4x4::MakeScaleMatrix(renderData->uvTransform.scale));
		uvTransformMatrix = (uvTransformMatrix * Matrix4x4::MakeRotateZMatrix(renderData->uvTransform.rotate.z));
		uvTransformMatrix = (uvTransformMatrix * Matrix4x4::MakeTranslateMatrix(renderData->uvTransform.translate));
		materialData_[drawCallIndex_]->uvTransform = uvTransformMatrix;

		// 頂点リソース
		VertexData* vData = nullptr;
		HRESULT hr = vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vData));
		if (FAILED(hr) || vData == nullptr) continue;
		std::memcpy(vData + vertexDataUsed_, &vertexData_[vertexDataUsed_], sizeof(VertexData) * kSumVertex);
		vertexResource_->Unmap(0, nullptr);

		// 動的頂点バッファを確保
		if (!EnsureDynamicVB(vertexDataUsed_ + kSumVertex)) continue;
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
		dxManager_->GetCommandContextManager()->GetCommandList()->SetGraphicsRootConstantBufferView(3, lightResources_[drawCallIndex_]->GetGPUVirtualAddress());

		// 描画
		dxManager_->GetCommandContextManager()->GetCommandList()->DrawInstanced(3, 1, 0, 0);


		drawCallIndex_++;
		vertexDataUsed_ += kSumVertex;
	}
}
void DrawSystem::DrawRect()
{
	for (auto& renderData : rectDrawList_)
	{
		// 描画回数上限
		if (drawCallIndex_ >= kMaxDrawCallPerFrame_) continue;

		// テクスチャの検索
		const TextureData* tex = dxManager_->GetResourceManager()->GetTextureManager()->GetTextureData(renderData->texture);
		if (!tex) continue;

		// RootSignatureとPSOを設定
		dxManager_->GetCommandContextManager()->GetCommandList()->SetGraphicsRootSignature(dxManager_->GetPipelineStateManager()->GetRootSignature()); // 共通のルートシグネチャ
		if (renderData->options.wireframe || wireframeMode_)
		{	// ワイヤーフレーム用PSOを設定
			dxManager_->GetCommandContextManager()->GetCommandList()->SetPipelineState(dxManager_->GetPipelineStateManager()->GetPipelineState(BlendMode::Wireframe, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE));
		}
		else
		{	// Triangle用PSOを設定
			dxManager_->GetCommandContextManager()->GetCommandList()->SetPipelineState(dxManager_->GetPipelineStateManager()->GetPipelineState(renderData->options.blendMode, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE));
		}

		// 頂点数の取得
		const uint32_t kSumVertex = 6;
		// 必要な頂点数分配列を拡張
		if (vertexDataUsed_ + kSumVertex > vertexData_.size())
		{
			vertexData_.resize(vertexDataUsed_ + kSumVertex);
		}


		/// 三角形１つめ
		// 左上
		vertexData_[vertexDataUsed_ + 0].position = { renderData->pos3.x, renderData->pos3.y, renderData->pos3.z, 1.0f };
		vertexData_[vertexDataUsed_ + 0].texcoord = { 0.0f, 0.0f };
		vertexData_[vertexDataUsed_ + 0].normal = { 0.0f, 0.0f, -1.0f };

		// 右上
		vertexData_[vertexDataUsed_ + 1].position = { renderData->pos1.x, renderData->pos1.y, renderData->pos1.z, 1.0f };
		vertexData_[vertexDataUsed_ + 1].texcoord = { 1.0f, 0.0f };
		vertexData_[vertexDataUsed_ + 1].normal = { 0.0f, 0.0f, -1.0f };

		// 右下
		vertexData_[vertexDataUsed_ + 2].position = { renderData->pos2.x, renderData->pos2.y, renderData->pos2.z, 1.0f };
		vertexData_[vertexDataUsed_ + 2].texcoord = { 1.0f, 1.0f };
		vertexData_[vertexDataUsed_ + 2].normal = { 0.0f, 0.0f, -1.0f };

		/// 三角形２つめ
		// 左上
		vertexData_[vertexDataUsed_ + 3].position = { renderData->pos3.x, renderData->pos3.y, renderData->pos3.z, 1.0f };
		vertexData_[vertexDataUsed_ + 3].texcoord = { 0.0f, 0.0f };
		vertexData_[vertexDataUsed_ + 3].normal = { 0.0f, 0.0f, -1.0f };

		// 右下
		vertexData_[vertexDataUsed_ + 4].position = { renderData->pos2.x, renderData->pos2.y, renderData->pos2.z, 1.0f };
		vertexData_[vertexDataUsed_ + 4].texcoord = { 1.0f, 1.0f };
		vertexData_[vertexDataUsed_ + 4].normal = { 0.0f, 0.0f, -1.0f };

		// 左下
		vertexData_[vertexDataUsed_ + 5].position = { renderData->pos4.x, renderData->pos4.y, renderData->pos4.z, 1.0f };
		vertexData_[vertexDataUsed_ + 5].texcoord = { 0.0f, 1.0f };
		vertexData_[vertexDataUsed_ + 5].normal = { 0.0f, 0.0f, -1.0f };


		// WVP行列
		Matrix4x4 world = Matrix4x4::MakeAffineMatrix(renderData->transforms.scale, renderData->transforms.rotate, renderData->transforms.translate);
		Matrix4x4 wvpMatrix = world * viewProjectionMatrix_;

		wvpData_[drawCallIndex_]->World = world;
		wvpData_[drawCallIndex_]->WVP = wvpMatrix;

		// ライトの設定
		*lightData_[drawCallIndex_] = *directionalLightData_;

		// マテリアル
		Vector4 color = ConvertUintToVector4(renderData->color);
		materialData_[drawCallIndex_]->color = color;
		materialData_[drawCallIndex_]->shininess = 1.0f;
		Matrix4x4 uvTransformMatrix = Matrix4x4::MakeIdentity4x4();
		uvTransformMatrix = (uvTransformMatrix * Matrix4x4::MakeScaleMatrix(renderData->uvTransform.scale));
		uvTransformMatrix = (uvTransformMatrix * Matrix4x4::MakeRotateZMatrix(renderData->uvTransform.rotate.z));
		uvTransformMatrix = (uvTransformMatrix * Matrix4x4::MakeTranslateMatrix(renderData->uvTransform.translate));
		materialData_[drawCallIndex_]->uvTransform = uvTransformMatrix;

		// 頂点リソース
		VertexData* vData = nullptr;
		HRESULT hr = vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vData));
		if (FAILED(hr) || vData == nullptr) continue;
		std::memcpy(vData + vertexDataUsed_, &vertexData_[vertexDataUsed_], sizeof(VertexData) * kSumVertex);
		vertexResource_->Unmap(0, nullptr);

		// 動的頂点バッファを確保
		if (!EnsureDynamicVB(vertexDataUsed_ + kSumVertex)) continue;
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
		dxManager_->GetCommandContextManager()->GetCommandList()->SetGraphicsRootConstantBufferView(3, lightResources_[drawCallIndex_]->GetGPUVirtualAddress());

		// 描画
		dxManager_->GetCommandContextManager()->GetCommandList()->DrawInstanced(6, 1, 0, 0);


		drawCallIndex_++;
		vertexDataUsed_ += kSumVertex;
	}
}
void DrawSystem::DrawSprite()
{
	for (auto& renderData : spriteDrawList_)
	{
		// 描画回数上限
		if (drawCallIndex_ >= kMaxDrawCallPerFrame_) continue;

		// テクスチャの検索
		const TextureData* tex = dxManager_->GetResourceManager()->GetTextureManager()->GetTextureData(renderData->texture);
		if (!tex) continue;

		// RootSignatureとPSOを設定 - Triangle
		dxManager_->GetCommandContextManager()->GetCommandList()->SetGraphicsRootSignature(dxManager_->GetPipelineStateManager()->GetRootSignature()); // 共通のルートシグネチャ
		dxManager_->GetCommandContextManager()->GetCommandList()->SetPipelineState(dxManager_->GetPipelineStateManager()->GetPipelineState(renderData->options.blendMode, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE)); // Triangle用PSOを設定

		// 必要な頂点数
		const uint32_t kSumVertex = 4;
		// 必要な頂点数分配列を拡張
		if (vertexDataUsed_ + kSumVertex > vertexData_.size())
		{
			vertexData_.resize(vertexDataUsed_ + kSumVertex);
		}

		// 座標頂点
		float halfWidth = static_cast<float>(tex->metadata.width) * 0.5f;
		float halfHeight = static_cast<float>(tex->metadata.height) * 0.5f;

		// UV座標頂点
		if (renderData->cutImageSize.x == 0 && renderData->cutImageSize.y == 0)
		{
			renderData->cutImageSize.x = static_cast<int>(tex->metadata.width);
			renderData->cutImageSize.y = static_cast<int>(tex->metadata.height);
		}
		if (renderData->cutImageLeftTop.x < 0) renderData->cutImageLeftTop.x = 0;
		if (renderData->cutImageLeftTop.y < 0) renderData->cutImageLeftTop.y = 0;

		Vector2 point = { float(renderData->cutImageLeftTop.x), float(renderData->cutImageLeftTop.y) };
		Vector2 size = { float(renderData->cutImageSize.x), float(renderData->cutImageSize.y) };

		float L = point.x / tex->metadata.width;
		float R = (point.x + size.x) / tex->metadata.width;
		float T = point.y / tex->metadata.height;
		float B = (point.y + size.y) / tex->metadata.height;

		// 左下 (index 0)
		vertexData_[vertexDataUsed_ + 0].position = { -halfWidth, -halfHeight, 0.0f, 1.0f };
		vertexData_[vertexDataUsed_ + 0].texcoord = { L, T };
		vertexData_[vertexDataUsed_ + 0].normal = { 0.0f, 0.0f, -1.0f };

		// 左上 (index 1)
		vertexData_[vertexDataUsed_ + 1].position = { halfWidth, -halfHeight, 0.0f, 1.0f };
		vertexData_[vertexDataUsed_ + 1].texcoord = { R, T };
		vertexData_[vertexDataUsed_ + 1].normal = { 0.0f, 0.0f, -1.0f };

		// 右下 (index 2)
		vertexData_[vertexDataUsed_ + 2].position = { -halfWidth, halfHeight, 0.0f, 1.0f };
		vertexData_[vertexDataUsed_ + 2].texcoord = { L, B };
		vertexData_[vertexDataUsed_ + 2].normal = { 0.0f, 0.0f, -1.0f };

		// 右上 (index 3)
		vertexData_[vertexDataUsed_ + 3].position = { halfWidth, halfHeight, 0.0f, 1.0f };
		vertexData_[vertexDataUsed_ + 3].texcoord = { R, B };
		vertexData_[vertexDataUsed_ + 3].normal = { 0.0f, 0.0f, -1.0f };

		// アンカーによる位置調整
		switch (renderData->anchor)
		{
		case Anchor::Center:
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
		case Anchor::CenterLeft:
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
		case Anchor::CenterRight:
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
		case Anchor::CenterTop:
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
		case Anchor::CenterDown:
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
		case Anchor::LeftTop:
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
		case Anchor::RightTop:
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
		case Anchor::LeftDown:
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
		case Anchor::RightDown:
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
		Matrix4x4 world = Matrix4x4::MakeAffineMatrix(renderData->transforms.scale, renderData->transforms.rotate, renderData->transforms.translate);
		Matrix4x4 wvpMatrix = (world * orthoProjectionMatrix_);

		wvpData_[drawCallIndex_]->World = world;
		wvpData_[drawCallIndex_]->WVP = wvpMatrix;

		// ライトの設定
		lightData_[drawCallIndex_]->color = { 1.0f, 1.0f, 1.0f, 1.0f };
		lightData_[drawCallIndex_]->intensity = 1.0f;
		lightData_[drawCallIndex_]->mode = LightMode::None;
		lightData_[drawCallIndex_]->phong = false;

		// マテリアル
		float uvCenterX = (renderData->pivot.x) - (halfWidth);
		float uvCenterY = (renderData->pivot.y) - (halfHeight);
		uvCenterX = uvCenterX / halfWidth / 2;
		uvCenterY = uvCenterY / halfHeight / 2;

		Matrix4x4 toCenter = Matrix4x4::MakeTranslateMatrix({ -uvCenterX, -uvCenterY, 0.0f });
		Matrix4x4 fromCenter = Matrix4x4::MakeTranslateMatrix({ uvCenterX, uvCenterY, 0.0f });
		Matrix4x4 uvTransformMatrix = Matrix4x4::MakeIdentity4x4();
		uvTransformMatrix = (uvTransformMatrix * Matrix4x4::MakeScaleMatrix(renderData->uvTransform.scale));
		uvTransformMatrix = (uvTransformMatrix * Matrix4x4::MakeRotateZMatrix(renderData->uvTransform.rotate.z));

		uvTransformMatrix = (fromCenter * (uvTransformMatrix * toCenter));

		uvTransformMatrix = (uvTransformMatrix * Matrix4x4::MakeTranslateMatrix(renderData->uvTransform.translate));

		// ===== UV マテリアルへ反映 =====
		Vector4 color = ConvertUintToVector4(renderData->color);
		materialData_[drawCallIndex_]->color = color;
		materialData_[drawCallIndex_]->shininess = 1.0f;
		materialData_[drawCallIndex_]->uvTransform = uvTransformMatrix;

		// スプライトの中心座標
		Vector2 center = { renderData->transforms.translate.x, renderData->transforms.translate.y };


		halfWidth *= renderData->transforms.scale.x;
		halfHeight *= renderData->transforms.scale.y;

		// アンカーに応じて中心座標を補正
		switch (renderData->anchor)
		{
		case Anchor::CenterLeft:
			center.x += halfWidth;
			break;
		case Anchor::CenterRight:
			center.x -= halfWidth;
			break;
		case Anchor::CenterTop:
			center.y += halfHeight;
			break;
		case Anchor::CenterDown:
			center.y -= halfHeight;
			break;
		case Anchor::LeftTop:
			center.x += halfWidth;
			center.y += halfHeight;
			break;
		case Anchor::RightTop:
			center.x -= halfWidth;
			center.y += halfHeight;
			break;
		case Anchor::LeftDown:
			center.x += halfWidth;
			center.y -= halfHeight;
			break;
		case Anchor::RightDown:
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
		Vector2 mousePos = Game::Input::Mouse::GetMousePosition();

		// マウス座標は仮想座標へ変換してから衝突判定に使う。
		float windowWidth = float(WindowManager::winWidth_);
		float windowHeight = float(WindowManager::winHeight_);

		// 当たり判定
		renderData->isCollisionMouseRay = (mousePos.x >= left && mousePos.x <= right && mousePos.y >= top && mousePos.y <= bottom);



		// 動的頂点バッファを確保
		if (!EnsureDynamicVB(vertexDataUsed_ + kSumVertex)) continue;
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
		dxManager_->GetCommandContextManager()->GetCommandList()->SetGraphicsRootConstantBufferView(3, lightResources_[drawCallIndex_]->GetGPUVirtualAddress());

		// 描画
		dxManager_->GetCommandContextManager()->GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);

		drawCallIndex_++;
		vertexDataUsed_ += kSumVertex;
	}
}
void DrawSystem::DrawLine()
{
	for (auto& renderData : lineDrawList_)
	{
		// 描画回数上限
		if (drawCallIndex_ >= kMaxDrawCallPerFrame_) continue;

		// 2点未満
		if (renderData->points.size() < 2) continue;

		// RootSignatureとPSOを設定 - Line
		dxManager_->GetCommandContextManager()->GetCommandList()->SetGraphicsRootSignature(dxManager_->GetPipelineStateManager()->GetRootSignature()); // 共通のルートシグネチャ
		dxManager_->GetCommandContextManager()->GetCommandList()->SetPipelineState(dxManager_->GetPipelineStateManager()->GetPipelineState(BlendMode::kBlendModeNormal, D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE)); // Line用PSOを設定

		// 指定された点
		std::vector<Vector3> mainPoints;
		for (size_t i = 1; i < renderData->points.size(); ++i)
		{
			mainPoints.push_back(renderData->points[i]);
		}
		// 曲線だった場合の補完点も含めた点リスト(points同士を直線で結ぶ)
		std::vector<Vector3> points;

		// 補完の分割数
		const int subdivision = my_max(uint32_t(1), renderData->kSubdivision);

		switch (renderData->lineType)
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
				Log("DrawLine制御点不足  ID:%d  name:%s", renderData->ID, renderData->name);
				continue;
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
				Log("DrawLine制御点不足  ID:%d  name:%s", renderData->ID, renderData->name);
				continue;
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
		Vector4 color = ConvertUintToVector4(renderData->color);
		materialData_[drawCallIndex_]->color = color;
		materialData_[drawCallIndex_]->shininess = 1.0f;
		materialData_[drawCallIndex_]->uvTransform = Matrix4x4::MakeIdentity4x4(); // 線にUV変換いらない

		// 頂点リソース
		VertexData* vData = nullptr;
		HRESULT hr = vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vData));
		if (FAILED(hr) || vData == nullptr) continue;
		std::memcpy(vData + vertexDataUsed_, &vertexData_[vertexDataUsed_], sizeof(VertexData) * kSumVertex);
		vertexResource_->Unmap(0, nullptr);

		// 動的頂点バッファを確保
		if (!EnsureDynamicVB(vertexDataUsed_ + kSumVertex)) continue;
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
}
void DrawSystem::DrawParticle(ParticleGroup& renderData)
{
	if (drawCallIndex_ >= kMaxDrawCallPerFrame_) return;


	// モデルの検索
	Object3D* obj = dxManager_->GetResourceManager()->GetModelManager()->GetModelData(renderData.model);
	if (!obj) return;

	// テクスチャの検索
	const TextureData* tex = dxManager_->GetResourceManager()->GetTextureManager()->GetTextureData(renderData.texture);
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

	// ライトの設定
	lightData_[drawCallIndex_]->color = { 0xFF, 0xFF, 0xFF, 0xFF };
	lightData_[drawCallIndex_]->mode = LightMode::None;
	lightData_[drawCallIndex_]->phong = false;

	// 頂点数の取得
	const uint32_t kSumVertex = static_cast<uint32_t>(obj->modelData.vertices.size());

	// マテリアルデータ
	materialData_[drawCallIndex_]->color = renderData.material.color;
	materialData_[drawCallIndex_]->shininess = 1.0f;
	materialData_[drawCallIndex_]->uvTransform = Matrix4x4::MakeIdentity4x4();

	// SRV（t1 VS）をこのプールのものに
	dxManager_->GetCommandContextManager()->GetCommandList()->SetGraphicsRootDescriptorTable(4, renderData.srvAllocation.gpu);
	// テクスチャ（t0 PS）
	dxManager_->GetCommandContextManager()->GetCommandList()->SetGraphicsRootDescriptorTable(2, tex->textureSrvHandleGPU);

	// VertexBufferViewを設定
	dxManager_->GetCommandContextManager()->GetCommandList()->IASetVertexBuffers(0, 1, &obj->vertexBufferView);
	// 描画形状を設定
	dxManager_->GetCommandContextManager()->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// 
	dxManager_->GetCommandContextManager()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResources_[drawCallIndex_]->GetGPUVirtualAddress());
	//dxManager_->GetCommandContextManager()->GetCommandList()->SetGraphicsRootConstantBufferView(3, lightResources_[drawCallIndex_]->GetGPUVirtualAddress());{
	dxManager_->GetCommandContextManager()->GetCommandList()->DrawInstanced(kSumVertex, UINT(renderData.particles.size()), 0, 0);
	drawCallIndex_++;
}

void DrawSystem::AddSphere(Vector3 pos, Vector3 radius, uint32_t color)
{
	RenderData_Line Lon[10];
	RenderData_Line Lat[10];

	for (int i = 0; i < 10; ++i)
	{
		Lon[i].color = color;
		Lon[i].kSubdivision = 10;
		Lon[i].lineType = LineType::Line;

		Lat[i].color = color;
		Lat[i].kSubdivision = 10;
		Lat[i].lineType = LineType::Line;
	}

	// 経度/緯度の分割数
	const uint32_t kSubdivision = 10;
	// 経度分割１つ分の角度
	const float kLonEvery = float((2 * std::numbers::pi_v<float>) / kSubdivision);
	// 緯度分割１つ分の角度
	const float kLatEvery = float(std::numbers::pi_v<float> / kSubdivision);

	float lat = 0.0f;
	float nextLat = 0.0f;
	float lon = 0.0f;
	float nextLon = 0.0f;

	// 緯度の方向に分割 -π/2 ～ π/2
	for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex)
	{
		// 現在の緯度と次の緯度
		lat = float(-std::numbers::pi_v<float> / 2.0f + latIndex * kLatEvery);
		nextLat = float(-std::numbers::pi_v<float> / 2.0f + (latIndex + 1) * kLatEvery);

		// 経度方向に分割 0 ～ 2π
		for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex)
		{
			// 現在の経度と次の経度
			lon = lonIndex * kLonEvery;
			nextLon = (lonIndex + 1) * kLonEvery;

			Lon[latIndex].points.push_back({
				pos.x + radius.x * std::cosf(lat) * std::cosf(lon),
				pos.y + radius.y * std::sinf(lat),
				pos.z + radius.z * std::cosf(lat) * std::sinf(lon),
				});

			Lon[latIndex].points.push_back({
				pos.x + radius.x * std::cosf(lat) * std::cosf(nextLon),
				pos.y + radius.y * std::sinf(lat),
				pos.z + radius.z * std::cosf(lat) * std::sinf(nextLon),
				});

			Lat[lonIndex].points.push_back({
				pos.x + radius.x * std::cosf(lat) * std::cosf(lon),
				pos.y + radius.y * std::sinf(lat),
				pos.z + radius.z * std::cosf(lat) * std::sinf(lon),
				});

			Lat[lonIndex].points.push_back({
				pos.x + radius.x * std::cosf(nextLat) * std::cosf(lon),
				pos.y + radius.y * std::sinf(nextLat),
				pos.z + radius.z * std::cosf(nextLat) * std::sinf(lon),
				});
		}
	}

	// 線を描画
	for (uint32_t i = 0; i < kSubdivision; ++i)
	{
		AddLineDrawList(Lon[i]);
		AddLineDrawList(Lat[i]);
	}
}
void DrawSystem::AddAABB(AABB aabb, uint32_t color)
{
	// 描画回数上限
	if (drawCallIndex_ >= kMaxDrawCallPerFrame_) return;

	// RootSignatureとPSOを設定（Line用）
	dxManager_->GetCommandContextManager()->GetCommandList()->SetGraphicsRootSignature(
		dxManager_->GetPipelineStateManager()->GetRootSignature());
	dxManager_->GetCommandContextManager()->GetCommandList()->SetPipelineState(
		dxManager_->GetPipelineStateManager()->GetPipelineState(BlendMode::kBlendModeNormal, D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE));

	// 頂点は8個
	Vector3 p[8] = {
		{ aabb.min.x, aabb.min.y, aabb.min.z }, // 0
		{ aabb.max.x, aabb.min.y, aabb.min.z }, // 1
		{ aabb.max.x, aabb.max.y, aabb.min.z }, // 2
		{ aabb.min.x, aabb.max.y, aabb.min.z }, // 3
		{ aabb.min.x, aabb.min.y, aabb.max.z }, // 4
		{ aabb.max.x, aabb.min.y, aabb.max.z }, // 5
		{ aabb.max.x, aabb.max.y, aabb.max.z }, // 6
		{ aabb.min.x, aabb.max.y, aabb.max.z }, // 7
	};

	// ラインの始点終点に分けると3 * 8個
	const uint32_t kSumVertex = 24;

	// 必要頂点数分確保
	if (vertexDataUsed_ + kSumVertex > vertexData_.size())
	{
		vertexData_.resize(vertexDataUsed_ + kSumVertex);
	}

	for (size_t i = 0; i < kSumVertex; ++i)
	{
		vertexData_[vertexDataUsed_ + i].texcoord = { 0.0f, 0.0f };
		vertexData_[vertexDataUsed_ + i].normal = { 0.0f, 0.0f, -1.0f };
	}

	// 底面（z=min）
	vertexData_[vertexDataUsed_ + 0].position = { p[0].x, p[0].y, p[0].z, 1.0f };
	vertexData_[vertexDataUsed_ + 1].position = { p[1].x, p[1].y, p[1].z, 1.0f };
	vertexData_[vertexDataUsed_ + 2].position = { p[1].x, p[1].y, p[1].z, 1.0f };
	vertexData_[vertexDataUsed_ + 3].position = { p[2].x, p[2].y, p[2].z, 1.0f };
	vertexData_[vertexDataUsed_ + 4].position = { p[2].x, p[2].y, p[2].z, 1.0f };
	vertexData_[vertexDataUsed_ + 5].position = { p[3].x, p[3].y, p[3].z, 1.0f };
	vertexData_[vertexDataUsed_ + 6].position = { p[3].x, p[3].y, p[3].z, 1.0f };
	vertexData_[vertexDataUsed_ + 7].position = { p[0].x, p[0].y, p[0].z, 1.0f };

	// 上面（z=max）
	vertexData_[vertexDataUsed_ + 8].position = { p[4].x, p[4].y, p[4].z, 1.0f };
	vertexData_[vertexDataUsed_ + 9].position = { p[5].x, p[5].y, p[5].z, 1.0f };
	vertexData_[vertexDataUsed_ + 10].position = { p[5].x, p[5].y, p[5].z, 1.0f };
	vertexData_[vertexDataUsed_ + 11].position = { p[6].x, p[6].y, p[6].z, 1.0f };
	vertexData_[vertexDataUsed_ + 12].position = { p[6].x, p[6].y, p[6].z, 1.0f };
	vertexData_[vertexDataUsed_ + 13].position = { p[7].x, p[7].y, p[7].z, 1.0f };
	vertexData_[vertexDataUsed_ + 14].position = { p[7].x, p[7].y, p[7].z, 1.0f };
	vertexData_[vertexDataUsed_ + 15].position = { p[4].x, p[4].y, p[4].z, 1.0f };

	// 側面（縦）
	vertexData_[vertexDataUsed_ + 16].position = { p[0].x, p[0].y, p[0].z, 1.0f };
	vertexData_[vertexDataUsed_ + 17].position = { p[4].x, p[4].y, p[4].z, 1.0f };
	vertexData_[vertexDataUsed_ + 18].position = { p[1].x, p[1].y, p[1].z, 1.0f };
	vertexData_[vertexDataUsed_ + 19].position = { p[5].x, p[5].y, p[5].z, 1.0f };
	vertexData_[vertexDataUsed_ + 20].position = { p[2].x, p[2].y, p[2].z, 1.0f };
	vertexData_[vertexDataUsed_ + 21].position = { p[6].x, p[6].y, p[6].z, 1.0f };
	vertexData_[vertexDataUsed_ + 22].position = { p[3].x, p[3].y, p[3].z, 1.0f };
	vertexData_[vertexDataUsed_ + 23].position = { p[7].x, p[7].y, p[7].z, 1.0f };

	// WVP
	wvpData_[drawCallIndex_]->World = Matrix4x4::MakeIdentity4x4();
	wvpData_[drawCallIndex_]->WVP = viewProjectionMatrix_;

	// ライトの設定
	*lightData_[drawCallIndex_] = *directionalLightData_;

	// マテリアル（ラインはテクスチャ不要）
	materialData_[drawCallIndex_]->color = ConvertUintToVector4(color);
	materialData_[drawCallIndex_]->shininess = 1.0f;
	materialData_[drawCallIndex_]->uvTransform = Matrix4x4::MakeIdentity4x4();

	// Upload（動的VB）
	if (!EnsureDynamicVB(vertexDataUsed_ + kSumVertex)) return;
	std::memcpy(vertexMappedPtr_ + vertexDataUsed_, &vertexData_[vertexDataUsed_], sizeof(VertexData) * kSumVertex);

	// VBVを作成
	D3D12_VERTEX_BUFFER_VIEW vbv{};
	vbv.BufferLocation = vertexResource_->GetGPUVirtualAddress() + sizeof(VertexData) * vertexDataUsed_;
	vbv.SizeInBytes = sizeof(VertexData) * kSumVertex;
	vbv.StrideInBytes = sizeof(VertexData);

	// バインドと描画
	auto* cmd = dxManager_->GetCommandContextManager()->GetCommandList();
	cmd->IASetVertexBuffers(0, 1, &vbv);
	cmd->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
	cmd->SetGraphicsRootConstantBufferView(0, materialResources_[drawCallIndex_]->GetGPUVirtualAddress());
	cmd->SetGraphicsRootConstantBufferView(1, wvpResources_[drawCallIndex_]->GetGPUVirtualAddress());
	cmd->SetGraphicsRootConstantBufferView(3, lightResources_[drawCallIndex_]->GetGPUVirtualAddress());
	cmd->DrawInstanced(kSumVertex, 1, 0, 0);

	// カウンタ更新
	drawCallIndex_++;
	vertexDataUsed_ += kSumVertex;
}
void DrawSystem::AddLine(Vector3 start, Vector3 end, uint32_t color)
{
	RenderData_Line lineData;
	lineData.color = color;
	lineData.kSubdivision = 1;
	lineData.lineType = LineType::Line;
	lineData.points.push_back(start);
	lineData.points.push_back(end);
	AddLineDrawList(lineData);
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
	Microsoft::WRL::ComPtr<ID3D12Resource> oldResource = vertexResource_; // 保持用に退避
	if (vertexMappedPtr_ && vertexDataUsed_ > 0)
	{
		memcpy(newMapped, vertexMappedPtr_, vertexDataUsed_ * sizeof(VertexData));
		if (oldResource) { oldResource->Unmap(0, nullptr); }
	}

	// 今フレーム中は古いVBを破棄しない（参照中のDrawがあるため）
	if (oldResource) { vbHoldUntilSubmit_.push_back(oldResource); }

	vertexResource_ = newResource;
	vertexMappedPtr_ = newMapped;
	vertexResourceSize_ = newSizeBytes;
	return true;
}