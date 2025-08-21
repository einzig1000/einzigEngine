#include "Engine/Engine.h"
#include "externals/DirectXTex/d3dx12.h"
#include "externals/DirectXTex/DirectXTex.h"
#include "Utilities/Easings.h"
#include "Utilities/functions.h"
#include <cstdint>

#include "input/MouseController.h"
#include "Camera/CameraController.h"
#include "Window/WindowManager.h"
#include "DirectX/DirectXManager.h"

// 初期化用
void Engine::Initialize(int width, int height, const std::wstring& title)
{
	// COM の初期化
	HRESULT hr = CoInitializeEx(0, COINIT_MULTITHREADED);
	assert(SUCCEEDED(hr));
	// 例外ハンドラの設定
	SetUnhandledExceptionFilter(ExportDump);

	if (!windowManager)
	{
		windowManager = new WindowManager(width, height, title);
	}
	if (!dxManager)
	{
		dxManager = new DirectXManager(windowManager->GetHwnd(), width, height);
	}

	/// imguiの初期化
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(windowManager->GetHwnd());
	ImGui_ImplDX12_Init(
		dxManager->GetDevice(),
		dxManager->GetSwapChainDesc().BufferCount,
		dxManager->GetRtvDesc().Format,
		dxManager->GetsrvDescriptorHeap(),
		dxManager->GetsrvDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(),
		dxManager->GetsrvDescriptorHeap()->GetGPUDescriptorHandleForHeapStart()
	);

	// カメラ
	cameraController = new CameraController;

	// マウス
	mouseController = new MouseController;

	// 頂点リソース
	vertexResourceSizeSprite = static_cast<UINT>(sizeof(VertexData) * 1536); // スプライト 
	vertexResourceSprite = CreateBufferResource(dxManager->GetDevice(), vertexResourceSizeSprite);

	vertexResourceSizeObj = static_cast<UINT>(sizeof(VertexData) * 4096); // オブジェクト
	//vertexResourceSizeObj = static_cast<UINT>(sizeof(VertexData) * 8192); // オブジェクト
	vertexResourceObj = CreateBufferResource(dxManager->GetDevice(), vertexResourceSizeObj);

	vertexResourceSizeTriangle = static_cast<UINT>(sizeof(VertexData) * 1024); // 三角形
	vertexResourceTriangle = CreateBufferResource(dxManager->GetDevice(), vertexResourceSizeTriangle);

	vertexResourceSizeSphere = static_cast<UINT>(sizeof(VertexData) * 4096); // 球
	vertexResourceSphere = CreateBufferResource(dxManager->GetDevice(), vertexResourceSizeSphere);

	vertexResourceSizeLine = static_cast<UINT>(sizeof(VertexData) * 2048); // 1024本の線
	vertexResourceLine = CreateBufferResource(dxManager->GetDevice(), vertexResourceSizeLine);

	materialResources.resize(kMaxDrawCallPerFrame);
	materialData.resize(kMaxDrawCallPerFrame);
	wvpResources.resize(kMaxDrawCallPerFrame);
	wvpData.resize(kMaxDrawCallPerFrame);
	for (size_t i = 0; i < kMaxDrawCallPerFrame; ++i)
	{
		materialResources[i] = CreateBufferResource(dxManager->GetDevice(), sizeof(Material));
		materialResources[i]->Map(0, nullptr, reinterpret_cast<void**>(&materialData[i]));
		wvpResources[i] = CreateBufferResource(dxManager->GetDevice(), sizeof(TransformationMatrix));
		wvpResources[i]->Map(0, nullptr, reinterpret_cast<void**>(&wvpData[i]));
	}


	materialResourceLine.resize(kMaxDrawLineCallPerFrame);
	materialDataLine.resize(kMaxDrawLineCallPerFrame);
	wvpResourceLine.resize(kMaxDrawLineCallPerFrame);
	wvpDataLine.resize(kMaxDrawLineCallPerFrame);
	InitializeLineResources(dxManager->GetDevice());



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


	// 光源の設定
	directionalLightResource = CreateBufferResource(dxManager->GetDevice(), sizeof(DirectionalLight));
	directionalLightData = nullptr;
	directionalLightResource->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData));
	directionalLightData->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	directionalLightData->direction = { 0.0f, -1.0f, 0.0f };
	directionalLightData->intensity = 1.0f;

	// プリミティブモードの設定
	WireframeMode = false;
}

