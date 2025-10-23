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
#include "Resource/Texture/TextureManager.h"
#include "DrawSystem/RenderData/RenderData.h"

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
		dxManager = new DirectXManager(windowManager->GetHwnd());
	}
	if (!drawSystem)
	{
		drawSystem = new DrawSystem(dxManager);
	}
	if (!cameraController)
	{
		cameraController = new CameraController();
		cameraController->cameraMode_ = false; // メインカメラは常に操作不能
	}
	if (!debugCameraController)
	{
		debugCameraController = new CameraController();
	}
	if (!inputManager_)
	{
		inputManager_ = new Input(windowManager->GetHwnd(), &cameraController->viewProjectionMatrix, &debugCameraController->viewProjectionMatrix, &debugCamera);
	}

	// カメラ
	debugCamera = false;	// 最初はデバッグカメラ

	// imguiの初期化
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(windowManager->GetHwnd());
	uint32_t slot = dxManager->GetDescriptorHeapManager()->AllocateSRVSlot();
	ImGui_ImplDX12_Init(
		dxManager->GetDevice(),
		dxManager->GetSwapChain()->GetSwapChainDesc().BufferCount,
		dxManager->GetSwapChain()->GetRtvDesc().Format,
		dxManager->GetDescriptorHeapManager()->GetSRVDescriptorHeap(),
		dxManager->GetDescriptorHeapManager()->GetCPUHandleAt(slot),                    // ImGuiフォントSRV用のCPUハンドル
		dxManager->GetDescriptorHeapManager()->GetGPUHandleAt(slot)                     // ImGuiフォントSRV用のGPUハンドル
	);



	inputManager_->GetMouseController()->wheelDelta = 0;

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
			inputManager_->GetMouseController()->wheelDelta += GET_WHEEL_DELTA_WPARAM(msg.wParam);
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return true;
}
void Engine::BeginFrame()
{
	// ImGuiを更新
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	//ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

	// カメラを更新
	UpdateCamera();

	// 描画関数初期化
	if (!debugCamera)drawSystem->BeginFrame(cameraController->viewProjectionMatrix);
	else drawSystem->BeginFrame(debugCameraController->viewProjectionMatrix);

	// デバッグ情報更新
	UpdateDebugInfo();

	// インプット系を更新
	inputManager_->Update();

	// DirectXを更新
	dxManager->BeginFrame();
}
void Engine::UpdateCamera()
{
	// カメラの更新（シェイクも自動的に処理される）
	cameraController->Update();
	debugCameraController->Update();

	// 左シフト＋左クリックでカメラターゲットをオブジェクトに合わせる
	if (GetHitKey::IsPressedNow(DIK_LSHIFT))
	{
		if (Game::GetMousePress(0) && !GetMousePrePress(0))
		{
			for (auto& rd : RenderData_Model::renderModels)
			{
				if (rd->isCollisionMouseRay == 0)
				{
					GetDebugCamera()->SetCenterTarget(rd->transforms.translate, 0, EaseType::IN_BACK);
				}
			}
		}
	}
}
void Engine::UpdateDebugInfo()
{
	if (GetHitKey::IsPressedDown(DIK_F1))
	{
		isDebugInfo = !isDebugInfo;
	}
	if (GetHitKey::IsPressedDown(DIK_F3))
	{
		ToggleCameraMode();
	}
	if (GetHitKey::IsPressedDown(DIK_F12))
	{
		ToggleFullscreen();
	}

	if (isDebugInfo)
	{
		static float fpsSmooth = 60.0f;
		float dt = dxManager->GetDeltaTime();
		float fps = (dt > 0.0f) ? 1.0f / dt : 0.0f;
		// 指数移動平均で平滑化（α=0.1）
		fpsSmooth += (fps - fpsSmooth) * 0.1f;

		ImGui::Begin("------debug info------");
		ImGui::Text("F1  : Hide this");
		ImGui::Text("F3  : Toggle Camera Mode");
		ImGui::Text("F12 : Toggle Fullscreen");
		ImGui::Text("DeltaTime: %.3f ms", dxManager->GetDeltaTime() * 1000.0f);
		ImGui::Text("FPS: %.1f ", 1.0f / dxManager->GetDeltaTime());
		ImGui::End();
	}
}
void Engine::EndFrame()
{
	if (!debugCamera) cameraController->Draw(debugCamera);
	else debugCameraController->Draw(debugCamera);
	ImGui::Render();

	// パーティクル更新
	drawSystem->EndFrame();

	// インプット系終了処理
	inputManager_->EndFrame();

	// DirectX終了処理
	dxManager->EndFrame();
}


