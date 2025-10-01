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
#include "Engine/Game.h"

#include <DirectXMath.h>
#include <filesystem>
using namespace DirectX;


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
		windowManager = new  WindowManager(width, height, title);
	}
	if (!dxManager)
	{
		dxManager = new DirectXManager(windowManager->GetHwnd(), width, height);
	}

	// カメラ
	cameraController = new CameraController();
	debugCameraController = new CameraController();
	cameraController->cameraMode_ = false;
	debugCamera = true;

	// インプット系
	inputManager_ = new Input(windowManager->GetHwnd(), windowManager->Getwidth(), windowManager->Getheight(), &cameraController->viewProjectionMatrix, &debugCameraController->viewProjectionMatrix, &debugCamera);

	/// imguiの初期化
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(windowManager->GetHwnd());
	ImGui_ImplDX12_Init(
		dxManager->GetDevice(),
		//dxManager->GetSwapChainDesc().BufferCount,
		dxManager->GetSwapChainManager()->GetSwapChainDesc().BufferCount,
		dxManager->GetRtvDesc().Format,
		dxManager->GetsrvDescriptorHeap(),
		dxManager->GetsrvDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(),
		dxManager->GetsrvDescriptorHeap()->GetGPUDescriptorHandleForHeapStart()
	);

	// 頂点リソース
	vertexResourceSizeSprite = static_cast<UINT>(sizeof(VertexData) * 1536); // スプライト 
	vertexResourceSprite = CreateBufferResource(dxManager->GetDevice(), vertexResourceSizeSprite);

	vertexResourceSizeObj = static_cast<UINT>(sizeof(VertexData) * 4096); // オブジェクト
	//vertexResourceSizeObj = static_cast<UINT>(sizeof(VertexData) * 300000); // オブジェクト
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


	// フルスクリーンクアッド用頂点データ
	VertexData quadVertices[4] = {
		{ { -1.0f, -1.0f, 0.0f, 1.0f }, { 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f } },
		{ { -1.0f,  1.0f, 0.0f, 1.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } },
		{ {  1.0f, -1.0f, 0.0f, 1.0f }, { 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f } },
		{ {  1.0f,  1.0f, 0.0f, 1.0f }, { 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } },
	};
	uint16_t quadIndices[6] = { 0, 1, 2, 2, 1, 3 };

	// 頂点バッファ
	fullScreenQuadVertexBuffer = CreateBufferResource(dxManager->GetDevice(), sizeof(quadVertices));
	void* mappedVB = nullptr;
	fullScreenQuadVertexBuffer->Map(0, nullptr, &mappedVB);
	memcpy(mappedVB, quadVertices, sizeof(quadVertices));
	fullScreenQuadVertexBuffer->Unmap(0, nullptr);

	fullScreenQuadVBView.BufferLocation = fullScreenQuadVertexBuffer->GetGPUVirtualAddress();
	fullScreenQuadVBView.SizeInBytes = sizeof(quadVertices);
	fullScreenQuadVBView.StrideInBytes = sizeof(VertexData);

	// インデックスバッファ
	fullScreenQuadIndexBuffer = CreateBufferResource(dxManager->GetDevice(), sizeof(quadIndices));
	void* mappedIB = nullptr;
	fullScreenQuadIndexBuffer->Map(0, nullptr, &mappedIB);
	memcpy(mappedIB, quadIndices, sizeof(quadIndices));
	fullScreenQuadIndexBuffer->Unmap(0, nullptr);

	fullScreenQuadIBView.BufferLocation = fullScreenQuadIndexBuffer->GetGPUVirtualAddress();
	fullScreenQuadIBView.SizeInBytes = sizeof(quadIndices);
	fullScreenQuadIBView.Format = DXGI_FORMAT_R16_UINT;

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

	// オフスクリーンレンダリング
	D3D12_RESOURCE_DESC desc = {};
	desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	desc.Width = windowManager->Getwidth();
	desc.Height = windowManager->Getheight();
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

	D3D12_HEAP_PROPERTIES heapProps = {};
	heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;

	D3D12_CLEAR_VALUE clearValue = {};
	clearValue.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	clearValue.Color[0] = 0.0f;
	clearValue.Color[1] = 0.0f;
	clearValue.Color[2] = 0.0f;
	clearValue.Color[3] = 1.0f;

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
	// DirectXを更新
	dxManager->BeginFrame();

	const float clearColor[4] = { 0.1f,0.25f,0.5f,1.0f };
	ID3D12GraphicsCommandList* cmd = dxManager->BeginScene(clearColor);

	// ダブルバッファ化したコマンドリストのインデックス取得
	frameIndex = dxManager->GetSwapChainManager()->GetCurrentBackBufferIndex();

	// ImGuiフレーム開始
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// ???
	sphereVertexDataUsed = 0;
	spriteVertexDataUsed = 0;

	// ライトを更新
	UpdateLight();

	// カメラを更新
	UpdateCamera();

	// インプット系を更新
	inputManager_->Update();
}
void Engine::UpdateLight()
{
	// ライトの向きを正規化
	directionalLightData->direction = (directionalLightData->direction.Normalized());
}
void Engine::UpdateCamera()
{
	// カメラの更新（シェイクも自動的に処理される）
	cameraController->Update();
	debugCameraController->Update();

	// 視錐台平面を更新
	if (!debugCamera)
	{
		CreateFrustumPlanes(cameraController->viewProjectionMatrix);
	}
	else
	{
		CreateFrustumPlanes(debugCameraController->viewProjectionMatrix);
	}

	ImGui::Text("---------------camera---------------");
	ImGui::Checkbox("switchDebugCamera", &debugCamera);
}
void Engine::EndFrame()
{
	wheelDelta = 0;

	// 事前にコマンドリスト、スワップチェインを取得しておく
	ID3D12GraphicsCommandList* cmd = dxManager->BeginPostProcess();

	// ポストエフェクト PSO / RS 設定
	cmd->SetGraphicsRootSignature(dxManager->GetPipelineStateManager()->GetRootSignature());
	cmd->SetPipelineState(dxManager->GetPipelineStateManager()->GetPipelineState(BlendMode::kBlendModeNone, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE));


	// オフスクリーンSRVヒープ
	{
		ID3D12DescriptorHeap* heaps[] = { dxManager->GetSwapChainManager()->GetOffscreenSRVDescriptorHeap() };
		cmd->SetDescriptorHeaps(1, heaps);
		cmd->SetGraphicsRootDescriptorTable(0, dxManager->GetSwapChainManager()->GetOffscreenSRVDescriptorHeap()->GetGPUDescriptorHandleForHeapStart());
	}
	DrawFullScreenQuad(cmd);

	// ImGui
	ImGui::Render();

	// 7. 終了処理（フェンス・Present 等）
	dxManager->EndFrame(true);
	drawCallIndex = 0;
	drawLineCallIndex = 0;
}


