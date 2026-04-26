#include "Engine.h"
#include "Utilities/functions.h"
#include <cstdint>

#include <IO/MouseController.h>
#include <IO/IOManager.h>
#include <Window/WindowManager.h>
#include <DirectX/DirectXManager.h>
#include <Facade/Game.h>
#include <ResourceManager/ResourceManager.h>
#include <DrawSystem/DrawSystem.h>
#include <Camera/CameraManager.h>
#include <imGuiManager/ImGuiManager.h>
#include <Physics/PhysicsSystem.h>
#include <DrawSystem/RenderData/RenderObject.h>

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


	windowManager_->AttachMouseController(ioManager_->GetMouseController());


	dxManager_->BeginFrame();
	Game::Resource::LoadTexture("resources/Prototypes/texture/uvChecker.png");
	//ResourceID::reload();
	dxManager_->EndFrame();
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
	//drawSystem_->SetViewProjectionMatrix(cameraManager_->GetCurrentViewProjectionMatrix());

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
		ToggleCamera();
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
		ImGui::Text("DeltaTime: %.3f ms", dxManager_->GetFixFPS()->GetDeltaTime() * 1000.0f);
		ImGui::Text("FPS: %.1f ", dxManager_->GetFixFPS()->GetAverageFPS());
		ImGui::Text("ImGui FPS: %.1f ", ImGui::GetIO().Framerate);
		ImGui::End();
	}
}
void Engine::EndFrame()
{
	// 入力終了処理
	ioManager_->EndFrame();

	// 物理更新
	physicsSystem_->Step();

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

	// COMの終了処理
	CoUninitialize();
}

// リソース読み込み
uint32_t Engine::LoadTexture(const std::string& filePath)
{
	return resourceManager_->GetTextureManager()->LoadTexture(filePath);
}
uint32_t Engine::LoadModel(const std::string& filename)
{
	return resourceManager_->GetModelManager()->LoadModel(filename);
}
uint32_t Engine::LoadAudio(const std::string& filePath)
{
	return resourceManager_->GetAudioManager()->LoadAudio(filePath);
}

ModelData* Engine::GetModelData(uint32_t modelID)
{
	return resourceManager_->GetModelManager()->GetModelData(modelID);
}
TextureData* Engine::GetTextureData(uint32_t textureID)
{
	return resourceManager_->GetTextureManager()->GetTextureData(textureID);
}
AudioData* Engine::GetAudioData(uint32_t audioID)
{
	return resourceManager_->GetAudioManager()->GetAudioData(audioID);
}

size_t Engine::GetTextureCount()
{
	return resourceManager_->GetTextureManager()->GetTextureCount();
}
size_t Engine::GetModelCount()
{
	return resourceManager_->GetModelManager()->GetModelCount();
}
size_t Engine::GetAudioCount()
{
	return resourceManager_->GetAudioManager()->GetAudioCount();
}

// 描画
void Engine::AddDrawList(const RenderObject* renderObject)
{
	drawSystem_->AddDrawList(renderObject);
}

void Engine::AddSphere(const Sphere& sphere, uint32_t color)
{
	drawSystem_->AddSphere(sphere, color);
}
void Engine::AddSphereXYZ(const SphereXYZ& sphere, uint32_t color)
{
	drawSystem_->AddSphereXYZ(sphere, color);
}
void Engine::AddCylinder(const Cylinder& cylinder, uint32_t color)
{
	drawSystem_->AddCylinder(cylinder, color);
}
void Engine::AddAABB(const AABB& aabb, uint32_t color)
{
	drawSystem_->AddAABB(aabb, color);
}
void Engine::AddLine(Vector3 start, Vector3 end, uint32_t color)
{
	drawSystem_->AddDebugLineList(start, end, color);
}

bool Engine::InFrustum(const AABB& aabb)
{
	return cameraManager_->InCamera(aabb);
}