void Engine::UpdateTransforms()
{

#pragma region モデルリスト取得

	const auto& modelList = RenderData_Model::renderModels;

#pragma endregion

#pragma region 座標更新 & 描画範囲内判定

	// オブジェクト更新
	std::vector<Object3D> objects = dxManager->GetResourceManager()->GetModelManager()->objects;
	for (auto& rd : modelList)
	{
		rd->Update(objects);
	}

#pragma endregion

#pragma region マウスレイ衝突判定

	// マウスレイ取得
	const Ray mouseRay = inputManager_->GetMouseController()->GetMouseRay();
	// モデルと衝突までの距離セット構造体
	struct HitInfo { RenderData_Model* rdm; float distance; };
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
	// ImGuiの終了処理
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	// COMの終了処理
	CoUninitialize();

	// 解放
	delete windowManager;
	windowManager = nullptr;
	delete dxManager;
	dxManager = nullptr;
	delete drawSystem;
	drawSystem = nullptr;
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
	return dxManager->GetResourceManager()->GetTextureManager()->LoadTexture(filePath, dxManager->GetCommandContextManager()->GetCommandList(), dxManager->GetDescriptorHeapManager(), dxManager->GetDevice());
}

uint32_t Engine::LoadModel(const std::string& directoryPath, const std::string& filename)
{
	return dxManager->GetResourceManager()->GetModelManager()->LoadModel(directoryPath, filename, dxManager->GetDevice());
}

uint32_t Engine::LoadAudio(const std::string& filePath)
{
	return dxManager->GetResourceManager()->GetAudioManager()->LoadAudio(filePath);
}

TextureData* Engine::GetTexture(uint32_t textureNumber)
{
	return dxManager->GetResourceManager()->GetTextureManager()->GetTexture(textureNumber);
}

size_t Engine::GetTextureCount()
{
	return dxManager->GetResourceManager()->GetTextureManager()->GetTextureCount();
}

// 描画
void Engine::DrawModel(RenderData_Model& renderData)
{
	drawSystem->DrawModel(renderData);
}