void Engine::UpdateTransforms()
{

#pragma region モデルリスト取得

	const auto& modelList = Game::GetModelList();

#pragma endregion

#pragma region 座標更新 & 描画範囲内判定
	
	// オブジェクト更新
	for (auto& rd : modelList)
	{
		rd->Updata(objects);
	}

#pragma endregion

#pragma region マウスレイ衝突判定

	// マウスレイ取得
	const Ray mouseRay = inputManager_->GetMouseController()->GetMouseRay();
	// モデルと衝突までの距離セット構造体
	struct HitInfo { Game::RenderData_Model* rdm; float distance; };
	// のリスト
	std::vector<HitInfo> hits;
	// のリサイズ(リサイズではない)
	hits.reserve(modelList.size());

	// 描画範囲内のオブジェクトを全て調査
	for (auto& rd : modelList)
	{
		rd->isCollisionMouseRay = -1;
		if (rd->inPicture)
		{
			float minDistance = (std::numeric_limits<float>::max)();
			std::optional<Vector3> nearestColPos;

			for (const auto& aabb : rd->aabb)
			{
				std::optional<Vector3> colPos = IntersectRayModel(
					mouseRay,
					objects[rd->model].modelData.vertices,
					aabb, rd->transforms
				);
				if (colPos)
				{
					float d = (colPos.value() - mouseRay.origin).Length();
					if (d < minDistance)
					{
						minDistance = d;
						nearestColPos = colPos;
					}
				}
			}

			if (nearestColPos)
			{
				hits.push_back({ rd, minDistance });
			}
		}
	}

	// 距離の昇順でソート
	std::sort(hits.begin(), hits.end(),
		[](auto& a, auto& b) { return a.distance < b.distance; });

	// ソート後に順序を割り当て
	for (int order = 0; order < (int)hits.size(); ++order)
	{
		hits[order].rdm->isCollisionMouseRay = order;
	}

#pragma endregion

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

	// COMの終了処理
	CoUninitialize();

	// 解放
	delete windowManager;
	windowManager = nullptr;
	delete dxManager;
	dxManager = nullptr;
	delete cameraController;
	cameraController = nullptr;
	delete debugCameraController;
	debugCameraController = nullptr;
	delete inputManager_;
	inputManager_ = nullptr;
}

// リソース読み込み
uint32_t Engine::LoadTexture(const std::string& filePath)
{
	return dxManager->GetTextureManager()->LoadTexture(filePath, dxManager->GetCommandList(frameIndex));
}

