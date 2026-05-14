#include "Engine.h"
#include "Utilities/functions.h"
#include <cstdint>

#include <IO/IOManager.h>
#include <Window/WindowManager.h>
#include <DirectX/DirectXManager.h>
#include <Facade/Game.h>
#include <ResourceManager/ResourceManager.h>
#include <DrawSystem/DrawSystem.h>
#include <Camera/CameraManager.h>
#include <imGuiManager/ImGuiManager.h>
#include <FixFPS/FixFPS.h>
#include <Physics/PhysicsSystem.h>

using namespace DirectX;


Engine& Engine::Instance()
{
	static Engine instance; 
	return instance;
}

// 初期化用
void Engine::Initialize(int width, int height, const std::wstring& title)
{
	// COM の初期化
	HRESULT hr = CoInitializeEx(0, COINIT_MULTITHREADED);
	assert(SUCCEEDED(hr));
	// 例外ハンドラの設定
	SetUnhandledExceptionFilter(ExportDump);

	windowManager_ = std::make_unique<WindowManager>(width, height, title);
	dxManager_ = std::make_unique<DirectXManager>(windowManager_->GetHwnd());
	resourceManager_ = std::make_unique<ResourceManager>(dxManager_->GetCommandContextManager()->GetCommandList(), dxManager_->GetDescriptorHeapManager(), dxManager_->GetDevice());
	drawSystem_ = std::make_unique<DrawSystem>(dxManager_.get(), resourceManager_.get());
	cameraManager_ = std::make_unique<CameraManager>();
	ioManager_ = std::make_unique<IOManager>(windowManager_->GetHwnd(), cameraManager_.get());
	imguiManager_ = std::make_unique<ImGuiManager>();
	imguiManager_->Initialize(dxManager_.get(), windowManager_.get());
	physicsSystem_ = std::make_unique<PhysicsSystem>();
	fixFPS_ = std::make_unique<FixFPS>();

	windowManager_->AttachMouseController(ioManager_->GetMouseController());
}

