#pragma once
#include <numbers>

#include "definition/definition.h"
#include "input/Input.h"
#include "Utilities/Easings.h"
#include "Utilities/functions.h"
#include "DrawSystem/RenderData/RenderData.h"

#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"
#include <algorithm>


class CameraController;
class Engine;

class Game
{
public:
	// 初期化
	static void Initialize(int width, int height, const std::wstring& title);

	// メインループ用
	static bool ProcessMessage();
	static void BeginFrame();
	static void UpdateTransforms();
	static void EndFrame();

	// 終了処理
	static void Finalize();

	// リソース読み込み
	static uint32_t LoadModel(const std::string& directoryPath, const std::string& filename);
	static uint32_t LoadTexture(const std::string& filePath);
	static uint32_t LoadAudio(const std::string& filePath);
	static TextureData* GetTexture(uint32_t textureNumber);
	static size_t GetTextureCount();
	static size_t GetModelCount();

	// 描画
	static void DrawModel(RenderData_Model& renderData);
	static void DrawSprite(RenderData_Sprite& renderData);
	static void DrawTriangle(RenderData_Triangle& renderData);
	static void DrawLine(RenderData_Line& renderData);
	static void DrawParticle(RenderData_Particle& renderData);

	static void AddSphere(Vector3 pos, Vector3 radius, uint32_t color);
	static void AddAABB(AABB aabb, uint32_t color);

	// 音
	static void PlayAudio(const uint32_t& audioId, bool loop);
	static void StopAudio(const uint32_t& audioId);
	static void SetAudioVolume(const uint32_t& audioId, float volume);
	static void SetMasterVolume(float volume);
	static float GetVolume(const uint32_t& audioId);
	static float GetMasterVolume();
	static bool IsAudioPlaying(const uint32_t& audioId);

	// ライト
	static void SetLightColor(const Vector4 color);
	static void SetLightDirection(const Vector3 direction);
	static void ToggleLightMode(const uint32_t mode);
	static void SetLightIntensity(float intensity);

	// マウス
	static Vector2 GetMousePosition();
	static Ray GetMouseRay();
	// 0 = 左クリック  1 = 右クリック  2 = ミドルボタン
	static bool GetMousePress(int i);
	// 0 = 左クリック  1 = 右クリック  2 = ミドルボタン
	static bool GetMousePrePress(int i);
	static uint32_t GetMouseWheel();

	// カメラ
	static void MoveCameraCenter(Vector3 target, int spendFrame, EaseType easetype);
	static void MoveCameraRotate(Vector3 target, int spendFrame, EaseType easetype);
	static void MoveCameraDistance(float target, int spendFrame, EaseType easetype);
	static void SetControlModeCamera(bool mode);
	static CameraController* GetCamera();
	static CameraController* GetDebugCamera();
	static void StartCameraShake(float intensity, float duration, float frequency = 25.0f);
	static bool IsCameraShaking();
	static void StopCameraShake();

	// AABBの作成
	static std::vector<AABB> CreateAABB(const Transforms& transforms, uint32_t objectNumber);

	// プリミティブモードの設定
	static void toggleWireframeMode();

	// 描画オブジェクトは画面内か
	static bool InFrustum(const AABB& aabb);

private:
	Game() = delete;
	~Game() = delete;
	Game(const Game&) = delete;
	Game& operator=(const Game&) = delete;
	Game(Game&&) = delete;
	Game& operator=(Game&&) = delete;
};