// 音
void Engine::PlayAudio(const uint32_t& audioId, bool loop)
{
	resourceManager_->GetAudioManager()->PlayAudio(audioId, loop);
}
void Engine::StopAudio(const uint32_t& audioId)
{
	resourceManager_->GetAudioManager()->StopAudio(audioId);
}
void Engine::SetAudioVolume(const uint32_t& audioId, float volume)
{
	resourceManager_->GetAudioManager()->SetVolume(audioId, volume);
}
void Engine::SetMasterVolume(float volume)
{
	resourceManager_->GetAudioManager()->SetMasterVolume(volume);
}
float Engine::GetVolume(const uint32_t& audioId)
{
	return resourceManager_->GetAudioManager()->GetVolume(audioId);
}
float Engine::GetMasterVolume()
{
	return resourceManager_->GetAudioManager()->GetMasterVolume();
}
bool Engine::IsAudioPlaying(const uint32_t& audioId)
{
	return resourceManager_->GetAudioManager()->IsAudioPlaying(audioId);
}

// ライト
void Engine::SetLightDirection(const Vector3 direction)
{
	//drawSystem_->SetLightDirection(direction);
}
void Engine::SetLightColor(const Vector4 color)
{
	//drawSystem_->SetLightColor(color);
}
void Engine::SetLightIntensity(float intensity)
{
	//drawSystem_->SetLightIntensity(intensity);
}
void Engine::ToggleLightMode(const LightMode mode)
{
	//drawSystem_->ToggleLightMode(mode);
}

// マウス
Vector2 Engine::GetMousePosition()
{
	return ioManager_->GetMouseController()->GetPosition();
}
Vector2 Engine::GetMousePositionDelta()
{
	return ioManager_->GetMouseController()->GetRawDelta();
}
Vector3 Engine::GetMouseWorldPosition()
{
	return ioManager_->GetMouseController()->GetWorldPosition();
}
Ray Engine::GetMouseRay()
{
	return ioManager_->GetMouseController()->GetRay();
}
int32_t Engine::GetMouseWheel()
{
	return ioManager_->GetMouseController()->GetWheelDelta();
}
bool Engine::IsMouseHeld(int i)
{
	return ioManager_->GetMouseController()->IsHeld(i);
}
bool Engine::IsMouseJustPressed(int i)
{
	return ioManager_->GetMouseController()->IsJustPressed(i);
}
bool Engine::IsMouseJustReleased(int i)
{
	return ioManager_->GetMouseController()->IsJustReleased(i);
}
uint32_t Engine::MouseHoldFrames(int i)
{
	return ioManager_->GetMouseController()->HoldFrames(i);
}
void Engine::ToggleMouseCursorVisible()
{
	ioManager_->GetMouseController()->ToggleMouseCursorVisible();
}
void Engine::SetMouseCursorVisible(bool visible)
{
	ioManager_->GetMouseController()->ShowCursor(visible);
}
void Engine::SetMouseSensitivity(float sensitivity)
{
	ioManager_->GetMouseController()->SetSensitivity(sensitivity);
}

// キーボード
bool Engine::IsKeyHeld(BYTE key)
{
	return ioManager_->GetGetHitKey()->IsHeld(key);
}
bool Engine::IsKeyJustPressed(BYTE key)
{
	return ioManager_->GetGetHitKey()->IsJustPressed(key);
}
bool Engine::IsKeyJustReleased(BYTE key)
{
	return ioManager_->GetGetHitKey()->IsJustReleased(key);
}
uint32_t Engine::KeyHoldFrames(BYTE key)
{
	return ioManager_->GetGetHitKey()->HoldFrames(key);
}
int Engine::TestTapLong(int n, BYTE key)
{
	return ioManager_->GetGetHitKey()->TestTapLong(n, key);
}