uint32_t Engine::LoadOBJ(const std::string& directoryPath, const std::string& filename)
{
	// ボックスを作成
	Object3D obj;
	// モデルデータ
	obj.modelData = LoadOBJFile(directoryPath, filename);
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

std::vector<AABB> Engine::LoadAABB(const std::string& csvPath, const ModelData& model)
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
void Engine::Drawobj(Game::RenderData_Model& renderData)
{
	// 画面内か判定
	if (!renderData.inPicture)return;

	// 描画
	{
		// 描画回数上限
		if (drawCallIndex >= kMaxDrawCallPerFrame) return;

		if (renderData.model >= objects.size()) return;

		// RootSignatureとPSOを設定
		dxManager->GetCommandList(frameIndex)->SetGraphicsRootSignature(dxManager->GetPipelineStateManager()->GetRootSignature()); // 共通のルートシグネチャ
		if (renderData.options.wireframe || WireframeMode)
		{	 // ワイヤーフレーム用PSOを設定
			dxManager->GetCommandList(frameIndex)->SetPipelineState(dxManager->GetPipelineStateManager()->GetPipelineState(BlendMode::Wireframe, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE));
		}
		else
		{	// Triangle用PSOを設定
			dxManager->GetCommandList(frameIndex)->SetPipelineState(dxManager->GetPipelineStateManager()->GetPipelineState(renderData.options.blendMode, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE));
		}

		// 描画するモデルの検索
		Object3D& obj = objects[renderData.model];
		// 頂点数の取得
		const uint32_t kSumVertex = static_cast<uint32_t>(obj.modelData.vertices.size());

		// WVP行列
		wvpData[drawCallIndex]->World = renderData.transforms.World;
		if (!debugCamera)
		{
			wvpData[drawCallIndex]->WVP = renderData.transforms.World * cameraController->viewProjectionMatrix;
		}
		else
		{
			wvpData[drawCallIndex]->WVP = renderData.transforms.World * debugCameraController->viewProjectionMatrix;
		}

		const TextureData* tex = dxManager->GetTextureManager()->GetTexture(renderData.texture);
		if (!tex) return;

		Vector4 color = ConvertUintToVector4(renderData.color);
		materialData[drawCallIndex]->color = color;
		materialData[drawCallIndex]->enableLighting = renderData.options.enableLighting;
		Matrix4x4 uvTransformMatrix = Matrix4x4::MakeIdentity4x4();
		uvTransformMatrix = (uvTransformMatrix * Matrix4x4::MakeScaleMatrix(renderData.uvTransform.scale));
		uvTransformMatrix = (uvTransformMatrix * Matrix4x4::MakeRotateZMatrix(renderData.uvTransform.rotate.z));
		uvTransformMatrix = (uvTransformMatrix * Matrix4x4::MakeTranslateMatrix(renderData.uvTransform.translate));
		materialData[drawCallIndex]->uvTransform = uvTransformMatrix;


		// 頂点バッファをバインド（描画に使う頂点データを指定）
		dxManager->GetCommandList(frameIndex)->IASetVertexBuffers(0, 1, &obj.vertexBufferView);
		// プリミティブトポロジ（描画する形状の種類：三角形リスト）を設定
		dxManager->GetCommandList(frameIndex)->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		// ルートパラメータ0にマテリアル用定数バッファ（色・ライティング情報など）をバインド
		dxManager->GetCommandList(frameIndex)->SetGraphicsRootConstantBufferView(0, materialResources[drawCallIndex]->GetGPUVirtualAddress());
		// ルートパラメータ1にWVP（ワールド・ビュー・プロジェクション）用定数バッファをバインド
		dxManager->GetCommandList(frameIndex)->SetGraphicsRootConstantBufferView(1, wvpResources[drawCallIndex]->GetGPUVirtualAddress());
		// ルートパラメータ2にテクスチャのSRV（シェーダリソースビュー）をバインド
		dxManager->GetCommandList(frameIndex)->SetGraphicsRootDescriptorTable(2, tex->textureSrvHandleGPU);
		// ルートパラメータ3にディレクショナルライト用定数バッファをバインド
		dxManager->GetCommandList(frameIndex)->SetGraphicsRootConstantBufferView(3, directionalLightResource->GetGPUVirtualAddress());
		// 頂点数分のインスタンス描画を実行（実際に描画コマンドを発行）
		dxManager->GetCommandList(frameIndex)->DrawInstanced(kSumVertex, 1, 0, 0);

		drawCallIndex++;
	}
}

void Engine::DrawSphere(const Transforms& transform, const Vector3& center, uint32_t kSubdivision, uint32_t textureNumber, const uint32_t& materialColor, const DrawOptions drawOptions)
{
	// 描画回数上限
	if (drawCallIndex >= kMaxDrawCallPerFrame) return;

	// RootSignatureとPSOを設定
	dxManager->GetCommandList(frameIndex)->SetGraphicsRootSignature(dxManager->GetPipelineStateManager()->GetRootSignature()); // 共通のルートシグネチャ
	if (drawOptions.wireframe || WireframeMode)
	{	 // ワイヤーフレーム用PSOを設定
		dxManager->GetCommandList(frameIndex)->SetPipelineState(dxManager->GetPipelineStateManager()->GetPipelineState(BlendMode::Wireframe, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE));
	}
	else
	{	// Triangle用PSOを設定
		dxManager->GetCommandList(frameIndex)->SetPipelineState(dxManager->GetPipelineStateManager()->GetPipelineState(drawOptions.blendMode, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE));
	}

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
	Matrix4x4 wvpMatrix;
	if (!debugCamera)
	{
		wvpMatrix = worldMatrix * cameraController->viewProjectionMatrix;
	}
	else
	{
		wvpMatrix = worldMatrix * debugCameraController->viewProjectionMatrix;
	}

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
	//uvTransformMatrix = (uvTransformMatrix * Matrix4x4::MakeScaleMatrix(drawOptions.uvTransform.scale));
	//uvTransformMatrix = (uvTransformMatrix * Matrix4x4::MakeRotateZMatrix(drawOptions.uvTransform.rotate.z));
	//uvTransformMatrix = (uvTransformMatrix * Matrix4x4::MakeTranslateMatrix(drawOptions.uvTransform.translate));
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
	dxManager->GetCommandList(frameIndex)->IASetVertexBuffers(0, 1, &vertexBufferView);
	dxManager->GetCommandList(frameIndex)->IASetIndexBuffer(&indexBufferView);
	// 形状を設定
	dxManager->GetCommandList(frameIndex)->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// CBVを設定する マテリアル用のCBufferの場所を設定
	dxManager->GetCommandList(frameIndex)->SetGraphicsRootConstantBufferView(0, materialResources[drawCallIndex]->GetGPUVirtualAddress());
	// CBVを設定する wvp用のCBufferの場所を設定
	dxManager->GetCommandList(frameIndex)->SetGraphicsRootConstantBufferView(1, wvpResources[drawCallIndex]->GetGPUVirtualAddress());
	// SRVのDescriptorTableの先頭を設定。２はrootParameters[2]。
	dxManager->GetCommandList(frameIndex)->SetGraphicsRootDescriptorTable(2, tex->textureSrvHandleGPU);
	// CBVを設定する ディレクショナルライト用のCBufferの場所を設定
	dxManager->GetCommandList(frameIndex)->SetGraphicsRootConstantBufferView(3, directionalLightResource->GetGPUVirtualAddress());

	dxManager->GetCommandList(frameIndex)->DrawInstanced(kSumVertex, 1, 0, 0);

	drawCallIndex++;
	sphereVertexDataUsed += kSumVertex;
}

void Engine::DrawTriangle(Game::RenderData_Triangle& renderData)
{
	// 描画回数上限
	if (drawCallIndex >= kMaxDrawCallPerFrame) return;

	// RootSignatureとPSOを設定
	dxManager->GetCommandList(frameIndex)->SetGraphicsRootSignature(dxManager->GetPipelineStateManager()->GetRootSignature()); // 共通のルートシグネチャ
	if (renderData.options.wireframe || WireframeMode)
	{	 // ワイヤーフレーム用PSOを設定
		dxManager->GetCommandList(frameIndex)->SetPipelineState(dxManager->GetPipelineStateManager()->GetPipelineState(BlendMode::Wireframe, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE));
	}
	else
	{	// Triangle用PSOを設定
		dxManager->GetCommandList(frameIndex)->SetPipelineState(dxManager->GetPipelineStateManager()->GetPipelineState(renderData.options.blendMode, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE));
	}

	// 必要な頂点数
	const uint32_t kSumVertex = 4;
	// 必要な頂点数分配列を拡張
	if (spriteVertexDataUsed + kSumVertex > spriteVertexData.size())
	{
		spriteVertexData.resize(spriteVertexDataUsed + kSumVertex);
	}

	// テクスチャ
	const TextureData* tex = dxManager->GetTextureManager()->GetTexture(renderData.texture);
	if (!tex) return;

	// 上
	spriteVertexData[spriteVertexDataUsed + 0].position = { renderData.pos1.x, renderData.pos1.y, renderData.pos1.z, 1.0f };
	spriteVertexData[spriteVertexDataUsed + 0].texcoord = { 0.5f, 0.0f };
	spriteVertexData[spriteVertexDataUsed + 0].normal = { 0.0f, 0.0f, -1.0f };

	// 右下
	spriteVertexData[spriteVertexDataUsed + 1].position = { renderData.pos2.x, renderData.pos2.y, renderData.pos2.z, 1.0f };
	spriteVertexData[spriteVertexDataUsed + 1].texcoord = { 1.0f, 1.0f };
	spriteVertexData[spriteVertexDataUsed + 1].normal = { 0.0f, 0.0f, -1.0f };

	// 左下
	spriteVertexData[spriteVertexDataUsed + 2].position = { renderData.pos3.x,renderData.pos3.y, renderData.pos3.z, 1.0f };
	spriteVertexData[spriteVertexDataUsed + 2].texcoord = { 0.0f, 1.0f };
	spriteVertexData[spriteVertexDataUsed + 2].normal = { 0.0f, 0.0f, -1.0f };

	// WVP行列
	Matrix4x4 world = Matrix4x4::MakeAffineMatrix(renderData.transform.scale, renderData.transform.rotate, renderData.transform.translate);
	Matrix4x4 viewProj;
	if (!debugCamera)
	{
		viewProj = cameraController->viewProjectionMatrix;
	}
	else
	{
		viewProj = debugCameraController->viewProjectionMatrix;
	}
	Matrix4x4 wvpMatrix = world * viewProj;

	wvpData[drawCallIndex]->World = world;
	wvpData[drawCallIndex]->WVP = wvpMatrix;

	// マテリアル
	Vector4 color = ConvertUintToVector4(renderData.color);
	materialData[drawCallIndex]->color = color;
	materialData[drawCallIndex]->enableLighting = renderData.options.enableLighting;
	Matrix4x4 uvTransformMatrix = Matrix4x4::MakeIdentity4x4();
	uvTransformMatrix = (uvTransformMatrix * Matrix4x4::MakeScaleMatrix(renderData.uvTransform.scale));
	uvTransformMatrix = (uvTransformMatrix * Matrix4x4::MakeRotateZMatrix(renderData.uvTransform.rotate.z));
	uvTransformMatrix = (uvTransformMatrix * Matrix4x4::MakeTranslateMatrix(renderData.uvTransform.translate));
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
	dxManager->GetCommandList(frameIndex)->IASetVertexBuffers(0, 1, &vertexBufferView);
	// 形状を設定
	dxManager->GetCommandList(frameIndex)->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// CBVを設定する マテリアル用のCBufferの場所を設定
	dxManager->GetCommandList(frameIndex)->SetGraphicsRootConstantBufferView(0, materialResources[drawCallIndex]->GetGPUVirtualAddress());
	// CBVを設定する wvp用のCBufferの場所を設定
	dxManager->GetCommandList(frameIndex)->SetGraphicsRootConstantBufferView(1, wvpResources[drawCallIndex]->GetGPUVirtualAddress());
	// SRVのDescriptorTableの先頭を設定。２はrootParameters[2]。
	dxManager->GetCommandList(frameIndex)->SetGraphicsRootDescriptorTable(2, tex->textureSrvHandleGPU);
	// CBVを設定する ディレクショナルライト用のCBufferの場所を設定
	dxManager->GetCommandList(frameIndex)->SetGraphicsRootConstantBufferView(3, directionalLightResource->GetGPUVirtualAddress());

	// 描画
	dxManager->GetCommandList(frameIndex)->DrawInstanced(3, 1, 0, 0);

	drawCallIndex++;
	spriteVertexDataUsed += kSumVertex;
}

void Engine::DrawSprite(Game::RenderData_Sprite& renderData)
{
	// 描画回数上限
	if (drawCallIndex >= kMaxDrawCallPerFrame) return;

	// RootSignatureとPSOを設定 - Triangle
	dxManager->GetCommandList(frameIndex)->SetGraphicsRootSignature(dxManager->GetPipelineStateManager()->GetRootSignature()); // 共通のルートシグネチャ
	dxManager->GetCommandList(frameIndex)->SetPipelineState(dxManager->GetPipelineStateManager()->GetPipelineState(renderData.options.blendMode, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE)); // Triangle用PSOを設定

	// 必要な頂点数
	const uint32_t kSumVertex = 4;
	// 必要な頂点数分配列を拡張
	if (spriteVertexDataUsed + kSumVertex > spriteVertexData.size())
	{
		spriteVertexData.resize(spriteVertexDataUsed + kSumVertex);
	}

	// テクスチャ
	const TextureData* tex = dxManager->GetTextureManager()->GetTexture(renderData.texture);
	if (!tex)return;

	// 頂点
	float halfWidth = static_cast<float>(tex->metadata.width) * 0.5f;
	float halfHeight = static_cast<float>(tex->metadata.height) * 0.5f;

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

	switch (renderData.anker)
	{
	case Anker::Center:
	{
		// 左下 (index 0)
		spriteVertexData[spriteVertexDataUsed + 0].position.x;
		spriteVertexData[spriteVertexDataUsed + 0].position.y;
		// 左上 (index 1)
		spriteVertexData[spriteVertexDataUsed + 1].position.x;
		spriteVertexData[spriteVertexDataUsed + 1].position.y;
		// 右下 (index 2)
		spriteVertexData[spriteVertexDataUsed + 2].position.x;
		spriteVertexData[spriteVertexDataUsed + 2].position.y;
		// 右上 (index 3)
		spriteVertexData[spriteVertexDataUsed + 3].position.x;
		spriteVertexData[spriteVertexDataUsed + 3].position.y;
		break;
	}
	case Anker::CenterLeft:
	{
		// 左下 (index 0)
		spriteVertexData[spriteVertexDataUsed + 0].position.x += halfWidth;
		spriteVertexData[spriteVertexDataUsed + 0].position.y;
		// 左上 (index 1)
		spriteVertexData[spriteVertexDataUsed + 1].position.x += halfWidth;
		spriteVertexData[spriteVertexDataUsed + 1].position.y;
		// 右下 (index 2)
		spriteVertexData[spriteVertexDataUsed + 2].position.x += halfWidth;
		spriteVertexData[spriteVertexDataUsed + 2].position.y;
		// 右上 (index 3)
		spriteVertexData[spriteVertexDataUsed + 3].position.x += halfWidth;
		spriteVertexData[spriteVertexDataUsed + 3].position.y;
		break;
	}
	case Anker::CenterRight:
	{
		// 左下 (index 0)
		spriteVertexData[spriteVertexDataUsed + 0].position.x += -halfWidth;
		spriteVertexData[spriteVertexDataUsed + 0].position.y;
		// 左上 (index 1)
		spriteVertexData[spriteVertexDataUsed + 1].position.x += -halfWidth;
		spriteVertexData[spriteVertexDataUsed + 1].position.y;
		// 右下 (index 2)
		spriteVertexData[spriteVertexDataUsed + 2].position.x += -halfWidth;
		spriteVertexData[spriteVertexDataUsed + 2].position.y;
		// 右上 (index 3)
		spriteVertexData[spriteVertexDataUsed + 3].position.x += -halfWidth;
		spriteVertexData[spriteVertexDataUsed + 3].position.y;
		break;
	}
	case Anker::CenterTop:
	{
		// 左下 (index 0)
		spriteVertexData[spriteVertexDataUsed + 0].position.x;
		spriteVertexData[spriteVertexDataUsed + 0].position.y += halfHeight;
		// 左上 (index 1)
		spriteVertexData[spriteVertexDataUsed + 1].position.x;
		spriteVertexData[spriteVertexDataUsed + 1].position.y += halfHeight;
		// 右下 (index 2)
		spriteVertexData[spriteVertexDataUsed + 2].position.x;
		spriteVertexData[spriteVertexDataUsed + 2].position.y += halfHeight;
		// 右上 (index 3)
		spriteVertexData[spriteVertexDataUsed + 3].position.x;
		spriteVertexData[spriteVertexDataUsed + 3].position.y += halfHeight;
		break;
	}
	case Anker::CenterDown:
	{
		// 左下 (index 0)
		spriteVertexData[spriteVertexDataUsed + 0].position.x;
		spriteVertexData[spriteVertexDataUsed + 0].position.y += -halfHeight;
		// 左上 (index 1)
		spriteVertexData[spriteVertexDataUsed + 1].position.x;
		spriteVertexData[spriteVertexDataUsed + 1].position.y += -halfHeight;
		// 右下 (index 2)
		spriteVertexData[spriteVertexDataUsed + 2].position.x;
		spriteVertexData[spriteVertexDataUsed + 2].position.y += -halfHeight;
		// 右上 (index 3)
		spriteVertexData[spriteVertexDataUsed + 3].position.x;
		spriteVertexData[spriteVertexDataUsed + 3].position.y += -halfHeight;
		break;
	}
	case Anker::LeftTop:
	{
		// 左下 (index 0)
		spriteVertexData[spriteVertexDataUsed + 0].position.x += halfWidth;
		spriteVertexData[spriteVertexDataUsed + 0].position.y += halfHeight;
		// 左上 (index 1)
		spriteVertexData[spriteVertexDataUsed + 1].position.x += halfWidth;
		spriteVertexData[spriteVertexDataUsed + 1].position.y += halfHeight;
		// 右下 (index 2)
		spriteVertexData[spriteVertexDataUsed + 2].position.x += halfWidth;
		spriteVertexData[spriteVertexDataUsed + 2].position.y += halfHeight;
		// 右上 (index 3)
		spriteVertexData[spriteVertexDataUsed + 3].position.x += halfWidth;
		spriteVertexData[spriteVertexDataUsed + 3].position.y += halfHeight;
		break;
	}
	case Anker::RightTop:
	{
		// 左下 (index 0)
		spriteVertexData[spriteVertexDataUsed + 0].position.x += -halfWidth;
		spriteVertexData[spriteVertexDataUsed + 0].position.y += halfHeight;
		// 左上 (index 1)
		spriteVertexData[spriteVertexDataUsed + 1].position.x += -halfWidth;
		spriteVertexData[spriteVertexDataUsed + 1].position.y += halfHeight;
		// 右下 (index 2)
		spriteVertexData[spriteVertexDataUsed + 2].position.x += -halfWidth;
		spriteVertexData[spriteVertexDataUsed + 2].position.y += halfHeight;
		// 右上 (index 3)
		spriteVertexData[spriteVertexDataUsed + 3].position.x += -halfWidth;
		spriteVertexData[spriteVertexDataUsed + 3].position.y += halfHeight;
		break;
	}
	case Anker::LeftDown:
	{
		// 左下 (index 0)
		spriteVertexData[spriteVertexDataUsed + 0].position.x += halfWidth;
		spriteVertexData[spriteVertexDataUsed + 0].position.y += -halfHeight;
		// 左上 (index 1)
		spriteVertexData[spriteVertexDataUsed + 1].position.x += halfWidth;
		spriteVertexData[spriteVertexDataUsed + 1].position.y += -halfHeight;
		// 右下 (index 2)
		spriteVertexData[spriteVertexDataUsed + 2].position.x += halfWidth;
		spriteVertexData[spriteVertexDataUsed + 2].position.y += -halfHeight;
		// 右上 (index 3)
		spriteVertexData[spriteVertexDataUsed + 3].position.x += halfWidth;
		spriteVertexData[spriteVertexDataUsed + 3].position.y += -halfHeight;
		break;
	}
	case Anker::RightDown:
	{
		// 左下 (index 0)
		spriteVertexData[spriteVertexDataUsed + 0].position.x += -halfWidth;
		spriteVertexData[spriteVertexDataUsed + 0].position.y += -halfHeight;
		// 左上 (index 1)
		spriteVertexData[spriteVertexDataUsed + 1].position.x += -halfWidth;
		spriteVertexData[spriteVertexDataUsed + 1].position.y += -halfHeight;
		// 右下 (index 2)
		spriteVertexData[spriteVertexDataUsed + 2].position.x += -halfWidth;
		spriteVertexData[spriteVertexDataUsed + 2].position.y += -halfHeight;
		// 右上 (index 3)
		spriteVertexData[spriteVertexDataUsed + 3].position.x += -halfWidth;
		spriteVertexData[spriteVertexDataUsed + 3].position.y += -halfHeight;
		break;
	}
	default:
		break;
	}


	// WVP行列
	Matrix4x4 orthoProjectionMatrix = Matrix4x4::MakeOrthographicMatrix(
		0.0f, 0.0f,
		static_cast<float>(windowManager->Getwidth()),
		static_cast<float>(windowManager->Getheight()),
		0.0f, 100.0f);
	Matrix4x4 world = Matrix4x4::MakeAffineMatrix(renderData.transforms.scale, renderData.transforms.rotate, renderData.transforms.translate);
	Matrix4x4 wvpMatrix = (world * orthoProjectionMatrix);

	wvpData[drawCallIndex]->World = world;
	wvpData[drawCallIndex]->WVP = wvpMatrix;


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

	// Spriteの描画
	dxManager->GetCommandList(frameIndex)->IASetVertexBuffers(0, 1, &vertexBufferView);
	dxManager->GetCommandList(frameIndex)->IASetIndexBuffer(&indexBufferView);
	// 形状を設定
	dxManager->GetCommandList(frameIndex)->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// CBVを設定する マテリアル用のCBufferの場所を設定
	dxManager->GetCommandList(frameIndex)->SetGraphicsRootConstantBufferView(0, materialResources[drawCallIndex]->GetGPUVirtualAddress());
	// CBVを設定する wvp用のCBufferの場所を設定
	dxManager->GetCommandList(frameIndex)->SetGraphicsRootConstantBufferView(1, wvpResources[drawCallIndex]->GetGPUVirtualAddress());
	// SRVのDescriptorTableの先頭を設定。２はrootParameters[2]。
	dxManager->GetCommandList(frameIndex)->SetGraphicsRootDescriptorTable(2, tex->textureSrvHandleGPU);
	// CBVを設定する ディレクショナルライト用のCBufferの場所を設定
	dxManager->GetCommandList(frameIndex)->SetGraphicsRootConstantBufferView(3, directionalLightResource->GetGPUVirtualAddress());

	// 描画
	dxManager->GetCommandList(frameIndex)->DrawIndexedInstanced(6, 1, 0, 0, 0);

	drawCallIndex++;
	spriteVertexDataUsed += kSumVertex;
}

void Engine::DrawLine(const Vector3& start, const Vector3& end, const uint32_t& materialColor)
{
	if (drawLineCallIndex >= kMaxDrawLineCallPerFrame) return;

	// RootSignatureとPSOを設定 - Line
	dxManager->GetCommandList(frameIndex)->SetGraphicsRootSignature(dxManager->GetPipelineStateManager()->GetRootSignature()); // 共通のルートシグネチャ
	dxManager->GetCommandList(frameIndex)->SetPipelineState(dxManager->GetPipelineStateManager()->GetPipelineState(BlendMode::kBlendModeNormal, D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE)); // Line用PSOを設定

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
	dxManager->GetCommandList(frameIndex)->IASetVertexBuffers(0, 1, &vertexBufferViewLine);

	// プリミティブトポロジーの設定
	dxManager->GetCommandList(frameIndex)->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST); // 直線を設定

	// マテリアル定数バッファの更新
	Vector4 color = ConvertUintToVector4(materialColor);
	materialDataLine[drawLineCallIndex]->color = color;
	materialDataLine[drawLineCallIndex]->enableLighting = 0; // 線にライト要らない
	materialDataLine[drawLineCallIndex]->uvTransform = Matrix4x4::MakeIdentity4x4(); // 線にUV変換いらない
	dxManager->GetCommandList(frameIndex)->SetGraphicsRootConstantBufferView(0, materialResourceLine[drawLineCallIndex]->GetGPUVirtualAddress());// b1にバインド

	// WVP行列定数バッファの更新 (カメラのWVP行列を使用)
	Matrix4x4 wvpMatrix = cameraController->viewProjectionMatrix;
	if (!debugCamera)
	{
		wvpMatrix = cameraController->viewProjectionMatrix;
	}
	else
	{
		wvpMatrix = debugCameraController->viewProjectionMatrix;
	}
	wvpDataLine[drawLineCallIndex]->WVP = wvpMatrix;
	wvpDataLine[drawLineCallIndex]->World = Matrix4x4::MakeIdentity4x4();
	dxManager->GetCommandList(frameIndex)->SetGraphicsRootConstantBufferView(1, wvpResourceLine[drawLineCallIndex]->GetGPUVirtualAddress()); // b0にバインド


	// 描画コマンドの発行
	dxManager->GetCommandList(frameIndex)->DrawInstanced(2, 1, 0, 0);

	drawLineCallIndex++;
}

