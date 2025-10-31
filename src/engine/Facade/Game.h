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

class Game
{
public:

	class Resource
	{
	public:
		static uint32_t LoadModel(const std::string& directoryPath, const std::string& filename);
		static uint32_t LoadTexture(const std::string& filePath);
		static uint32_t LoadAudio(const std::string& filePath);
		static TextureData* GetTexture(uint32_t textureNumber);
		static size_t GetTextureCount();
		static size_t GetModelCount();
	};

	class DebugDraw
	{
	public:
		static void AddSphere(Vector3 pos, Vector3 radius, uint32_t color);
		static void AddAABB(AABB aabb, uint32_t color);
	};

	class Audio
	{
	public:
		static void PlayAudio(const uint32_t& audioId, bool loop);
		static void StopAudio(const uint32_t& audioId);
		static void SetAudioVolume(const uint32_t& audioId, float volume);
		static void SetMasterVolume(float volume);
		static float GetVolume(const uint32_t& audioId);
		static float GetMasterVolume();
		static bool IsAudioPlaying(const uint32_t& audioId);
	};

	class Light
	{
	public:
		// ライト
		static void SetLightColor(const Vector4 color);
		static void SetLightDirection(const Vector3 direction);
		static void ToggleLightMode(const uint32_t mode);
		static void SetLightIntensity(float intensity);
	};

	class Input
	{
	public:
		class Mouse
		{
		public:
			// マウス
			static Vector2 GetMousePosition();
			static Ray GetMouseRay();
			// 0 = 左クリック  1 = 右クリック  2 = ミドルボタン
			static bool GetMousePress(int i);
			// 0 = 左クリック  1 = 右クリック  2 = ミドルボタン
			static bool GetMousePrePress(int i);
			static uint32_t GetMouseWheel();
		};

		class Key
		{

		};

		class Pad
		{

		};
	};

	class Camera
	{
	public:
		static void MoveCameraCenter(Vector3 target, int spendFrame, EaseType easetype);
		static void MoveCameraRotate(Vector3 target, int spendFrame, EaseType easetype);
		static void MoveCameraDistance(float target, int spendFrame, EaseType easetype);
		static void SetControlModeCamera(bool mode);
		static CameraController* GetCamera();
		static CameraController* GetDebugCamera();
		static void StartCameraShake(float intensity, float duration, float frequency = 25.0f);
		static bool IsCameraShaking();
		static void StopCameraShake();
		// 描画オブジェクトは画面内か
		static bool InCamera(const AABB& aabb);
	};

	// EasingとかRandとかもいれるべきか？
	class Utilitie
	{
		// プリミティブモードの設定
		static void toggleWireframeMode();

	};



private:
	Game() = delete;
	~Game() = delete;
	Game(const Game&) = delete;
	Game& operator=(const Game&) = delete;
	Game(Game&&) = delete;
	Game& operator=(Game&&) = delete;
};