void Engine::DrawSphere(const Transforms& transform, const Vector3& center, uint32_t kSubdivision, uint32_t textureNumber, const uint32_t& materialColor, const DrawOptions drawOptions)
{
	//// 描画回数上限
	//if (drawCallIndex >= kMaxDrawCallPerFrame) return;

	//// RootSignatureとPSOを設定
	//dxManager->GetCommandList()->SetGraphicsRootSignature(dxManager->GetPipelineStateManager()->GetRootSignature()); // 共通のルートシグネチャ
	//if (drawOptions.wireframe || WireframeMode)
	//{	 // ワイヤーフレーム用PSOを設定
	//	dxManager->GetCommandList()->SetPipelineState(dxManager->GetPipelineStateManager()->GetPipelineState(BlendMode::Wireframe, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE));
	//}
	//else
	//{	// Triangle用PSOを設定
	//	dxManager->GetCommandList()->SetPipelineState(dxManager->GetPipelineStateManager()->GetPipelineState(drawOptions.blendMode, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE));
	//}

	//// 必要な頂点数
	//const uint32_t kSumVertex = kSubdivision * kSubdivision * 6;
	//// 必要な頂点数分配列を拡張
	//if (vertexDataUsed + kSumVertex > vertexData.size())
	//{
	//	vertexData.resize(vertexDataUsed + kSumVertex);
	//}

	//// 頂点
	//CreateSphere(&vertexData[vertexDataUsed], kSubdivision);


	//// 1. オブジェクトのスケール行列
	//Matrix4x4 scaleMatrix = Matrix4x4::MakeScaleMatrix(transform.scale);

	//// 2. ワールド空間での最終的な位置への移動行列
	//Matrix4x4 translateMatrix = Matrix4x4::MakeTranslateMatrix(transform.translate);

	//// 3. 回転の中心への移動 (centerを原点に移動)
	//Matrix4x4 toRotationCenter = Matrix4x4::MakeTranslateMatrix({ -center.x, -center.y, -center.z });

	//// 4. 回転行列 (transform.rotate を center を中心とする回転として使う)
	//Matrix4x4 rotateXMatrix = Matrix4x4::MakeRotateXMatrix(transform.rotate.x);
	//Matrix4x4 rotateYMatrix = Matrix4x4::MakeRotateYMatrix(transform.rotate.y);
	//Matrix4x4 rotateZMatrix = Matrix4x4::MakeRotateZMatrix(transform.rotate.z);
	//Matrix4x4 rotationMatrix = rotateZMatrix * rotateXMatrix * rotateYMatrix;

	//// 5. 回転後、元の回転中心の位置に戻す
	//Matrix4x4 fromRotationCenter = Matrix4x4::MakeTranslateMatrix(center);

	//// 最終的なワールド行列の構築
	//Matrix4x4 worldMatrix =
	//	scaleMatrix *		 // 1. 拡縮はどうでもいい
	//	toRotationCenter *	 // 2. 回転中心を原点に移動
	//	rotationMatrix *	 // 3. 原点で回転 (centerを中心とした回転)
	//	fromRotationCenter * // 4. 回転したものを元の回転中心に戻す
	//	translateMatrix;	 // 5. 最終的なワールド位置へ移動

	//// WVP行列
	//Matrix4x4 wvpMatrix;
	//if (!debugCamera)
	//{
	//	wvpMatrix = worldMatrix * cameraController->viewProjectionMatrix;
	//}
	//else
	//{
	//	wvpMatrix = worldMatrix * debugCameraController->viewProjectionMatrix;
	//}

	//wvpData[drawCallIndex]->World = worldMatrix;
	//wvpData[drawCallIndex]->WVP = wvpMatrix;

	//// テクスチャ
	//const TextureData* tex = dxManager->GetResourceManager()->GetTextureManager()->GetTexture(textureNumber);
	//if (!tex)return;

	//// マテリアル
	//Vector4 color = ConvertUintToVector4(materialColor);
	//materialData[drawCallIndex]->color = color;
	//materialData[drawCallIndex]->enableLighting = drawOptions.enableLighting;
	//Matrix4x4 uvTransformMatrix = Matrix4x4::MakeIdentity4x4();
	////uvTransformMatrix = (uvTransformMatrix * Matrix4x4::MakeScaleMatrix(drawOptions.uvTransform.scale));
	////uvTransformMatrix = (uvTransformMatrix * Matrix4x4::MakeRotateZMatrix(drawOptions.uvTransform.rotate.z));
	////uvTransformMatrix = (uvTransformMatrix * Matrix4x4::MakeTranslateMatrix(drawOptions.uvTransform.translate));
	//materialData[drawCallIndex]->uvTransform = uvTransformMatrix;


	//// 頂点リソース
	//VertexData* vData = nullptr;
	//HRESULT hr = vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vData));
	//if (FAILED(hr) || vData == nullptr) return;
	//std::memcpy(vData + vertexDataUsed, &vertexData[vertexDataUsed], sizeof(VertexData) * kSumVertex);
	//vertexResource->Unmap(0, nullptr);

	//// 頂点バッファビュー
	//D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	//vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress() + sizeof(VertexData) * vertexDataUsed;
	//vertexBufferView.SizeInBytes = sizeof(VertexData) * static_cast<UINT>(kSumVertex);
	//vertexBufferView.StrideInBytes = sizeof(VertexData);

	//// 描画処理
	//dxManager->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);
	//dxManager->GetCommandList()->IASetIndexBuffer(&indexBufferView);
	//// 形状を設定
	//dxManager->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//// CBVを設定する マテリアル用のCBufferの場所を設定
	//dxManager->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResources[drawCallIndex]->GetGPUVirtualAddress());
	//// CBVを設定する wvp用のCBufferの場所を設定
	//dxManager->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResources[drawCallIndex]->GetGPUVirtualAddress());
	//// SRVのDescriptorTableの先頭を設定。２はrootParameters[2]。
	//dxManager->GetCommandList()->SetGraphicsRootDescriptorTable(2, tex->textureSrvHandleGPU);
	//// CBVを設定する ディレクショナルライト用のCBufferの場所を設定
	//dxManager->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource->GetGPUVirtualAddress());

	//dxManager->GetCommandList()->DrawInstanced(kSumVertex, 1, 0, 0);

	//drawCallIndex++;
	//vertexDataUsed += kSumVertex;
}