void Engine::DrawParticle(Game::RenderData_Particle& renderData)
{
	if (renderData.frame >= renderData.emissionDelay)
	{
		for (int i = 0; i < renderData.particlesPerEmission; ++i)
		{
			// AABB逆転対策
			AABB buf = renderData.emitterAABB;
			renderData.emitterAABB.min.x = my_min(buf.min.x, buf.max.x);
			renderData.emitterAABB.max.x = my_max(buf.min.x, buf.max.x);
			renderData.emitterAABB.min.y = my_min(buf.min.y, buf.max.y);
			renderData.emitterAABB.max.y = my_max(buf.min.y, buf.max.y);
			renderData.emitterAABB.min.z = my_min(buf.min.z, buf.max.z);
			renderData.emitterAABB.max.z = my_max(buf.min.z, buf.max.z);

			// フレームリセット
			renderData.frame = 0;

			////////////// オブジェクト作成 //////////////
			Game::RenderData_Model model;

			// エミッターがAABB型だった場合
			if (renderData.option.emitterShape == true)
			{
				// エミッターが内部を指す場合
				if (renderData.option.spawnInsideEmitter == true)
				{
					model.transforms.translate.x = RandomFloat(renderData.emitterAABB.min.x, renderData.emitterAABB.max.x, 3);
					model.transforms.translate.y = RandomFloat(renderData.emitterAABB.min.y, renderData.emitterAABB.max.y, 3);
					model.transforms.translate.z = RandomFloat(renderData.emitterAABB.min.z, renderData.emitterAABB.max.z, 3);
				}
				// エミッターが外殻を指す場合
				else
				{
					int i = RandomInt(1, 6);
					if (i == 1 || i == 2)
					{
						if (i == 1)
						{
							model.transforms.translate.x = renderData.emitterAABB.min.x;
						}
						else
						{
							model.transforms.translate.x = renderData.emitterAABB.max.x;
						}
						model.transforms.translate.y = RandomFloat(renderData.emitterAABB.min.y, renderData.emitterAABB.max.y, 3);
						model.transforms.translate.z = RandomFloat(renderData.emitterAABB.min.z, renderData.emitterAABB.max.z, 3);
					}
					else if (i == 3 || i == 4)
					{
						if (i == 3)
						{
							model.transforms.translate.y = renderData.emitterAABB.min.y;
						}
						else
						{
							model.transforms.translate.y = renderData.emitterAABB.max.y;
						}
						model.transforms.translate.x = RandomFloat(renderData.emitterAABB.min.x, renderData.emitterAABB.max.x, 3);
						model.transforms.translate.z = RandomFloat(renderData.emitterAABB.min.z, renderData.emitterAABB.max.z, 3);
					}
					else if (i == 5 || i == 6)
					{
						if (i == 5)
						{
							model.transforms.translate.z = renderData.emitterAABB.min.z;
						}
						else
						{
							model.transforms.translate.z = renderData.emitterAABB.max.z;
						}
						model.transforms.translate.y = RandomFloat(renderData.emitterAABB.min.y, renderData.emitterAABB.max.y, 3);
						model.transforms.translate.x = RandomFloat(renderData.emitterAABB.min.x, renderData.emitterAABB.max.x, 3);
					}
				}
			}
			// エミッターが球型だった場合
			else
			{
				// エミッターが内部を指す場合
				if (renderData.option.spawnInsideEmitter == true)
				{
					Vector3 center = renderData.emitterSphere.center;
					Vector3 radius = renderData.emitterSphere.radius;

					// ランダムな方向（単位ベクトル）を生成
					float theta = RandomFloat(0.0f, 2.0f * float(std::numbers::pi), 3);       // 0〜2π
					float phi = RandomFloat(0.0f, float(std::numbers::pi), 3);              // 0〜π
					float r = RandomFloat(0.0f, 1.0f, 3);            // 0〜1（球内）

					// 球内部の距離に合わせてスケーリング（立方根で均等分布）
					r = pow(r, 1.0f / 3.0f);

					// 球面座標系から直交座標系へ変換
					float x = r * sin(phi) * cos(theta) * radius.x;
					float y = r * sin(phi) * sin(theta) * radius.y;
					float z = r * cos(phi) * radius.z;

					model.transforms.translate.x = center.x + x;
					model.transforms.translate.y = center.y + y;
					model.transforms.translate.z = center.z + z;
				}
				// エミッターが外殻を指す場合
				else
				{
					Vector3 center = renderData.emitterSphere.center;
					Vector3 radius = renderData.emitterSphere.radius;

					// ランダムな方向（単位ベクトル）を生成
					float theta = RandomFloat(0.0f, 2.0f * float(std::numbers::pi), 3); // 0〜2π
					float phi = RandomFloat(0.0f, float(std::numbers::pi), 3);        // 0〜π

					// r = 1.0f 固定 → 外殻のみ
					float x = sin(phi) * cos(theta) * radius.x;
					float y = sin(phi) * sin(theta) * radius.y;
					float z = cos(phi) * radius.z;

					model.transforms.translate.x = center.x + x;
					model.transforms.translate.y = center.y + y;
					model.transforms.translate.z = center.z + z;
				}

			}

			model.transforms.rotate = renderData.mono.transforms.rotate;
			model.transforms.scale = renderData.mono.transforms.scale;
			model.model = renderData.mono.model;
			model.texture = renderData.mono.texture;
			model.color = renderData.mono.color;
			model.options = renderData.mono.options;

			// 速度設定
			ParticleInf inf;
			if (renderData.option.targetDirection == true)
			{
				inf.velocity = renderData.target - Vector3{ (renderData.emitterAABB.max + renderData.emitterAABB.min) / 2.0f };
			}
			else
			{
				inf.velocity = renderData.target - model.transforms.translate;
			}
			inf.velocity.Normalize();
			inf.velocity *= renderData.velocity;

			// 人生設計
			inf.liveTime = renderData.liveMax;

			// リストに追加
			renderData.GetModelList().push_back(model);
			renderData.GetInfList().push_back(inf);
		}
	}


	for (int i = 0; i < renderData.GetModelList().size(); ++i)
	{
		if (renderData.GetInfList()[i].liveTime > 0)
		{
			// 更新
			renderData.GetModelList()[i].transforms.translate += renderData.GetInfList()[i].velocity;
			renderData.GetModelList()[i].transforms.rotate += renderData.AddRotate;
			renderData.GetModelList()[i].transforms.scale += renderData.AddScale;

			//static float floatColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
			//ImGui::ColorEdit4("model2.color", floatColor, 1);
			//Vector4 vector4Color = { floatColor[0], floatColor[1], floatColor[2], floatColor[3] };
			//model1PreColor = ConvertVector4ToUint(vector4Color);
			//renderData.GetModelList()[i].color -= renderData.AddColor;

			if (renderData.GetModelList()[i].transforms.scale.x < 0)
				renderData.GetModelList()[i].transforms.scale.x = 0.0f;
			if (renderData.GetModelList()[i].transforms.scale.y < 0)
				renderData.GetModelList()[i].transforms.scale.y = 0.0f;
			if (renderData.GetModelList()[i].transforms.scale.z < 0)
				renderData.GetModelList()[i].transforms.scale.z = 0.0f;

			// ワールド行列更新
			renderData.GetModelList()[i].transforms.World =
				Matrix4x4::MakeAffineMatrix(
					renderData.GetModelList()[i].transforms.scale,
					renderData.GetModelList()[i].transforms.rotate,
					renderData.GetModelList()[i].transforms.translate);

			// 人生消費
			renderData.GetInfList()[i].liveTime--;

			// 描画
			renderData.GetModelList()[i].Draw();
		}

		if (
			// 生存時間０の時
			renderData.GetInfList()[i].liveTime <= 0
			// 大きさ０の時
			|| (renderData.GetModelList()[i].transforms.scale.x <= 0.0f || renderData.GetModelList()[i].transforms.scale.y <= 0.0f || renderData.GetModelList()[i].transforms.scale.z <= 0.0f)
			// アルファ値０の時
			//|| renderData.GetModelList()[i].color
			)
		{
			renderData.GetModelList().erase(renderData.GetModelList().begin() + i);
			renderData.GetInfList().erase(renderData.GetInfList().begin() + i);
		}
	}

	renderData.frame++;
}