// メインループ用
bool Engine::ProcessMessage()
{
	MSG msg = {};
	if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
		{
			return false;
		}
		if (msg.message == WM_MOUSEWHEEL)
		{
			// ホイールの回転量を加算　クリックはboolで回転量はintだからmessageを使う。らしい。なんで？
			wheelDelta += GET_WHEEL_DELTA_WPARAM(msg.wParam);
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return true;
}
void Engine::BeginFrame()
{
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	sphereVertexDataUsed = 0;
	spriteVertexDataUsed = 0;
	ImGui::NewFrame();


	UpdateLight();
	UpdateCamera();
	UpdateTransforms();
	dxManager->BeginFrame();
}
void Engine::UpdateLight()
{
	// ライトの向きを正規化
	directionalLightData->direction = (directionalLightData->direction.Normalized());
}
void Engine::UpdateCamera()
{
	/*if (GetHitKey::keys[DIK_SPACE] && !GetHitKey::preKeys[DIK_SPACE])
	{
		if (cameraController->cameraMode_ == 1)cameraController->cameraMode_ = 0;
		else cameraController->cameraMode_ = 1;
	}*/
	// カメラの更新
	cameraController->Updata();
}
void Engine::EndFrame()
{
	wheelDelta = 0;

	cameraController->Draw();
	ImGui::Render();

	dxManager->EndFrame();

	drawCallIndex = 0;
	drawLineCallIndex = 0;
}

void Engine::UpdateTransforms()
{
	//for (auto& obj : objects)
	//{
	//	obj.transform.World = Matrix4x4::MakeAffineMatrix(obj.transform.scale, obj.transform.rotate, obj.transform.translate);
	//	if (obj.transform.parentWorld)
	//	{
	//		obj.transform.World = obj.transform.World * (*obj.transform.parentWorld);
	//	}
	//}
}

// 終了処理
void Engine::Finalize()
{
	// 解放処理
	for (size_t i = 0; i < kMaxDrawCallPerFrame; ++i)
	{
		if (materialResources[i])
		{
			materialResources[i]->Unmap(0, nullptr);
			materialResources[i].Reset();
			materialData[i] = nullptr;
		}
		if (wvpResources[i])
		{
			wvpResources[i]->Unmap(0, nullptr);
			wvpResources[i].Reset();
			wvpData[i] = nullptr;
		}
	}
	for (size_t i = 0; i < kMaxDrawLineCallPerFrame; ++i)
	{
		if (materialResourceLine[i])
		{
			materialResourceLine[i]->Unmap(0, nullptr);
			materialResourceLine[i].Reset();
			materialDataLine[i] = nullptr;
		}
		if (wvpResourceLine[i])
		{
			wvpResourceLine[i]->Unmap(0, nullptr);
			wvpResourceLine[i].Reset();
			wvpDataLine[i] = nullptr;
		}
	}

	// ImGuiの終了処理
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();


	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>>().swap(materialResources);
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>>().swap(wvpResources);
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>>().swap(materialResourceLine);
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>>().swap(wvpResourceLine);

	vertexResourceSprite.Reset();
	vertexResourceObj.Reset();
	vertexResourceTriangle.Reset();
	vertexResourceSphere.Reset();
	vertexResourceLine.Reset();
	indexResource.Reset();
	directionalLightResource.Reset();

	objects.clear();

	delete dxManager;
	dxManager = nullptr;
	delete windowManager;
	windowManager = nullptr;
	delete cameraController;
	cameraController = nullptr;
	delete mouseController;
	mouseController = nullptr;

	// COMの終了処理
	CoUninitialize();
}

// Draw用データ作成するやつ
DrawData Engine::SetupDrawData(size_t dstBufferSize, const VertexData* srcVertexData, size_t vertexCount, Microsoft::WRL::ComPtr<ID3D12Resource>& vertexResource, UINT& vertexResourceSize, Material* material, const uint32_t& materialColor, bool enableLighting, const Matrix4x4& uvTransform, TransformationMatrix* wvp, const Matrix4x4& world, const Matrix4x4& wvpMatrix, uint32_t textureNumber)
{
	// 描画回数上限
	if (drawCallIndex >= kMaxDrawCallPerFrame) return{};

	// 頂点数
	if (vertexCount == 0) return{};
	if (vertexCount * sizeof(VertexData) > dstBufferSize) return {};

	// 頂点リソース
	VertexData* vData = nullptr;
	HRESULT hr = vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vData));
	if (FAILED(hr) || vData == nullptr) return {};
	std::memcpy(vData, srcVertexData, sizeof(VertexData) * vertexCount);
	vertexResource->Unmap(0, nullptr);

	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	vertexBufferView.SizeInBytes = sizeof(VertexData) * static_cast<UINT>(vertexCount);
	vertexBufferView.StrideInBytes = sizeof(VertexData);

	// マテリアル
	Vector4 color = ConvertUintToVector4(materialColor);
	material->color = color;
	material->enableLighting = enableLighting;
	material->uvTransform = uvTransform;

	// WVP
	wvp->World = world;
	wvp->WVP = wvpMatrix;

	// テクスチャ
	const TextureData* tex = dxManager->GetTextureManager()->GetTexture(textureNumber);

	return { vertexBufferView, tex };
}

// リソース読み込み
uint32_t Engine::LoadTexture(const std::string& filePath)
{
	return dxManager->GetTextureManager()->LoadTexture(filePath, dxManager->GetCommandList());
}

uint32_t Engine::LoadOBJ(const std::string& directoryPath, const std::string& filename)
{
	// ボックスを作成
	Object3D obj;
	// モデルデータ
	obj.modelData = LoadOBJFile(directoryPath, filename);
	// 変換行列
	obj.transform = { {1.0f,1.0f,1.0f}, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f} };
	// AABB
	obj.aabb = CreateLocalAABB(obj.modelData);
	// 識別ナンバー
	obj.number = static_cast<uint32_t>(objects.size());

	// まず空のObject3Dをvectorに追加し、参照を取得
	objects.push_back(obj);
	Object3D& ref = objects.back();

	// 頂点バッファ作成
	ref.vertexBufferSize = sizeof(VertexData) * UINT(ref.modelData.vertices.size());
	ref.vertexBuffer = CreateBufferResource(dxManager->GetDevice(), ref.vertexBufferSize);
	VertexData* vData = nullptr;
	ref.vertexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&vData));
	std::memcpy(vData, ref.modelData.vertices.data(), ref.vertexBufferSize);
	ref.vertexBuffer->Unmap(0, nullptr);

	ref.vertexBufferView.BufferLocation = ref.vertexBuffer->GetGPUVirtualAddress();
	ref.vertexBufferView.SizeInBytes = static_cast<UINT>(ref.vertexBufferSize);
	ref.vertexBufferView.StrideInBytes = sizeof(VertexData);

	return ref.number;
}

uint32_t Engine::LoadAudio(const std::string& filePath)
{
	return dxManager->GetAudioManager()->LoadAudio(filePath);
}

TextureData* Engine::GetTexture(uint32_t textureNumber)
{
	TextureData* tex = dxManager->GetTextureManager()->GetTexture(textureNumber);
	return tex;
}