void Engine::DrawTriangle(RenderData_Triangle& renderData)
{
	drawSystem->DrawTriangle(renderData);
}

void Engine::DrawSprite(RenderData_Sprite& renderData)
{
	drawSystem->DrawSprite(renderData);
}

void Engine::DrawLine(RenderData_Line& renderData)
{
	drawSystem->DrawLine(renderData);
}

void Engine::DrawParticle(RenderData_Particle& renderData)
{
	drawSystem->DrawParticle(renderData);
	//if (renderData.frame >= renderData.emissionDelay)
	//{
	//	for (int i = 0; i < renderData.particlesPerEmission; ++i)
	//	{
	//		// AABB逆転対策
	//		AABB buf = renderData.emitterAABB;
	//		renderData.emitterAABB.min.x = my_min(buf.min.x, buf.max.x);
	//		renderData.emitterAABB.max.x = my_max(buf.min.x, buf.max.x);
	//		renderData.emitterAABB.min.y = my_min(buf.min.y, buf.max.y);
	//		renderData.emitterAABB.max.y = my_max(buf.min.y, buf.max.y);
	//		renderData.emitterAABB.min.z = my_min(buf.min.z, buf.max.z);
	//		renderData.emitterAABB.max.z = my_max(buf.min.z, buf.max.z);
	//
	//		// フレームリセット
	//		renderData.frame = 0;
	//
	//		////////////// オブジェクト作成 //////////////
	//		Game::RenderData_Model model;
	//
	//		// エミッターがAABB型だった場合
	//		if (renderData.option.emitterShape == true)
	//		{
	//			// エミッターが内部を指す場合
	//			if (renderData.option.spawnInsideEmitter == true)
	//			{
	//				model.transforms.translate.x = RandomFloat(renderData.emitterAABB.min.x, renderData.emitterAABB.max.x, 3);
	//				model.transforms.translate.y = RandomFloat(renderData.emitterAABB.min.y, renderData.emitterAABB.max.y, 3);
	//				model.transforms.translate.z = RandomFloat(renderData.emitterAABB.min.z, renderData.emitterAABB.max.z, 3);
	//			}
	//			// エミッターが外殻を指す場合
	//			else
	//			{
	//				int i = RandomInt(1, 6);
	//				if (i == 1 || i == 2)
	//				{
	//					if (i == 1)
	//					{
	//						model.transforms.translate.x = renderData.emitterAABB.min.x;
	//					}
	//					else
	//					{
	//						model.transforms.translate.x = renderData.emitterAABB.max.x;
	//					}
	//					model.transforms.translate.y = RandomFloat(renderData.emitterAABB.min.y, renderData.emitterAABB.max.y, 3);
	//					model.transforms.translate.z = RandomFloat(renderData.emitterAABB.min.z, renderData.emitterAABB.max.z, 3);
	//				}
	//				else if (i == 3 || i == 4)
	//				{
	//					if (i == 3)
	//					{
	//						model.transforms.translate.y = renderData.emitterAABB.min.y;
	//					}
	//					else
	//					{
	//						model.transforms.translate.y = renderData.emitterAABB.max.y;
	//					}
	//					model.transforms.translate.x = RandomFloat(renderData.emitterAABB.min.x, renderData.emitterAABB.max.x, 3);
	//					model.transforms.translate.z = RandomFloat(renderData.emitterAABB.min.z, renderData.emitterAABB.max.z, 3);
	//				}
	//				else if (i == 5 || i == 6)
	//				{
	//					if (i == 5)
	//					{
	//						model.transforms.translate.z = renderData.emitterAABB.min.z;
	//					}
	//					else
	//					{
	//						model.transforms.translate.z = renderData.emitterAABB.max.z;
	//					}
	//					model.transforms.translate.y = RandomFloat(renderData.emitterAABB.min.y, renderData.emitterAABB.max.y, 3);
	//					model.transforms.translate.x = RandomFloat(renderData.emitterAABB.min.x, renderData.emitterAABB.max.x, 3);
	//				}
	//			}
	//		}
	//		// エミッターが球型だった場合
	//		else
	//		{
	//			// エミッターが内部を指す場合
	//			if (renderData.option.spawnInsideEmitter == true)
	//			{
	//				Vector3 center = renderData.emitterSphere.center;
	//				Vector3 radius = renderData.emitterSphere.radius;
	//
	//				// ランダムな方向（単位ベクトル）を生成
	//				float theta = RandomFloat(0.0f, 2.0f * float(std::numbers::pi), 3);       // 0〜2π
	//				float phi = RandomFloat(0.0f, float(std::numbers::pi), 3);              // 0〜π
	//				float r = RandomFloat(0.0f, 1.0f, 3);            // 0〜1（球内）
	//
	//				// 球内部の距離に合わせてスケーリング（立方根で均等分布）
	//				r = pow(r, 1.0f / 3.0f);
	//
	//				// 球面座標系から直交座標系へ変換
	//				float x = r * sin(phi) * cos(theta) * radius.x;
	//				float y = r * sin(phi) * sin(theta) * radius.y;
	//				float z = r * cos(phi) * radius.z;
	//
	//				model.transforms.translate.x = center.x + x;
	//				model.transforms.translate.y = center.y + y;
	//				model.transforms.translate.z = center.z + z;
	//			}
	//			// エミッターが外殻を指す場合
	//			else
	//			{
	//				Vector3 center = renderData.emitterSphere.center;
	//				Vector3 radius = renderData.emitterSphere.radius;
	// 
	//				// ランダムな方向（単位ベクトル）を生成
	//				float theta = RandomFloat(0.0f, 2.0f * float(std::numbers::pi), 3); // 0〜2π
	//				float phi = RandomFloat(0.0f, float(std::numbers::pi), 3);        // 0〜π
	// 
	//				// r = 1.0f 固定 → 外殻のみ
	//				float x = sin(phi) * cos(theta) * radius.x;
	//				float y = sin(phi) * sin(theta) * radius.y;
	//				float z = cos(phi) * radius.z;
	// 
	//				model.transforms.translate.x = center.x + x;
	//				model.transforms.translate.y = center.y + y;
	//				model.transforms.translate.z = center.z + z;
	//			}
	// 
	//		}
	// 
	//		model.transforms.rotate = renderData.mono.transforms.rotate;
	//		model.transforms.scale = renderData.mono.transforms.scale;
	//		model.model = renderData.mono.model;
	//		model.texture = renderData.mono.texture;
	//		model.color = renderData.mono.color;
	//		model.options = renderData.mono.options;
	// 
	//		// 速度設定
	//		ParticleInf inf;
	//		if (renderData.option.targetDirection == true)
	//		{
	//			inf.velocity = renderData.target - Vector3{ (renderData.emitterAABB.max + renderData.emitterAABB.min) / 2.0f };
	//		}
	//		else
	//		{
	//			inf.velocity = renderData.target - model.transforms.translate;
	//		}
	//		inf.velocity.Normalize();
	//		inf.velocity *= renderData.velocity;
	// 
	//		// 人生設計
	//		inf.liveTime = renderData.liveMax;
	// 
	//		// リストに追加
	//		renderData.GetModelList().push_back(model);
	//		renderData.GetInfList().push_back(inf);
	//	}
	//}
	// 
	// 
	//for (int i = 0; i < renderData.GetModelList().size(); ++i)
	//{
	//	if (renderData.GetInfList()[i].liveTime > 0)
	//	{
	//		// 更新
	//		renderData.GetModelList()[i].transforms.translate += renderData.GetInfList()[i].velocity;
	//		renderData.GetModelList()[i].transforms.rotate += renderData.AddRotate;
	//		renderData.GetModelList()[i].transforms.scale += renderData.AddScale;
	// 
	//		//static float floatColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	//		//ImGui::ColorEdit4("model2.color", floatColor, 1);
	//		//Vector4 vector4Color = { floatColor[0], floatColor[1], floatColor[2], floatColor[3] };
	//		//model1PreColor = ConvertVector4ToUint(vector4Color);
	//		//renderData.GetModelList()[i].color -= renderData.AddColor;
	// 
	//		if (renderData.GetModelList()[i].transforms.scale.x < 0)
	//			renderData.GetModelList()[i].transforms.scale.x = 0.0f;
	//		if (renderData.GetModelList()[i].transforms.scale.y < 0)
	//			renderData.GetModelList()[i].transforms.scale.y = 0.0f;
	//		if (renderData.GetModelList()[i].transforms.scale.z < 0)
	//			renderData.GetModelList()[i].transforms.scale.z = 0.0f;
	// 
	//		// ワールド行列更新
	//		renderData.GetModelList()[i].transforms.World =
	//			Matrix4x4::MakeAffineMatrix(
	//				renderData.GetModelList()[i].transforms.scale,
	//				renderData.GetModelList()[i].transforms.rotate,
	//				renderData.GetModelList()[i].transforms.translate);
	// 
	//		// 人生消費
	//		renderData.GetInfList()[i].liveTime--;
	// 
	//		// 描画
	//		renderData.GetModelList()[i].Draw();
	//	}
	// 
	//	if (
	//		// 生存時間０の時
	//		renderData.GetInfList()[i].liveTime <= 0
	//		// 大きさ０の時
	//		|| (renderData.GetModelList()[i].transforms.scale.x <= 0.0f || renderData.GetModelList()[i].transforms.scale.y <= 0.0f || renderData.GetModelList()[i].transforms.scale.z <= 0.0f)
	//		// アルファ値０の時
	//		//|| renderData.GetModelList()[i].color
	//		)
	//	{
	//		renderData.GetModelList().erase(renderData.GetModelList().begin() + i);
	//		renderData.GetInfList().erase(renderData.GetInfList().begin() + i);
	//	}
	//}
	// 
	//renderData.frame++;
}