void Engine::DrawFullScreenQuad(ID3D12GraphicsCommandList* cmdList)
{
	cmdList->IASetVertexBuffers(0, 1, &fullScreenQuadVBView);
	cmdList->IASetIndexBuffer(&fullScreenQuadIBView);
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	cmdList->DrawIndexedInstanced(6, 1, 0, 0, 0);
}
//{
//	// dxManager からデバイス取得
//	ID3D12Device* device = dxManager->GetDevice();
//
//	// 頂点データ（NDC座標系、UV、法線はZ+）
//	VertexData quadVertices[4] = {
//		//   position                texcoord   normal
//		{ { -1.0f, -1.0f, 0.0f, 1.0f }, { 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f } }, // 左下
//		{ { -1.0f,  1.0f, 0.0f, 1.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } }, // 左上
//		{ {  1.0f, -1.0f, 0.0f, 1.0f }, { 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f } }, // 右下
//		{ {  1.0f,  1.0f, 0.0f, 1.0f }, { 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } }, // 右上
//	};
//	uint16_t quadIndices[6] = { 0, 1, 2, 2, 1, 3 };
//
//    // 頂点バッファ作成
//	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer;
//	{
//		D3D12_HEAP_PROPERTIES heapProps = {};
//		heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
//		D3D12_RESOURCE_DESC resDesc = {};
//		resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
//		resDesc.Width = sizeof(quadVertices);
//		resDesc.Height = 1;
//		resDesc.DepthOrArraySize = 1;
//		resDesc.MipLevels = 1;
//		resDesc.Format = DXGI_FORMAT_UNKNOWN;
//		resDesc.SampleDesc.Count = 1;
//		resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
//		resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
//		HRESULT hr = device->CreateCommittedResource(
//			&heapProps, D3D12_HEAP_FLAG_NONE, &resDesc,
//			D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&vertexBuffer));
//		void* mapped = nullptr;
//		vertexBuffer->Map(0, nullptr, &mapped);
//		memcpy(mapped, quadVertices, sizeof(quadVertices));
//		vertexBuffer->Unmap(0, nullptr);
//	}
//	D3D12_VERTEX_BUFFER_VIEW vbView = {};
//	vbView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
//	vbView.SizeInBytes = sizeof(quadVertices);
//	vbView.StrideInBytes = sizeof(VertexData);
//
//	// インデックスバッファ作成
//	Microsoft::WRL::ComPtr<ID3D12Resource> indexBuffer;
//	{
//		D3D12_HEAP_PROPERTIES heapProps = {};
//		heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
//		D3D12_RESOURCE_DESC resDesc = {};
//		resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
//		resDesc.Width = sizeof(quadIndices);
//		resDesc.Height = 1;
//		resDesc.DepthOrArraySize = 1;
//		resDesc.MipLevels = 1;
//		resDesc.Format = DXGI_FORMAT_UNKNOWN;
//		resDesc.SampleDesc.Count = 1;
//		resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
//		resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
//		HRESULT hr = device->CreateCommittedResource(
//			&heapProps, D3D12_HEAP_FLAG_NONE, &resDesc,
//			D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&indexBuffer));
//		void* mapped = nullptr;
//		indexBuffer->Map(0, nullptr, &mapped);
//		memcpy(mapped, quadIndices, sizeof(quadIndices));
//		indexBuffer->Unmap(0, nullptr);
//	}
//	D3D12_INDEX_BUFFER_VIEW ibView = {};
//	ibView.BufferLocation = indexBuffer->GetGPUVirtualAddress();
//	ibView.SizeInBytes = sizeof(quadIndices);
//	ibView.Format = DXGI_FORMAT_R16_UINT;
//
//	// バッファバインド
//	cmdList->IASetVertexBuffers(0, 1, &vbView);
//	cmdList->IASetIndexBuffer(&ibView);
//	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//
//	// CBVバインド（Material, TransformationMatrix, DirectionalLight）
//	// ※ここでは既にSetGraphicsRootDescriptorTable/SetGraphicsRootConstantBufferView済みのはず
//	// 必要ならここで再バインド
//
//	// 描画
//	cmdList->DrawIndexedInstanced(6, 1, 0, 0, 0);
//}