// ゲームパッド
bool Engine::IsPadHeld(int padIndex, BYTE button)
{
	return ioManager_->GetGetPadState()->IsHeld(padIndex, button);
}
bool Engine::IsPadJustPressed(int padIndex, BYTE button)
{
	return ioManager_->GetGetPadState()->IsJustPressed(padIndex, button);
}
bool Engine::IsPadJustReleased(int padIndex, BYTE button)
{
	return ioManager_->GetGetPadState()->IsJustReleased(padIndex, button);
}
uint32_t Engine::PadHoldFrames(int padIndex, BYTE button)
{
	return ioManager_->GetGetPadState()->HoldFrames(padIndex, button);
}
Vector2 Engine::GetLeftStick(int padIndex)
{
	return ioManager_->GetGetPadState()->GetLeftStick(padIndex);
}
Vector2 Engine::GetRightStick(int padIndex)
{
	return ioManager_->GetGetPadState()->GetRightStick(padIndex);
}
float Engine::GetLeftTrigger(int padIndex)
{
	return ioManager_->GetGetPadState()->GetLeftTrigger(padIndex);
}
float Engine::GetRightTrigger(int padIndex)
{
	return ioManager_->GetGetPadState()->GetRightTrigger(padIndex);
}
void Engine::SetPadVibration(int padIndex, float leftMotor, float rightMotor)
{
	ioManager_->GetGetPadState()->SetVibration(padIndex, leftMotor, rightMotor);
}
int32_t Engine::GetConnectedPadNum()
{
	return ioManager_->GetGetPadState()->GetConnectedPadNum();
}

// カメラ
Vector3 Engine::GetCameraTranslate() const
{
	return cameraManager_->GetCurrentTranslate();
}
void Engine::MoveCameraCenter(Vector3 target, int spendFrame, EaseType easetype)
{
	cameraManager_->SetCenterTarget(target, spendFrame, easetype);
}
void Engine::MoveCameraRotate(Vector3 target, int spendFrame, EaseType easetype)
{
	cameraManager_->SetRotateTarget(target, spendFrame, easetype);
}
void Engine::MoveCameraDistance(float target, int spendFrame, EaseType easetype)
{
	cameraManager_->SetDistanceTarget(target, spendFrame, easetype);
}
void Engine::StartCameraShake(float intensity, float duration, float frequency)
{
	cameraManager_->StartShake(intensity, duration, frequency);
}
bool Engine::IsCameraShaking()
{
	return cameraManager_->IsShaking();
}
void Engine::SetCameraMode(CameraMode_ORBIT_FPS mode)
{
	cameraManager_->SetCameraMode(mode); 
}
void Engine::ToggleCamera()
{
	cameraManager_->ToggleCamera();
}
void Engine::StopCameraShake()
{
	cameraManager_->StopShake();
}
void Engine::SetEnableCameraControl(bool enable)
{
	cameraManager_->SetEnableControl(enable);
}


// 時間制御
float Engine::GetDeltaTime()
{
	float dt = dxManager_->GetFixFPS()->GetDeltaTime();

	// ブレークポイントで止めたときなどに、くそでかデルタタイムが出るのを防止
	constexpr float kMaxDt = 0.1f; // 100ms
	if (dt < 0.0f) dt = 0.0f;
	if (dt > kMaxDt) dt = kMaxDt;

	return dt;
}
uint32_t Engine::GetElapsedTime()
{
	return dxManager_->GetFixFPS()->GetFrameCount();
}
float Engine::GetFrameRate()
{
	return dxManager_->GetFixFPS()->GetAverageFPS();
}
void Engine::SetTimeScale(float scale)
{
	dxManager_->GetFixFPS()->SetTimeScale(scale);
}


// 物理制御
void Engine::AddWorldCollider(IWorldCollider* worldCollider)
{
	physicsSystem_->AddWorldCollider(worldCollider);
}

void Engine::RegisterDynamic(IPhysicsBody* b)
{
	physicsSystem_->RegisterDynamic(b);
}

void Engine::UnregisterDynamic(IPhysicsBody* b)
{
	physicsSystem_->UnregisterDynamic(b);
}
void Engine::ClearDynamicAll()
{
	physicsSystem_->ClearDynamics();
}

// ウィンドウ操作
void Engine::ToggleFullscreen()
{
	windowManager_->ToggleFullscreen();

	// DirectXのリサイズ処理
	dxManager_->Resize();

	// 描画システムのリサイズ処理
	//drawSystem_->Resize();

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