// 描画
void Engine::Drawobj(const Transforms& transform, const Vector3& center, uint32_t objectNumber, uint32_t textureNumber, const uint32_t& materialColor, const DrawOptions drawOptions)
{
	// 描画回数上限
	if (drawCallIndex >= kMaxDrawCallPerFrame) return;

	if (objectNumber >= objects.size()) return;

	// RootSignatureとPSOを設定
	if (drawOptions.enableWireframeMode && WireframeMode)
	{	// Wireframe
		dxManager->GetCommandList()->SetPipelineState(dxManager->GetPipelineStateManager()->GetWireframePipelineState()); // ワイヤーフレーム用PSOを設定
	}
	else
	{	// Triangle
		dxManager->GetCommandList()->SetPipelineState(dxManager->GetPipelineStateManager()->GetPipelineState()); // Triangle用PSOを設定
	}
	dxManager->GetCommandList()->SetGraphicsRootSignature(dxManager->GetPipelineStateManager()->GetRootSignature()); // 共通のルートシグネチャ

	// 描画するモデルの検索
	Object3D& obj = objects[objectNumber];
	// 頂点数の取得
	const uint32_t kSumVertex = static_cast<uint32_t>(obj.modelData.vertices.size());

	// WVP行列
	wvpData[drawCallIndex]->World = transform.World;
	wvpData[drawCallIndex]->WVP = transform.World * cameraController->viewProjectionMatrix;

	const TextureData* tex = dxManager->GetTextureManager()->GetTexture(textureNumber);
	if (!tex) return;

	Vector4 color = ConvertUintToVector4(materialColor);
	materialData[drawCallIndex]->color = color;
	materialData[drawCallIndex]->enableLighting = drawOptions.enableLighting;
	Matrix4x4 uvTransformMatrix = Matrix4x4::MakeIdentity4x4();
	uvTransformMatrix = (uvTransformMatrix * Matrix4x4::MakeScaleMatrix(drawOptions.uvTransform.scale));
	uvTransformMatrix = (uvTransformMatrix * Matrix4x4::MakeRotateZMatrix(drawOptions.uvTransform.rotate.z));
	uvTransformMatrix = (uvTransformMatrix * Matrix4x4::MakeTranslateMatrix(drawOptions.uvTransform.translate));
	materialData[drawCallIndex]->uvTransform = uvTransformMatrix;


	// 頂点バッファをバインド（描画に使う頂点データを指定）
	dxManager->GetCommandList()->IASetVertexBuffers(0, 1, &obj.vertexBufferView);
	// プリミティブトポロジ（描画する形状の種類：三角形リスト）を設定
	dxManager->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// ルートパラメータ0にマテリアル用定数バッファ（色・ライティング情報など）をバインド
	dxManager->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResources[drawCallIndex]->GetGPUVirtualAddress());
	// ルートパラメータ1にWVP（ワールド・ビュー・プロジェクション）用定数バッファをバインド
	dxManager->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResources[drawCallIndex]->GetGPUVirtualAddress());
	// ルートパラメータ2にテクスチャのSRV（シェーダリソースビュー）をバインド
	dxManager->GetCommandList()->SetGraphicsRootDescriptorTable(2, tex->textureSrvHandleGPU);
	// ルートパラメータ3にディレクショナルライト用定数バッファをバインド
	dxManager->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource->GetGPUVirtualAddress());
	// 頂点数分のインスタンス描画を実行（実際に描画コマンドを発行）
	dxManager->GetCommandList()->DrawInstanced(kSumVertex, 1, 0, 0);

	drawCallIndex++;
}

