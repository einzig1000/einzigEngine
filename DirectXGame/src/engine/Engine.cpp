#include "Engine.h"
#include "externals/DirectXTex/d3dx12.h"
#include "externals/DirectXTex/DirectXTex.h"
#include "Utilities/Easings.h"
#include "Utilities/functions.h"
#include <cstdint>

#include "IO/MouseController.h"
#include "Window/WindowManager.h"
#include "DirectX/DirectXManager.h"
#include "Facade/Game.h"
#include "Resource/Texture/TextureManager.h"
#include "DrawSystem/RenderData/RenderData.h"
#include "DrawSystem/DrawSystem.h"
#include "Camera/CameraManager.h"
#include "imGuiManager/ImGuiManager.h"
#include "Physics/PhysicsSystem.h"

#include <DirectXMath.h>
#include <filesystem>
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
	drawSystem_ = std::make_unique<DrawSystem>(dxManager_.get());
	cameraManager_ = std::make_unique<CameraManager>();
	ioManager_ = std::make_unique<IOManager>(windowManager_->GetHwnd(), cameraManager_.get());
	imguiManager_ = std::make_unique<ImGuiManager>();
	imguiManager_->Initialize(dxManager_.get(), windowManager_.get());
	physicsSystem_ = std::make_unique<PhysicsSystem>();


	windowManager_->AttachMouseController(ioManager_->GetMouseController());


	dxManager_->BeginFrame();
	ResourceID::reload();
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
	drawSystem_->SetViewProjectionMatrix(cameraManager_->GetCurrentViewProjectionMatrix());

	// デバッグ情報更新
	UpdateDebugInfo();

	// インプット系を更新
	ioManager_->Update();
}
void Engine::UpdateTransforms()
{
	if (RenderData_Model::renderModels.size() == 0)return;

	if (Game::IO::Key::IsHeld(DIK_RSHIFT))
	{
		if (!Game::IO::Key::IsJustPressed(DIK_RIGHTARROW))
		{
			return;
		}
	}

#pragma region モデルリスト取得

	const auto& modelList = RenderData_Model::renderModels;

#pragma endregion

#pragma region 座標更新 & 描画範囲内判定

	// オブジェクト更新
	std::vector<Object3D> objects = dxManager_->GetResourceManager()->GetModelManager()->GetModelList();

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

	// マウスレイ取得
	Ray mouseRay = ioManager_->GetMouseController()->GetRay();

	// モデルと衝突までの距離セット構造体
	struct HitInfo { RenderData_Model* rdm; float distance; };
	// のリスト
	std::vector<HitInfo> hits;
	// のリサイズ(リサイズではない)
	hits.reserve(modelList.size());

	for (auto& rd : modelList)
	{
		rd->isCollisionMouseRay = -1;
		// 描画範囲内なら判定
		if (rd->inPicture && rd->isCheckMouseRay)
		{
			// 最近接衝突点を取得
			std::optional<Vector3> colPos = IntersectRayModel(
				mouseRay,
				objects[rd->GetModel()].modelData.vertices, rd
			);
			// 衝突していたらリストに登録
			if (colPos)
			{
				float minDistance = (colPos.value() - mouseRay.origin).Length();
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
void Engine::UpdateParticles()
{
	// パーティクル更新
	RenderData_Particle::UpdateAllParticles();
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
	return dxManager_->GetResourceManager()->GetTextureManager()->LoadTexture(filePath);
}
uint32_t Engine::LoadTextureArray(const std::vector<std::string>& filePaths)
{
	return dxManager_->GetResourceManager()->GetTextureManager()->LoadTexture2DArray(filePaths);
}
uint32_t Engine::LoadModel(const std::string& directoryPath, const std::string& filename)
{
	return dxManager_->GetResourceManager()->GetModelManager()->LoadModel(directoryPath, filename);
}
uint32_t Engine::LoadAudio(const std::string& filePath)
{
	return dxManager_->GetResourceManager()->GetAudioManager()->LoadAudio(filePath);
}
Object3D* Engine::GetModelData(uint32_t modelNumber)
{
	return dxManager_->GetResourceManager()->GetModelManager()->GetModelData(modelNumber);
}
TextureData* Engine::GetTextureData(uint32_t textureNumber)
{
	return dxManager_->GetResourceManager()->GetTextureManager()->GetTextureData(textureNumber);
}
size_t Engine::GetTextureCount()
{
	return dxManager_->GetResourceManager()->GetTextureManager()->GetTextureCount();
}
size_t Engine::GetModelCount()
{
	return dxManager_->GetResourceManager()->GetModelManager()->GetModelCount();
}

// 描画
void Engine::AddModelDrawList(RenderData_Model* renderData)
{
	drawSystem_->AddModelDrawList(renderData);
}
void Engine::AddTriangleDrawList(RenderData_Triangle* renderData)
{
	drawSystem_->AddTriangleDrawList(renderData);
}
void Engine::AddRectDrawList(RenderData_Rect* renderData)
{
	drawSystem_->AddRectDrawList(renderData);
}
void Engine::AddSpriteDrawList(RenderData_Sprite* renderData)
{
	drawSystem_->AddSpriteDrawList(renderData);
}
void Engine::AddLineDrawList(RenderData_Line* renderData)
{
	drawSystem_->AddLineDrawList(renderData);
}
void Engine::AddParticleDrawList(RenderData_Particle* renderData)
{
	drawSystem_->AddParticleDrawList(renderData);
}
void Engine::AddBlockDrawList(RenderData_Block* renderData)
{
	drawSystem_->AddBlockDrawList(renderData);
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
	dxManager_->GetResourceManager()->GetAudioManager()->PlayAudio(audioId, loop);
}
void Engine::StopAudio(const uint32_t& audioId)
{
	dxManager_->GetResourceManager()->GetAudioManager()->StopAudio(audioId);
}
void Engine::SetAudioVolume(const uint32_t& audioId, float volume)
{
	dxManager_->GetResourceManager()->GetAudioManager()->SetVolume(audioId, volume);
}
void Engine::SetMasterVolume(float volume)
{
	dxManager_->GetResourceManager()->GetAudioManager()->SetMasterVolume(volume);
}
float Engine::GetVolume(const uint32_t& audioId)
{
	return dxManager_->GetResourceManager()->GetAudioManager()->GetVolume(audioId);
}
float Engine::GetMasterVolume()
{
	return dxManager_->GetResourceManager()->GetAudioManager()->GetMasterVolume();
}
bool Engine::IsAudioPlaying(const uint32_t& audioId)
{
	return dxManager_->GetResourceManager()->GetAudioManager()->IsAudioPlaying(audioId);
}

// ライト
void Engine::SetLightDirection(const Vector3 direction)
{
	drawSystem_->SetLightDirection(direction);
}
void Engine::SetLightColor(const Vector4 color)
{
	drawSystem_->SetLightColor(color);
}
void Engine::SetLightIntensity(float intensity)
{
	drawSystem_->SetLightIntensity(intensity);
}
void Engine::ToggleLightMode(const LightMode mode)
{
	drawSystem_->ToggleLightMode(mode);
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

	// alt-tab / ウィンドウドラッグ等で巨大dtが出るのを防ぐ
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
std::vector<AABB>  Engine::CreateAABB(RenderData_Model* data)
{
	if (data->GetModel() < 0 || data->GetModel() >= (int)dxManager_->GetResourceManager()->GetModelManager()->GetModelCount())
	{
		return {};
	}
	Matrix4x4 worldMatrix = data->GetWorldMatrix();
	Object3D& obj = dxManager_->GetResourceManager()->GetModelManager()->GetModelList()[data->GetModel()];
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
	drawSystem_->toggleWireframeMode();
}


Microsoft::WRL::ComPtr<ID3D12Resource> Engine::CreateBufferResource(size_t sizeInBytes)
{
	// ID3D12Resourceを格納するポインタ
	Microsoft::WRL::ComPtr<ID3D12Resource> pResource = nullptr;

	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;

	// リソース記述子を作成
	D3D12_RESOURCE_DESC resourceDesc{};
	// バッファリソース。テクスチャの場合はまた別の設定をする
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Width = sizeInBytes;
	// バッファの場合はこれらは１にする決まり
	resourceDesc.Height = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.SampleDesc.Count = 1;
	// バッファの場合はこれにする決まり
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// リソースを作成
	HRESULT hr = dxManager_->GetDevice()->CreateCommittedResource(
		&heapProperties,        // ヒープのプロパティ
		D3D12_HEAP_FLAG_NONE,   // ヒープフラグ
		&resourceDesc,          // リソースの記述子
		D3D12_RESOURCE_STATE_GENERIC_READ,           // 初期状態
		nullptr,                // Clear値 (バッファの場合はnullptr)
		IID_PPV_ARGS(&pResource) // ID3D12Resourceポインタを取得
	);


	if (FAILED(hr) || !pResource)
	{
		const HRESULT removed = dxManager_->GetDevice()->GetDeviceRemovedReason();
		Log("CreateCommittedResource failed. size=%zu hr=0x%08X removed=0x%08X",
			sizeInBytes,
			static_cast<unsigned>(hr),
			static_cast<unsigned>(removed));
		return nullptr;
	}

	pResource->SetName(L"CreateBufferResource()");

	return pResource;
}

Microsoft::WRL::ComPtr<ID3D12Resource> Engine::CreateConstantBufferResource(size_t sizeInBytes)
{
	size_t ConstantSize;
	ConstantSize = (sizeInBytes + (D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT - 1)) & ~(D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT - 1);

	return CreateBufferResource(ConstantSize);
}

const std::vector<Object3D> Engine::GetAllObject3D()
{
	return dxManager_->GetResourceManager()->GetModelManager()->GetModelList();
}
