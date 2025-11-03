#include "Engine.h"
#include "externals/DirectXTex/d3dx12.h"
#include "externals/DirectXTex/DirectXTex.h"
#include "Utilities/Easings.h"
#include "Utilities/functions.h"
#include <cstdint>

#include "input/MouseController.h"
#include "Window/WindowManager.h"
#include "DirectX/DirectXManager.h"
#include "Facade/Game.h"
#include "Resource/Texture/TextureManager.h"
#include "DrawSystem/RenderData/RenderData.h"

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
	if (!cameraManager)
	{
		cameraManager = new CameraManager();
	}
	if (!inputManager_)
	{
		inputManager_ = new Input(windowManager->GetHwnd(), cameraManager);
	}

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
	if (isDebugInfo)
	{
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		//ImGui::DockSpaceOverViewport
	}

	// DirectXを更新
	dxManager->BeginFrame();(ImGui::GetMainViewport());

	// カメラを更新
	UpdateCamera();

	// 描画関数初期化
	drawSystem->BeginFrame(cameraManager->GetCurrentViewProjectionMatrix());

	// デバッグ情報更新
	UpdateDebugInfo();

	// インプット系を更新
	inputManager_->Update();
}
void Engine::UpdateCamera()
{
	// カメラの更新
	cameraManager->Update();

	//// 左シフト＋左クリックでカメラターゲットをオブジェクトに合わせる
	//if (Game::Input::Key::IsPressedNow(DIK_LSHIFT))
	//{
	//	if (Game::Input::Mouse::GetMousePress(0) && !GetMousePrePress(0))
	//	{
	//		for (auto& rd : RenderData_Model::renderModels)
	//		{
	//			if (rd->isCollisionMouseRay == 0)
	//			{
	//				GetDebugCamera()->SetCenterTarget(rd->transforms.translate, 0, EaseType::IN_BACK);
	//			}
	//		}
	//	}
	//}
}
void Engine::UpdateDebugInfo()
{
	if (Game::Input::Key::IsJustPressed(DIK_F1))
	{
		isDebugInfo = !isDebugInfo;
	}
	if (Game::Input::Key::IsJustPressed(DIK_F3))
	{
		ToggleCameraMode();
	}
	if (Game::Input::Key::IsJustPressed(DIK_F12))
	{
		ToggleFullscreen();
	}

	if (isDebugInfo)
	{
		cameraManager->Draw();

		static float fpsSmooth = 60.0f;
		float dt = dxManager->GetDeltaTime();
		float fps = (dt > 0.0f) ? 1.0f / dt : 0.0f;
		// 指数移動平均で平滑化（α=0.1）
		fpsSmooth += (fps - fpsSmooth) * 0.1f;

		ImGui::Begin("------debug info------");
		ImGui::Text("F1  : Hide Debug Info");
		ImGui::Text("F3  : Toggle Camera Mode");
		//ImGui::Text("F12 : Toggle Fullscreen");
		ImGui::Text("DeltaTime: %.3f ms", dxManager->GetDeltaTime() * 1000.0f);
		ImGui::Text("FPS: %.1f ", 1.0f / dxManager->GetDeltaTime());
		ImGui::End();
	}
}
void Engine::EndFrame()
{
	// 
	if (isDebugInfo)ImGui::Render();

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
	std::vector<Object3D> objects = dxManager->GetResourceManager()->GetModelManager()->GetModelList();
	for (auto& rd : modelList)
	{
		//rd->Update(objects);
		rd->Update();
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

			for (const auto& aabb : rd->aabbs)
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
	delete cameraManager;
	cameraManager = nullptr;
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

size_t Engine::GetModelCount()
{
	return dxManager->GetResourceManager()->GetModelManager()->GetModelCount();
}

// 描画
void Engine::DrawModel(RenderData_Model& renderData)
{
	drawSystem->DrawModel(renderData);
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
}

void Engine::AddSphere(Vector3 pos, Vector3 radius, uint32_t color)
{
	if (isDebugInfo)drawSystem->AddSphere(pos, radius, color);
}

void Engine::AddAABB(AABB aabb, uint32_t color)
{
	if (isDebugInfo)drawSystem->AddAABB(aabb, color);
}

bool Engine::InFrustum(const AABB& aabb)
{
	return cameraManager->InCamera(aabb);
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

Vector3 Engine::GetMouseWorldPosition()
{
	return inputManager_->GetMouseController()->GetMouseWorldPosition();
}

Ray Engine::GetMouseRay()
{
	return inputManager_->GetMouseController()->GetMouseRay();
}

uint32_t Engine::GetMouseWheel()
{
	return inputManager_->GetMouseController()->wheelDelta;
}

bool Engine::IsMouseHeld(int i)
{
	return inputManager_->GetMouseController()->IsHeld(i);
}

bool Engine::IsMouseJustPressed(int i)
{
	return inputManager_->GetMouseController()->IsJustPressed(i);
}

bool Engine::IsMouseJustReleased(int i)
{
	return inputManager_->GetMouseController()->IsJustReleased(i);
}

uint32_t Engine::MouseHoldFrames(int i)
{
	return inputManager_->GetMouseController()->HoldFrames(i);
}


bool Engine::IsKeyHeld(BYTE key)
{
	return inputManager_->GetGetHitKey()->IsHeld(key);
}

bool Engine::IsKeyJustPressed(BYTE key)
{
	return inputManager_->GetGetHitKey()->IsJustPressed(key);
}

bool Engine::IsKeyJustReleased(BYTE key)
{
	return inputManager_->GetGetHitKey()->IsJustReleased(key);
}

uint32_t Engine::KeyHoldFrames(BYTE key)
{
	return inputManager_->GetGetHitKey()->HoldFrames(key);
}

int Engine::TestTapLong(int n, BYTE key)
{
	return inputManager_->GetGetHitKey()->TestTapLong(n, key);
}

// カメラ
Vector3 Engine::GetCameraTranslate() const
{
	return cameraManager->GetCurrentTranslate();
}

void Engine::MoveCameraCenter(Vector3 target, int spendFrame, EaseType easetype)
{
	cameraManager->SetCenterTarget(target, spendFrame, easetype);
}

void Engine::MoveCameraRotate(Vector3 target, int spendFrame, EaseType easetype)
{
	cameraManager->SetRotateTarget(target, spendFrame, easetype);
}

void Engine::MoveCameraDistance(float target, int spendFrame, EaseType easetype)
{
	cameraManager->SetDistanceTarget(target, spendFrame, easetype);
}

void Engine::StartCameraShake(float intensity, float duration, float frequency)
{
	cameraManager->StartShake(intensity, duration, frequency);
}

bool Engine::IsCameraShaking()
{
	return cameraManager->IsShaking();
}

void Engine::ToggleCameraMode()
{
	cameraManager->ToggleCameraMode();
}

void Engine::StopCameraShake()
{
	cameraManager->StopShake();
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
	cameraManager->Resize();
}
// CreateLocalAABBでつくったAABBに座標を適応させる（当たり判定の毎フレーム更新用）
std::vector<AABB>  Engine::CreateAABB(const Transforms& transforms, uint32_t objectNumber)
{
	Matrix4x4 worldMatrix = transforms.World;
	Object3D& obj = dxManager->GetResourceManager()->GetModelManager()->GetModelList()[objectNumber];
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