void Engine::DrawSphere(const Transforms& transform, const Vector3& center, uint32_t kSubdivision, uint32_t textureNumber, const uint32_t& materialColor, const DrawOptions drawOptions)
{
	// 描画回数上限
	if (drawCallIndex >= kMaxDrawCallPerFrame) return;

	//// RootSignatureとPSOを設定 - Triangle
	//dxManager->GetCommandList()->SetPipelineState(dxManager->GetPipelineStateManager()->GetPipelineState()); // Triangle用PSOを設定
	//dxManager->GetCommandList()->SetGraphicsRootSignature(dxManager->GetPipelineStateManager()->GetRootSignature()); // 共通のルートシグネチャ

	// RootSignatureとPSOを設定
	if (drawOptions.enableWireframeMode && WireframeMode)
	{	// Wireframe
		dxManager->GetCommandList()->SetPipelineState(dxManager->GetPipelineStateManager()->GetWireframePipelineState()); // ワイヤーフレーム用PSOを設定
	}
	else
	{	// Triangle
		dxManager->GetCommandList()->SetPipelineState(dxManager->GetPipelineStateManager()->GetPipelineState()); // Triangle用PSOを設定
	}
	dxManager->GetCommandList()->SetGraphicsRootSignature(dxManager->GetPipelineStateManager()->GetRootSignature()); // 共通のルートシグネチャ

	// 必要な頂点数
	const uint32_t kSumVertex = kSubdivision * kSubdivision * 6;
	// 必要な頂点数分配列を拡張
	if (sphereVertexDataUsed + kSumVertex > sphereVertexData.size())
	{
		sphereVertexData.resize(sphereVertexDataUsed + kSumVertex);
	}

	// 頂点
	CreateSphere(&sphereVertexData[sphereVertexDataUsed], kSubdivision);


	// 1. オブジェクトのスケール行列
	Matrix4x4 scaleMatrix = Matrix4x4::MakeScaleMatrix(transform.scale);

	// 2. ワールド空間での最終的な位置への移動行列
	Matrix4x4 translateMatrix = Matrix4x4::MakeTranslateMatrix(transform.translate);

	// 3. 回転の中心への移動 (centerを原点に移動)
	Matrix4x4 toRotationCenter = Matrix4x4::MakeTranslateMatrix({ -center.x, -center.y, -center.z });

	// 4. 回転行列 (transform.rotate を center を中心とする回転として使う)
	Matrix4x4 rotateXMatrix = Matrix4x4::MakeRotateXMatrix(transform.rotate.x);
	Matrix4x4 rotateYMatrix = Matrix4x4::MakeRotateYMatrix(transform.rotate.y);
	Matrix4x4 rotateZMatrix = Matrix4x4::MakeRotateZMatrix(transform.rotate.z);
	Matrix4x4 rotationMatrix = rotateZMatrix * rotateXMatrix * rotateYMatrix;

	// 5. 回転後、元の回転中心の位置に戻す
	Matrix4x4 fromRotationCenter = Matrix4x4::MakeTranslateMatrix(center);

	// 最終的なワールド行列の構築
	Matrix4x4 worldMatrix =
		scaleMatrix *		 // 1. 拡縮はどうでもいい
		toRotationCenter *	 // 2. 回転中心を原点に移動
		rotationMatrix *	 // 3. 原点で回転 (centerを中心とした回転)
		fromRotationCenter * // 4. 回転したものを元の回転中心に戻す
		translateMatrix;	 // 5. 最終的なワールド位置へ移動

	// WVP行列
	Matrix4x4 wvpMatrix = (worldMatrix * cameraController->viewProjectionMatrix);

	wvpData[drawCallIndex]->World = worldMatrix;
	wvpData[drawCallIndex]->WVP = wvpMatrix;

	// テクスチャ
	const TextureData* tex = dxManager->GetTextureManager()->GetTexture(textureNumber);
	if (!tex)return;

	// マテリアル
	Vector4 color = ConvertUintToVector4(materialColor);
	materialData[drawCallIndex]->color = color;
	materialData[drawCallIndex]->enableLighting = drawOptions.enableLighting;
	Matrix4x4 uvTransformMatrix = Matrix4x4::MakeIdentity4x4();
	uvTransformMatrix = (uvTransformMatrix * Matrix4x4::MakeScaleMatrix(drawOptions.uvTransform.scale));
	uvTransformMatrix = (uvTransformMatrix * Matrix4x4::MakeRotateZMatrix(drawOptions.uvTransform.rotate.z));
	uvTransformMatrix = (uvTransformMatrix * Matrix4x4::MakeTranslateMatrix(drawOptions.uvTransform.translate));
	materialData[drawCallIndex]->uvTransform = uvTransformMatrix;


	// 頂点リソース
	VertexData* vData = nullptr;
	HRESULT hr = vertexResourceSphere->Map(0, nullptr, reinterpret_cast<void**>(&vData));
	if (FAILED(hr) || vData == nullptr) return;
	std::memcpy(vData + sphereVertexDataUsed, &sphereVertexData[sphereVertexDataUsed], sizeof(VertexData) * kSumVertex);
	vertexResourceSphere->Unmap(0, nullptr);

	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	vertexBufferView.BufferLocation = vertexResourceSphere->GetGPUVirtualAddress() + sizeof(VertexData) * sphereVertexDataUsed;
	vertexBufferView.SizeInBytes = sizeof(VertexData) * static_cast<UINT>(kSumVertex);
	vertexBufferView.StrideInBytes = sizeof(VertexData);

	// 描画処理
	dxManager->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);
	dxManager->GetCommandList()->IASetIndexBuffer(&indexBufferView);
	// 形状を設定
	dxManager->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// CBVを設定する マテリアル用のCBufferの場所を設定
	dxManager->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResources[drawCallIndex]->GetGPUVirtualAddress());
	// CBVを設定する wvp用のCBufferの場所を設定
	dxManager->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResources[drawCallIndex]->GetGPUVirtualAddress());
	// SRVのDescriptorTableの先頭を設定。２はrootParameters[2]。
	dxManager->GetCommandList()->SetGraphicsRootDescriptorTable(2, tex->textureSrvHandleGPU);
	// CBVを設定する ディレクショナルライト用のCBufferの場所を設定
	dxManager->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource->GetGPUVirtualAddress());

	dxManager->GetCommandList()->DrawInstanced(kSumVertex, 1, 0, 0);

	drawCallIndex++;
	sphereVertexDataUsed += kSumVertex;
}