bool Engine::InFrustum(const AABB& aabb)
{
	if (!debugCamera) return cameraController->InFrustum(aabb);
	else return debugCameraController->InFrustum(aabb);
}

// 音
void Engine::PlayAudio(const uint32_t& audioId, bool loop)
{
	dxManager->GetResourceManager()->GetAudioManager()->PlayAudio(audioId, loop);
}

void Engine::StopAudio(const uint32_t& audioId)
{
	dxManager->GetResourceManager()->GetAudioManager()->StopAudio(audioId);
}

void Engine::SetAudioVolume(const uint32_t& audioId, float volume)
{
	dxManager->GetResourceManager()->GetAudioManager()->SetVolume(audioId, volume);
}

void Engine::SetMasterVolume(float volume)
{
	dxManager->GetResourceManager()->GetAudioManager()->SetMasterVolume(volume);
}

float Engine::GetVolume(const uint32_t& audioId)
{
	return dxManager->GetResourceManager()->GetAudioManager()->GetVolume(audioId);
}

float Engine::GetMasterVolume()
{
	return dxManager->GetResourceManager()->GetAudioManager()->GetMasterVolume();
}

bool Engine::IsAudioPlaying(const uint32_t& audioId)
{
	return dxManager->GetResourceManager()->GetAudioManager()->IsAudioPlaying(audioId);
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
	return inputManager_->GetMouseController()->wheelDelta;
}