void Engine::CreateFrustumPlanes(const Matrix4x4& viewProjectionMatrix)
{
	// Left Plane
	frustumPlanes_[0].normal.x = viewProjectionMatrix.m[0][3] + viewProjectionMatrix.m[0][0];
	frustumPlanes_[0].normal.y = viewProjectionMatrix.m[1][3] + viewProjectionMatrix.m[1][0];
	frustumPlanes_[0].normal.z = viewProjectionMatrix.m[2][3] + viewProjectionMatrix.m[2][0];
	frustumPlanes_[0].distance = viewProjectionMatrix.m[3][3] + viewProjectionMatrix.m[3][0];
	// Right Plane
	frustumPlanes_[1].normal.x = viewProjectionMatrix.m[0][3] - viewProjectionMatrix.m[0][0];
	frustumPlanes_[1].normal.y = viewProjectionMatrix.m[1][3] - viewProjectionMatrix.m[1][0];
	frustumPlanes_[1].normal.z = viewProjectionMatrix.m[2][3] - viewProjectionMatrix.m[2][0];
	frustumPlanes_[1].distance = viewProjectionMatrix.m[3][3] - viewProjectionMatrix.m[3][0];
	// Bottom Plane
	frustumPlanes_[2].normal.x = viewProjectionMatrix.m[0][3] + viewProjectionMatrix.m[0][1];
	frustumPlanes_[2].normal.y = viewProjectionMatrix.m[1][3] + viewProjectionMatrix.m[1][1];
	frustumPlanes_[2].normal.z = viewProjectionMatrix.m[2][3] + viewProjectionMatrix.m[2][1];
	frustumPlanes_[2].distance = viewProjectionMatrix.m[3][3] + viewProjectionMatrix.m[3][1];
	// Top Plane
	frustumPlanes_[3].normal.x = viewProjectionMatrix.m[0][3] - viewProjectionMatrix.m[0][1];
	frustumPlanes_[3].normal.y = viewProjectionMatrix.m[1][3] - viewProjectionMatrix.m[1][1];
	frustumPlanes_[3].normal.z = viewProjectionMatrix.m[2][3] - viewProjectionMatrix.m[2][1];
	frustumPlanes_[3].distance = viewProjectionMatrix.m[3][3] - viewProjectionMatrix.m[3][1];
	// Near Plane
	frustumPlanes_[4].normal.x = viewProjectionMatrix.m[0][2];
	frustumPlanes_[4].normal.y = viewProjectionMatrix.m[1][2];
	frustumPlanes_[4].normal.z = viewProjectionMatrix.m[2][2];
	frustumPlanes_[4].distance = viewProjectionMatrix.m[3][2];
	// Far Plane
	frustumPlanes_[5].normal.x = viewProjectionMatrix.m[0][3] - viewProjectionMatrix.m[0][2];
	frustumPlanes_[5].normal.y = viewProjectionMatrix.m[1][3] - viewProjectionMatrix.m[1][2];
	frustumPlanes_[5].normal.z = viewProjectionMatrix.m[2][3] - viewProjectionMatrix.m[2][2];
	frustumPlanes_[5].distance = viewProjectionMatrix.m[3][3] - viewProjectionMatrix.m[3][2];

	// 各平面を正規化
	for (int i = 0; i < 6; ++i)
	{
		float length = sqrt(frustumPlanes_[i].normal.x * frustumPlanes_[i].normal.x +
			frustumPlanes_[i].normal.y * frustumPlanes_[i].normal.y +
			frustumPlanes_[i].normal.z * frustumPlanes_[i].normal.z);
		frustumPlanes_[i].normal = frustumPlanes_[i].normal / length;
		frustumPlanes_[i].distance /= length;
	}
}