void Engine::DrawTriangle(const Transforms& transform, const Vector3& pos1, const Vector3& pos2, const Vector3& pos3, uint32_t textureNumber, const uint32_t& materialColor, const DrawOptions drawOptions)
{
	// 描画回数上限
	if (drawCallIndex >= kMaxDrawCallPerFrame) return;

	// RootSignatureとPSOを設定
	if (drawOptions.enableWireframeMode && WireframeMode)
	{	// Wireframe
		dxManager->GetCommandList()->SetPipelineState(dxManager->GetPipelineStateManager()->GetWireframePipelineState()); // ワイヤーフレーム用PSOを設定
	}
	else
	{	// Triangle
		dxManager->GetCommandList()->SetPipelineState(dxManager->GetPipelineStateManager()->GetPipelineState()); // Triangle用PSOを設定
	}
	dxManager->GetCommandList()->SetGraphicsRootSignature(dxManager->GetPipelineStateManager()->GetRootSignature()); // 共通のルートシグネチャ

	// 必要な頂点数
	const uint32_t kSumVertex = 4;
	// 必要な頂点数分配列を拡張
	if (spriteVertexDataUsed + kSumVertex > spriteVertexData.size())
	{
		spriteVertexData.resize(spriteVertexDataUsed + kSumVertex);
	}

	// 上
	spriteVertexData[spriteVertexDataUsed + 0].position = { pos1.x,pos1.y,pos1.z,0 };
	spriteVertexData[spriteVertexDataUsed + 0].texcoord = { 0.5f, 0.0f };
	spriteVertexData[spriteVertexDataUsed + 0].normal = { 0.0f, 0.0f, -1.0f };

	// 右下
	spriteVertexData[spriteVertexDataUsed + 1].position = { pos2.x,pos2.y,pos2.z,0 };
	spriteVertexData[spriteVertexDataUsed + 1].texcoord = { 1.0f, 1.0f };
	spriteVertexData[spriteVertexDataUsed + 1].normal = { 0.0f, 0.0f, -1.0f };

	// 左下
	spriteVertexData[spriteVertexDataUsed + 2].position = { pos3.x,pos3.y,pos3.z,0 };
	spriteVertexData[spriteVertexDataUsed + 2].texcoord = { 0.0f, 1.0f };
	spriteVertexData[spriteVertexDataUsed + 2].normal = { 0.0f, 0.0f, -1.0f };

	// WVP行列
	Matrix4x4 orthoProjectionMatrix = Matrix4x4::MakeOrthographicMatrix(
		0.0f, 0.0f,
		static_cast<float>(windowManager->Getwidth()),
		static_cast<float>(windowManager->Getheight()),
		0.0f, 100.0f);
	Matrix4x4 world = Matrix4x4::MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
	Matrix4x4 wvpMatrix = (world * orthoProjectionMatrix);

	wvpData[drawCallIndex]->World = world;
	wvpData[drawCallIndex]->WVP = wvpMatrix;

	// テクスチャ
	const TextureData* tex = dxManager->GetTextureManager()->GetTexture(textureNumber);
	if (!tex)return;

	// マテリアル
	Vector4 color = ConvertUintToVector4(materialColor);
	materialData[drawCallIndex]->color = color;
	materialData[drawCallIndex]->enableLighting = drawOptions.enableLighting;
	Matrix4x4 uvTransformMatrix = Matrix4x4::MakeIdentity4x4();
	uvTransformMatrix = (uvTransformMatrix * Matrix4x4::MakeScaleMatrix(drawOptions.uvTransform.scale));
	uvTransformMatrix = (uvTransformMatrix * Matrix4x4::MakeRotateZMatrix(drawOptions.uvTransform.rotate.z));
	uvTransformMatrix = (uvTransformMatrix * Matrix4x4::MakeTranslateMatrix(drawOptions.uvTransform.translate));
	materialData[drawCallIndex]->uvTransform = uvTransformMatrix;

	// 頂点リソース
	VertexData* vData = nullptr;
	HRESULT hr = vertexResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&vData));
	if (FAILED(hr) || vData == nullptr) return;
	std::memcpy(vData + spriteVertexDataUsed, &spriteVertexData[spriteVertexDataUsed], sizeof(VertexData) * kSumVertex);
	vertexResourceSprite->Unmap(0, nullptr);

	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	vertexBufferView.BufferLocation = vertexResourceSprite->GetGPUVirtualAddress() + sizeof(VertexData) * (spriteVertexDataUsed);
	vertexBufferView.SizeInBytes = sizeof(VertexData) * static_cast<UINT>(kSumVertex);
	vertexBufferView.StrideInBytes = sizeof(VertexData);



	// RootSignatureを設定。
	dxManager->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);
	// 形状を設定
	dxManager->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// CBVを設定する マテリアル用のCBufferの場所を設定
	dxManager->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResources[drawCallIndex]->GetGPUVirtualAddress());
	// CBVを設定する wvp用のCBufferの場所を設定
	dxManager->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResources[drawCallIndex]->GetGPUVirtualAddress());
	// SRVのDescriptorTableの先頭を設定。２はrootParameters[2]。
	dxManager->GetCommandList()->SetGraphicsRootDescriptorTable(2, tex->textureSrvHandleGPU);
	// CBVを設定する ディレクショナルライト用のCBufferの場所を設定
	dxManager->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource->GetGPUVirtualAddress());

	// 描画
	dxManager->GetCommandList()->DrawInstanced(3, 1, 0, 0);

	drawCallIndex++;
	spriteVertexDataUsed += kSumVertex;
}