bool Engine::GetMousePress(int i)
{
	return inputManager_->GetMouseController()->GetMousePress(i);
}

bool Engine::GetMousePrePress(int i)
{
	return inputManager_->GetMouseController()->GetMousePrePress(i);
}

// カメラ操作
void Engine::MoveCameraCenter(Vector3 target, int spendFrame, EaseType easetype)
{
	cameraController->SetCenterTarget(target, spendFrame, easetype);
}

void Engine::MoveCameraRotate(Vector3 target, int spendFrame, EaseType easetype)
{
	cameraController->SetRotateTarget(target, spendFrame, easetype);
}

void Engine::MoveCameraDistance(float target, int spendFrame, EaseType easetype)
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

// ウィンドウ操作
void Engine::ToggleFullscreen()
{
	windowManager->ToggleFullscreen();

	// DirectXのリサイズ処理
	dxManager->Resize();

	// 描画システムのリサイズ処理
	//drawSystem->Resize();

	// カメラのアスペクト比を更新
	cameraController->Resize();
	debugCameraController->Resize();
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

// CreateLocalAABBでつくったAABBに座標を適応させる（当たり判定の毎フレーム更新用）
std::vector<AABB>  Engine::CreateAABB(const Transforms& transforms, uint32_t objectNumber)
{
	Matrix4x4 worldMatrix = transforms.World;
	Object3D& obj = dxManager->GetResourceManager()->GetModelManager()->objects[objectNumber];
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
	drawSystem->toggleWireframeMode();
}

void Engine::ToggleCameraMode()
{
	debugCamera = !debugCamera;
}