bool Engine::IsAABBInFrustum(const AABB& aabb, const Matrix4x4& worldMatrix)
{
	// AABBの8つの頂点をワールド空間に変換
	Vector3 points[8];
	//points[0] = Transform(Vector3{ aabb.min.x, aabb.min.y, aabb.min.z }, worldMatrix);
	//points[1] = Transform(Vector3{ aabb.max.x, aabb.min.y, aabb.min.z }, worldMatrix);
	//points[2] = Transform(Vector3{ aabb.max.x, aabb.max.y, aabb.min.z }, worldMatrix);
	//points[3] = Transform(Vector3{ aabb.min.x, aabb.max.y, aabb.min.z }, worldMatrix);
	//points[4] = Transform(Vector3{ aabb.min.x, aabb.min.y, aabb.max.z }, worldMatrix);
	//points[5] = Transform(Vector3{ aabb.max.x, aabb.min.y, aabb.max.z }, worldMatrix);
	//points[6] = Transform(Vector3{ aabb.max.x, aabb.max.y, aabb.max.z }, worldMatrix);
	//points[7] = Transform(Vector3{ aabb.min.x, aabb.max.y, aabb.max.z }, worldMatrix);

	points[0] = Vector3{ aabb.min.x, aabb.min.y, aabb.min.z };
	points[1] = Vector3{ aabb.max.x, aabb.min.y, aabb.min.z };
	points[2] = Vector3{ aabb.max.x, aabb.max.y, aabb.min.z };
	points[3] = Vector3{ aabb.min.x, aabb.max.y, aabb.min.z };
	points[4] = Vector3{ aabb.min.x, aabb.min.y, aabb.max.z };
	points[5] = Vector3{ aabb.max.x, aabb.min.y, aabb.max.z };
	points[6] = Vector3{ aabb.max.x, aabb.max.y, aabb.max.z };
	points[7] = Vector3{ aabb.min.x, aabb.max.y, aabb.max.z };

	// 6つの各平面に対してテスト
	for (const auto& plane : frustumPlanes_)
	{
		int inCount = 0;
		// AABBのすべての頂点が平面の裏側にあるかチェック
		for (int i = 0; i < 8; ++i)
		{
			float dist = plane.normal.Dot(points[i]) + plane.distance;
			if (dist >= 0)
			{
				inCount++;
			}
		}
		// すべての頂点が平面の裏側にある場合は、AABBは視錐台の外
		if (inCount == 0)
		{
			return false;
		}
	}

	return true; // どの平面の外側にもない場合は、視錐台内にあると判定
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
Vector2 Engine::GetMousePosition()
{
	return inputManager_->GetMouseController()->GetMousePosition();
}

Ray Engine::GetMouseRay()
{
	return inputManager_->GetMouseController()->GetMouseRay();
}

uint32_t Engine::GetMouseWheel()
{
	uint32_t delta = wheelDelta;
	return delta;
}

bool Engine::IsCollisionMouseRayAABB(uint32_t objectNumber, const Transforms& data)
{
	std::vector<AABB> aabbs = CreateAABB(data, objectNumber);
	Ray ray = inputManager_->GetMouseController()->GetMouseRay();

	// どれか1つでも衝突すればtrue
	for (const auto& aabb : aabbs)
	{
		if (IsCollision(ray, objects[objectNumber].modelData.vertices, aabb, data))
		{
			return true;
		}
	}
	return false;
}

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

// カメラ操作
void Engine::MoveCenterTarget(Vector3 target, int spendFrame, EaseType easetype)
{
	cameraController->SetCenterTarget(target, spendFrame, easetype);
}

void Engine::MoveRotateTarget(Vector3 target, int spendFrame, EaseType easetype)
{
	cameraController->SetRotateTarget(target, spendFrame, easetype);
}

void Engine::MoveDistanceTarget(float target, int spendFrame, EaseType easetype)
{
	cameraController->SetDistanceTarget(target, spendFrame, easetype);
}

void Engine::SetControlModeCamera(bool mode)
{
	cameraController->cameraMode_ = mode;
}

CameraController* Engine::GetCamera()
{
	return cameraController;
}

CameraController* Engine::GetDebugCamera()
{
	return debugCameraController;
}

//// カメラシェイク開始
//void Engine::StartCameraShake(float intensity, float duration, float frequency)
//{
//	// メインカメラとデバッグカメラ両方にシェイクを適用
//	cameraController->StartShake(intensity, duration, frequency);
//	debugCameraController->StartShake(intensity, duration, frequency);
//}
//
//// カメラシェイク中かどうか
//bool Engine::IsCameraShaking()
//{
//	// 現在アクティブなカメラのシェイク状態を返す
//	if (!debugCamera)
//	{
//		return cameraController->IsShaking();
//	}
//	else
//	{
//		return debugCameraController->IsShaking();
//	}
//}

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
std::vector<AABB>  Engine::CreateAABB(const Transforms& transforms, uint32_t objectNumber)
{
	Matrix4x4 worldMatrix = transforms.World;//
	const Game::RenderData_Model* renderModel = Game::GetModelList()[objectNumber];
	const Object3D& obj = objects[renderModel->model];


	std::vector<AABB> result;

	for (const auto& localAABB : obj.aabb)
	{
		// ローカルAABBの8頂点
		Vector3 corners[8] = {
			{localAABB.min.x, localAABB.min.y, localAABB.min.z},
			{localAABB.max.x, localAABB.min.y, localAABB.min.z},
			{localAABB.min.x, localAABB.max.y, localAABB.min.z},
			{localAABB.max.x, localAABB.max.y, localAABB.min.z},
			{localAABB.min.x, localAABB.min.y, localAABB.max.z},
			{localAABB.max.x, localAABB.min.y, localAABB.max.z},
			{localAABB.min.x, localAABB.max.y, localAABB.max.z},
			{localAABB.max.x, localAABB.max.y, localAABB.max.z},
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
		result.push_back({ worldMin, worldMax });
	}
	return result;
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