void Engine::DrawSprite(const Transforms& transform, const Vector2& center, uint32_t textureNumber, const uint32_t& materialColor, const DrawOptions drawOptions)
{
	// 描画回数上限
	if (drawCallIndex >= kMaxDrawCallPerFrame) return;

	// RootSignatureとPSOを設定 - Triangle
	dxManager->GetCommandList()->SetPipelineState(dxManager->GetPipelineStateManager()->GetPipelineState()); // Triangle用PSOを設定
	dxManager->GetCommandList()->SetGraphicsRootSignature(dxManager->GetPipelineStateManager()->GetRootSignature()); // 共通のルートシグネチャ

	// 必要な頂点数
	const uint32_t kSumVertex = 4;
	// 必要な頂点数分配列を拡張
	if (spriteVertexDataUsed + kSumVertex > spriteVertexData.size())
	{
		spriteVertexData.resize(spriteVertexDataUsed + kSumVertex);
	}

	// テクスチャ
	const TextureData* tex = dxManager->GetTextureManager()->GetTexture(textureNumber);
	if (!tex)return;

	// 頂点
	const float halfWidth = tex->metadata.width * 0.5f;
	const float halfHeight = tex->metadata.height * 0.5f;

	// 左下 (index 0)
	spriteVertexData[spriteVertexDataUsed + 0].position = { -halfWidth, -halfHeight, 0.0f, 1.0f };
	spriteVertexData[spriteVertexDataUsed + 0].texcoord = { 0.0f, 0.0f };
	spriteVertexData[spriteVertexDataUsed + 0].normal = { 0.0f, 0.0f, -1.0f };

	// 左上 (index 1)
	spriteVertexData[spriteVertexDataUsed + 1].position = { halfWidth, -halfHeight, 0.0f, 1.0f };
	spriteVertexData[spriteVertexDataUsed + 1].texcoord = { 1.0f, 0.0f };
	spriteVertexData[spriteVertexDataUsed + 1].normal = { 0.0f, 0.0f, -1.0f };

	// 右下 (index 2)
	spriteVertexData[spriteVertexDataUsed + 2].position = { -halfWidth, halfHeight, 0.0f, 1.0f };
	spriteVertexData[spriteVertexDataUsed + 2].texcoord = { 0.0f, 1.0f };
	spriteVertexData[spriteVertexDataUsed + 2].normal = { 0.0f, 0.0f, -1.0f };

	// 右上 (index 3)
	spriteVertexData[spriteVertexDataUsed + 3].position = { halfWidth, halfHeight, 0.0f, 1.0f };
	spriteVertexData[spriteVertexDataUsed + 3].texcoord = { 1.0f, 1.0f };
	spriteVertexData[spriteVertexDataUsed + 3].normal = { 0.0f, 0.0f, -1.0f }; 

	// WVP行列
	Matrix4x4 orthoProjectionMatrix = Matrix4x4::MakeOrthographicMatrix(
		0.0f, 0.0f,
		static_cast<float>(windowManager->Getwidth()),
		static_cast<float>(windowManager->Getheight()),
		0.0f, 100.0f);
	Matrix4x4 world = Matrix4x4::MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
	Matrix4x4 wvpMatrix = (world * orthoProjectionMatrix);

	wvpData[drawCallIndex]->World = world;
	wvpData[drawCallIndex]->WVP = wvpMatrix;

	
	// マテリアル
	float uvCenterX = (center.x) - (halfWidth);
	float uvCenterY = (center.y) - (halfHeight);
	uvCenterX = uvCenterX / halfWidth / 2;
	uvCenterY = uvCenterY / halfHeight / 2;

	Matrix4x4 toCenter = Matrix4x4::MakeTranslateMatrix({ -uvCenterX, -uvCenterY, 0.0f });
	Matrix4x4 fromCenter = Matrix4x4::MakeTranslateMatrix({ uvCenterX, uvCenterY, 0.0f });
	Matrix4x4 uvTransformMatrix = Matrix4x4::MakeIdentity4x4();
		uvTransformMatrix = (uvTransformMatrix * Matrix4x4::MakeScaleMatrix(drawOptions.uvTransform.scale));
		uvTransformMatrix = (uvTransformMatrix * Matrix4x4::MakeRotateZMatrix(drawOptions.uvTransform.rotate.z));

	uvTransformMatrix = (fromCenter * (uvTransformMatrix * toCenter));

	uvTransformMatrix = (uvTransformMatrix * Matrix4x4::MakeTranslateMatrix(drawOptions.uvTransform.translate));


	Vector4 color = ConvertUintToVector4(materialColor);
	materialData[drawCallIndex]->color = color;
	materialData[drawCallIndex]->enableLighting = false;
	materialData[drawCallIndex]->uvTransform = uvTransformMatrix;
	

	// 頂点リソース
	VertexData* vData = nullptr;
	HRESULT hr = vertexResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&vData));
	if (FAILED(hr) || vData == nullptr) return;
	std::memcpy(vData + spriteVertexDataUsed, &spriteVertexData[spriteVertexDataUsed], sizeof(VertexData) * kSumVertex);
	vertexResourceSprite->Unmap(0, nullptr);

	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	vertexBufferView.BufferLocation = vertexResourceSprite->GetGPUVirtualAddress() + sizeof(VertexData) * (spriteVertexDataUsed);
	vertexBufferView.SizeInBytes = sizeof(VertexData) * static_cast<UINT>(kSumVertex);
	vertexBufferView.StrideInBytes = sizeof(VertexData);



	// Spriteの描画
	dxManager->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);
	dxManager->GetCommandList()->IASetIndexBuffer(&indexBufferView);
	// 形状を設定
	dxManager->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// CBVを設定する マテリアル用のCBufferの場所を設定
	dxManager->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResources[drawCallIndex]->GetGPUVirtualAddress());
	// CBVを設定する wvp用のCBufferの場所を設定
	dxManager->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResources[drawCallIndex]->GetGPUVirtualAddress());
	// SRVのDescriptorTableの先頭を設定。２はrootParameters[2]。
	dxManager->GetCommandList()->SetGraphicsRootDescriptorTable(2, tex->textureSrvHandleGPU);
	// CBVを設定する ディレクショナルライト用のCBufferの場所を設定
	dxManager->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource->GetGPUVirtualAddress());

	// 描画
	//dxManager->GetCommandList()->DrawIndexedInstanced(6, 1, 0, static_cast<UINT>(spriteVertexDataUsed), 0);
	dxManager->GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);

	drawCallIndex++;
	spriteVertexDataUsed += kSumVertex;
}