// メインループ用
bool Engine::ProcessMessage()
{
	MSG msg = {};
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
		{
			return false;
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return true;
}
void Engine::BeginFrame()
{
	// GPU同期
	dxManager_->GetSynchronizationManager()->WaitForGPU();

	if (Game::IO::Key::IsJustPressed(DIK_F12))
	{
		ToggleFullscreen();
	}

	// DirectXを更新
	dxManager_->BeginFrame();

	// imguiを更新
	imguiManager_->BeginFrame();

	// カメラを更新	
	UpdateCamera();

	// 描画関数初期化
	drawSystem_->Update();

	// デバッグ情報更新
	UpdateDebugInfo();

	// インプット系を更新
	ioManager_->Update();
}
void Engine::UpdateTransforms()
{
	//if (RenderData_Model::renderModels.size() == 0)return;

	//if (Game::IO::Key::IsHeld(DIK_RSHIFT))
	//{
	//	if (!Game::IO::Key::IsJustPressed(DIK_RIGHTARROW))
	//	{
	//		return;
	//	}
	//}

#pragma region モデルリスト取得

	//const auto& modelList = RenderData_Model::renderModels;

#pragma endregion

#pragma region 座標更新 & 描画範囲内判定

	// オブジェクト更新
	std::vector<ModelData> objects = resourceManager_->GetModelManager()->GetModelList();

	//for (auto& rd : modelList)
	//{
	//	//rd->Update1();
	//}
	//for (auto& rd : modelList)
	//{
	//	//rd->Update2();
	//}
	//for (auto& rd : modelList)
	//{
	//	// 座標更新,衝突ペア・深度の保存
	//	//rd->Update3();
	//}
	//for (auto& rd : modelList)
	//{
	//	// 衝突時の更新、それに伴う座標修正など
	//	//rd->Update4();
	//}
	//for (auto& rd : modelList)
	//{
	//	//rd->Update5();
	//}

#pragma endregion

#pragma region マウスレイ衝突判定

	//// マウスレイ取得
	//Ray mouseRay = ioManager_->GetMouseController()->GetRay();

	//// モデルと衝突までの距離セット構造体
	//struct HitInfo { RenderData_Model* rdm; float distance; };
	//// のリスト
	//std::vector<HitInfo> hits;
	//// のリサイズ(リサイズではない)
	//hits.reserve(modelList.size());

	//for (auto& rd : modelList)
	//{
	//	rd->isCollisionMouseRay = -1;
	//	// 描画範囲内なら判定
	//	if (rd->inPicture && rd->isCheckMouseRay)
	//	{
	//		// 最近接衝突点を取得
	//		std::optional<Vector3> colPos = IntersectRayModel(
	//			mouseRay,
	//			objects[rd->GetModel()].modelData.vertices, rd
	//		);
	//		// 衝突していたらリストに登録
	//		if (colPos)
	//		{
	//			float minDistance = (colPos.value() - mouseRay.origin).Length();
	//			hits.push_back({ rd, minDistance });
	//		}
	//	}
	//}

	//// 距離の昇順でソート
	//std::sort(hits.begin(), hits.end(),
	//	[](auto& a, auto& b) { return a.distance < b.distance; });

	//// ソート後に順序を割り当て
	//for (int order = 0; order < (int)hits.size(); ++order)
	//{
	//	hits[order].rdm->isCollisionMouseRay = order;
	//}

#pragma endregion

}
void Engine::UpdateParticles()
{
	// パーティクル更新
	//RenderData_Particle::UpdateAllParticles();
}
void Engine::UpdateCamera()
{
	// カメラの更新
	cameraManager_->Update();

	//// 左シフト＋左クリックでカメラターゲットをオブジェクトに合わせる
	//if (Game::IO::Key::IsHeld(DIK_LSHIFT))
	//{
	//	if (Game::IO::Mouse::IsJustPressed(0))
	//	{
	//		for (auto& rd : RenderData_Model::renderModels)
	//		{
	//			if (rd->isCollisionMouseRay == 0)
	//			{
	//				cameraManager_->SetCenterTarget(rd->GetWorldPosition(), 0, EaseType::IN_BACK);
	//			}
	//		}
	//	}
	//}
}
void Engine::UpdateDebugInfo()
{
	if (Game::IO::Key::IsJustPressed(DIK_F1))
	{
		isDebugInfo_ = !isDebugInfo_;
	}
	if (Game::IO::Key::IsJustPressed(DIK_F3))
	{
		cameraManager_->ToggleCamera();
	}

	if (isDebugInfo_)
	{
		cameraManager_->Draw();

		ImGui::Begin("------debug info------");
		ImGui::Text("ESC : Quit Application");
		ImGui::Text("F1  : Hide Debug Info");
		ImGui::Text("F3  : Toggle Camera Release or Debug");
		ImGui::Text("F5  : Toggle Camera FirstPerson or ThirdPerson");
		ImGui::Text("F12 : Toggle Fullscreen");
		ImGui::Text("DeltaTime: %.3f ms", fixFPS_->GetDeltaTime() * 1000.0f);
		ImGui::Text("FPS: %.1f ", fixFPS_->GetAverageFPS());
		ImGui::Text("ImGui FPS: %.1f ", ImGui::GetIO().Framerate);
		ImGui::End();
	}
}
void Engine::EndFrame()
{
	// 入力終了処理
	ioManager_->EndFrame();

	// 物理更新
	//physicsSystem_->Step();

	/// 座標更新
	//UpdateTransforms();

	/// パーティクル更新
	//UpdateParticles();

	// 描画実行
	drawSystem_->Draw();

	// ImGui描画
	imguiManager_->EndFrame();
	if (isDebugInfo_)imguiManager_->Draw();

	// DirectX終了処理
	dxManager_->EndFrame();

	// FPS制限
	fixFPS_->UpdateFixFPS();
}
void Engine::Quit()
{
	windowManager_->Quit();
}

// 終了処理
void Engine::Finalize()
{
	// ImGuiの終了処理
	imguiManager_->Finalize();

	// フレームレート制御
	fixFPS_.reset();
	// 物理演算
	physicsSystem_.reset();
	// ImGui
	imguiManager_.reset();
	// 入力関連
	ioManager_.reset();
	// カメラ
	cameraManager_.reset();
	// 描画関連
	drawSystem_.reset();
	// リソース管理
	resourceManager_.reset();
	// DirectX関連
	dxManager_.reset();
	// ウィンドウ関連
	windowManager_.reset();

	// COMの終了処理
	CoUninitialize();
}

// ウィンドウ操作
void Engine::ToggleFullscreen()
{
	windowManager_->ToggleFullscreen();

	// DirectXのリサイズ処理
	dxManager_->Resize();

	// カメラのアスペクト比を更新
	cameraManager_->Resize();
}

// CreateLocalAABBでつくったAABBに座標を適応させる（当たり判定の毎フレーム更新用）
//std::vector<AABB>  Engine::CreateAABB(RenderData_Model* data)
//{
//	//if (data->GetModel() < 0 || data->GetModel() >= (int)resourceManager_->GetModelManager()->GetModelCount())
//	//{
//	//	return {};
//	//}
//	//Matrix4x4 worldMatrix = data->GetWorldMatrix();
//	//ModelData& obj = resourceManager_->GetModelManager()->GetModelList()[data->GetModel()];
//	std::vector<AABB> result;
//
//	//for (const auto& localAABB : obj.aabb)
//	//{
//	//	// ローカルAABBの8頂点
//	//	Vector3 corners[8] = {
//	//		{localAABB.min.x, localAABB.min.y, localAABB.min.z},
//	//		{localAABB.max.x, localAABB.min.y, localAABB.min.z},
//	//		{localAABB.min.x, localAABB.max.y, localAABB.min.z},
//	//		{localAABB.max.x, localAABB.max.y, localAABB.min.z},
//	//		{localAABB.min.x, localAABB.min.y, localAABB.max.z},
//	//		{localAABB.max.x, localAABB.min.y, localAABB.max.z},
//	//		{localAABB.min.x, localAABB.max.y, localAABB.max.z},
//	//		{localAABB.max.x, localAABB.max.y, localAABB.max.z},
//	//	};
//
//	//	// 8頂点をワールド空間に変換
//	//	Vector3 worldMin = Transform(corners[0], worldMatrix);
//	//	Vector3 worldMax = worldMin;
//	//	for (int i = 1; i < 8; ++i)
//	//	{
//	//		Vector3 v = Transform(corners[i], worldMatrix);
//	//		worldMin.x = my_min(worldMin.x, v.x);
//	//		worldMin.y = my_min(worldMin.y, v.y);
//	//		worldMin.z = my_min(worldMin.z, v.z);
//	//		worldMax.x = my_max(worldMax.x, v.x);
//	//		worldMax.y = my_max(worldMax.y, v.y);
//	//		worldMax.z = my_max(worldMax.z, v.z);
//	//	}
//	//	result.push_back({ worldMin, worldMax });
//	//}
//	return result;
//}