void Engine::DrawLine(const Vector3& start, const Vector3& end, const uint32_t& materialColor)
{
	if (drawLineCallIndex >= kMaxDrawLineCallPerFrame) return;

	// RootSignatureとPSOを設定 - Line
	dxManager->GetCommandList()->SetPipelineState(dxManager->GetPipelineStateManager()->GetLinePipelineState()); // Line用PSOを設定
	dxManager->GetCommandList()->SetGraphicsRootSignature(dxManager->GetPipelineStateManager()->GetRootSignature()); // 共通のルートシグネチャ

	// 頂点データの準備
	VertexData vertices[2];
	vertices[0].position = { start.x, start.y, start.z, 1.0f };
	vertices[0].texcoord = { 0.0f, 0.0f };
	vertices[0].normal = { 0.0f, 0.0f, 1.0f };

	vertices[1].position = { end.x, end.y, end.z, 1.0f };
	vertices[1].texcoord = { 0.0f, 0.0f };
	vertices[1].normal = { 0.0f, 0.0f, 1.0f };

	UINT currentLineVertexOffset = static_cast<UINT>(drawLineCallIndex * 2);

	// 頂点バッファへのデータ書き込み
	// Mapして直接書き込む
	VertexData* mappedVertexData = nullptr;
	HRESULT hr = vertexResourceLine->Map(0, nullptr, reinterpret_cast<void**>(&mappedVertexData));
	assert(SUCCEEDED(hr));
	memcpy(mappedVertexData + currentLineVertexOffset, vertices, sizeof(VertexData) * 2);
	vertexResourceLine->Unmap(0, nullptr);

	// 頂点バッファビューの設定
	D3D12_VERTEX_BUFFER_VIEW vertexBufferViewLine{};
	vertexBufferViewLine.BufferLocation = vertexResourceLine->GetGPUVirtualAddress() + sizeof(VertexData) * currentLineVertexOffset;;
	vertexBufferViewLine.SizeInBytes = sizeof(VertexData) * 2;
	vertexBufferViewLine.StrideInBytes = sizeof(VertexData);
	dxManager->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferViewLine);

	// プリミティブトポロジーの設定
	dxManager->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST); // 直線を設定

	// マテリアル定数バッファの更新
	Vector4 color = ConvertUintToVector4(materialColor);
	materialDataLine[drawLineCallIndex]->color = color;
	materialDataLine[drawLineCallIndex]->enableLighting = 0; // 線にライト要らない
	materialDataLine[drawLineCallIndex]->uvTransform = Matrix4x4::MakeIdentity4x4(); // 線にUV変換いらない
	dxManager->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResourceLine[drawLineCallIndex]->GetGPUVirtualAddress());// b1にバインド

	// WVP行列定数バッファの更新 (カメラのWVP行列を使用)
	Matrix4x4 wvpMatrix = cameraController->viewProjectionMatrix; // カメラのViewProjection行列
	wvpDataLine[drawLineCallIndex]->WVP = wvpMatrix;
	wvpDataLine[drawLineCallIndex]->World = Matrix4x4::MakeIdentity4x4();
	dxManager->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResourceLine[drawLineCallIndex]->GetGPUVirtualAddress()); // b0にバインド


	// 描画コマンドの発行
	dxManager->GetCommandList()->DrawInstanced(2, 1, 0, 0);

	drawLineCallIndex++;
}

// 音
void Engine::PlayAudio(const uint32_t& audioId, bool loop)
{
	dxManager->GetAudioManager()->PlayAudio(audioId, loop);
}

void Engine::StopAudio(const uint32_t& audioId)
{
	dxManager->GetAudioManager()->StopAudio(audioId);
}

void Engine::SetAudioVolume(const uint32_t& audioId, float volume)
{
	dxManager->GetAudioManager()->SetVolume(audioId, volume);
}

void Engine::SetMasterVolume(float volume)
{
	dxManager->GetAudioManager()->SetMasterVolume(volume);
}

float Engine::GetVolume(const uint32_t& audioId)
{
	return dxManager->GetAudioManager()->GetVolume(audioId);
}

float Engine::GetMasterVolume()
{
	return dxManager->GetAudioManager()->GetMasterVolume();
}

bool Engine::IsAudioPlaying(const uint32_t& audioId)
{
	return dxManager->GetAudioManager()->IsAudioPlaying(audioId);
}


// 入力
void Engine::GetMousePosition(Vector2* position)
{
	// hwnd: ゲームウィンドウのハンドル（WindowManagerなどから取得）
	POINT mousePosScreen;
	GetCursorPos(&mousePosScreen); // 画面座標で取得

	// クライアント座標（ウィンドウ左上基準）に変換
	ScreenToClient(windowManager->GetHwnd(), &mousePosScreen);

	// mousePosScreen.x, mousePosScreen.y がウィンドウ内のマウス座標
	position->x = float(mousePosScreen.x);
	position->y = float(mousePosScreen.y);
}

void Engine::SetMouseRay()
{
	// hwnd: ゲームウィンドウのハンドル（WindowManagerなどから取得）
	POINT mousePosScreen;
	GetCursorPos(&mousePosScreen); // 画面座標で取得

	// クライアント座標（ウィンドウ左上基準）に変換
	ScreenToClient(windowManager->GetHwnd(), &mousePosScreen);

	// mousePosScreen.x, mousePosScreen.y がウィンドウ内のマウス座標
	mouseController->SetMousePosition({ float(mousePosScreen.x) ,float(mousePosScreen.y) });
	mouseController->SetMouseRay(windowManager->Getwidth(), windowManager->Getheight(), cameraController->viewProjectionMatrix);

	//DrawLine(mouseController->GetMouseRay().origin, mouseController->GetMouseRay().diff * 10000, 0xFFFF00FF);
}

Ray Engine::GetMouseRay()
{
	return mouseController->GetMouseRay();
}

bool Engine::IsCollisionMouseRayAABB(uint32_t objectNumber, const Transforms& data)
{
	AABB aabb = CreateAABB(data, objectNumber);
	return IsCollision(mouseController->GetMouseRay(), objects[objectNumber].modelData.vertices, aabb, data);
};

bool Engine::IsPressMouse(int i)
{
	// 左クリック
	if (i == 0)
	{
		bool leftButton = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
		return leftButton;
	}
	// 右クリック
	if (i == 1)
	{
		bool rightButton = (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0;
		return rightButton;
	}
	// ミドルボタン（マウスホイールクリック）
	if (i == 2)
	{
		bool middleButton = (GetAsyncKeyState(VK_MBUTTON) & 0x8000) != 0;
		return middleButton;
	}

	return false;
}

uint32_t Engine::GetMouseWheel()
{
	uint32_t delta = wheelDelta;
	return delta;
}

// カメラ操作
void Engine::MoveCenterTarget(Vector3 target, int spendFrame)
{
	cameraController->SetCenterTarget(target, spendFrame);
}

void Engine::MoveRotateTarget(Vector3 target, int spendFrame)
{
	cameraController->SetRotateTarget(target, spendFrame);
}

void Engine::MoveDistanceTarget(float target, int spendFrame)
{
	cameraController->SetDistanceTarget(target, spendFrame);
}

void Engine::SetControlModeCamera(bool mode)
{
	cameraController->cameraMode_ = mode;
}

void Engine::SetControlModeCameraCenter(bool mode)
{
	cameraController->cameraMode_centerControl_ = mode;
	if (mode = true && cameraController->cameraMode_ == false)cameraController->cameraMode_ = true;
}

void Engine::SetControlModeCameraRotate(bool mode)
{
	cameraController->cameraMode_rotateControl_ = mode;
	if (mode = true && cameraController->cameraMode_ == false)cameraController->cameraMode_ = true;
}

void Engine::SetControlModeCameraDistance(bool mode)
{
	cameraController->cameraMode_distanceControl_ = mode;
	if (mode = true && cameraController->cameraMode_ == false)cameraController->cameraMode_ = true;
}


// 座標とかない、本当にただモデルの形のAABBを作るだけの関数（LoadOBJの時のAABB初期化用）
AABB Engine::CreateLocalAABB(const ModelData& model)
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

// CreateLocalAABBでつくったAABBに座標を適応させる（当たり判定の毎フレーム更新用）
AABB Engine::CreateAABB(const Transforms& transforms, uint32_t objectNumber)
{
	Matrix4x4 worldMatrix = transforms.World;

	Object3D& obj = objects[objectNumber];

	// ローカルAABBの8頂点
	Vector3 corners[8] = {
		{obj.aabb.min.x, obj.aabb.min.y, obj.aabb.min.z},
		{obj.aabb.max.x, obj.aabb.min.y, obj.aabb.min.z},
		{obj.aabb.min.x, obj.aabb.max.y, obj.aabb.min.z},
		{obj.aabb.max.x, obj.aabb.max.y, obj.aabb.min.z},
		{obj.aabb.min.x, obj.aabb.min.y, obj.aabb.max.z},
		{obj.aabb.max.x, obj.aabb.min.y, obj.aabb.max.z},
		{obj.aabb.min.x, obj.aabb.max.y, obj.aabb.max.z},
		{obj.aabb.max.x, obj.aabb.max.y, obj.aabb.max.z},
	};

	// 8頂点をワールド空間に変換
	Vector3 worldMin = Transform(corners[0], worldMatrix);
	Vector3 worldMax = worldMin;

	for (int i = 1; i < 8; ++i)
	{
		Vector3 v = Transform(corners[i], worldMatrix);
		worldMin.x = my_min(worldMin.x, v.x);
		worldMin.y = my_min(worldMin.y, v.y);
		worldMin.z = my_min(worldMin.z, v.z);
		worldMax.x = my_max(worldMax.x, v.x);
		worldMax.y = my_max(worldMax.y, v.y);
		worldMax.z = my_max(worldMax.z, v.z);
	}
	return { worldMin, worldMax };
}

void Engine::toggleWireframeMode()
{
	if (WireframeMode)WireframeMode = false;
	else WireframeMode = true;
}


void Engine::InitializeLineResources(ID3D12Device* device)
{
	HRESULT hr;

	// Line描画用の頂点バッファを確保（2頂点分）
	vertexResourceSizeLine = static_cast<UINT>(sizeof(VertexData) * 2 * kMaxDrawLineCallPerFrame);
	vertexResourceLine = CreateBufferResource(device, vertexResourceSizeLine);

	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;


	// WVPバッファの記述 (定数バッファは256バイトアライメントが必要)
	D3D12_RESOURCE_DESC wvpBufferDesc{};
	wvpBufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	wvpBufferDesc.Width = (sizeof(TransformationMatrix) + 0xff) & ~0xff; // 256バイトアライメント
	wvpBufferDesc.Height = 1;
	wvpBufferDesc.DepthOrArraySize = 1;
	wvpBufferDesc.MipLevels = 1;
	wvpBufferDesc.Format = DXGI_FORMAT_UNKNOWN;
	wvpBufferDesc.SampleDesc.Count = 1;
	wvpBufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// マテリアルバッファの記述 (各マテリアルは小さい)
	D3D12_RESOURCE_DESC materialBufferDesc{};
	materialBufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	materialBufferDesc.Width = sizeof(Material); // 1つのMaterial構造体のサイズ
	materialBufferDesc.Height = 1;
	materialBufferDesc.DepthOrArraySize = 1;
	materialBufferDesc.MipLevels = 1;
	materialBufferDesc.Format = DXGI_FORMAT_UNKNOWN;
	materialBufferDesc.SampleDesc.Count = 1;
	materialBufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	for (size_t i = 0; i < kMaxDrawLineCallPerFrame; ++i)
	{
		// マテリアルリソースの作成とマップ
		hr = device->CreateCommittedResource(
			&heapProperties,
			D3D12_HEAP_FLAG_NONE,
			&materialBufferDesc, // materialBufferDesc を使用
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&materialResourceLine[i]));
		assert(SUCCEEDED(hr));
		materialResourceLine[i]->SetName(L"materialResourceLine");
		materialResourceLine[i]->Map(0, nullptr, reinterpret_cast<void**>(&materialDataLine[i]));

		// WVPリソースの作成とマップ
		hr = device->CreateCommittedResource(
			&heapProperties,
			D3D12_HEAP_FLAG_NONE,
			&wvpBufferDesc, // wvpBufferDesc を使用
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&wvpResourceLine[i]));
		assert(SUCCEEDED(hr));
		wvpResourceLine[i]->SetName(L"wvpResourceLine");
		wvpResourceLine[i]->Map(0, nullptr, reinterpret_cast<void**>(&wvpDataLine[i]));
	}
}