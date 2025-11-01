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

		class Getter
		{
		public:

			static Vector3 GetCenter(const std::string name);			// カメラ回転中心
			static Vector3 GetTranslate(const std::string name);			// カメラ位置
			static Matrix4x4 GetViewProjectionMatrix(const std::string name); // ビュープロジェクション行列
			static float GetDistance(const std::string name);			// カメラ距離

			static Vector3 GetCurrentCenter();			// カメラ回転中心
			static Vector3 GetCurrentTranslate();		// カメラ位置
			static Matrix4x4 GetCurrentViewProjectionMatrix(); // ビュープロジェクション行列
			static float GetCurrentDistance();			// カメラ距離
		};

		/// <summary>
		/// カメラの回転中心座標の移動
		/// </summary>
		/// <param name="target">目標座標</param>
		/// <param name="spendFrame">移動にかけるフレーム数</param>
		/// <param name="easetype">移動補完イージングタイプ</param>
		static void MoveCameraCenter(Vector3 target, int spendFrame, EaseType easetype);

		/// <summary>
		///	カメラの回転量変更
		/// </summary>
		/// <param name="target">目標回転量</param>
		/// <param name="spendFrame">変更にかけるフレーム数</param>
		/// <param name="easetype">変更補完イージングタイプ</param>
		static void MoveCameraRotate(Vector3 target, int spendFrame, EaseType easetype);

		/// <summary>
		///  カメラの回転中心からの距離(ズーム量)変更
		/// </summary>
		/// <param name="target">目標ズーム量</param>
		/// <param name="spendFrame">変更にかけるフレーム数</param>
		/// <param name="easetype">変更補完イージングタイプ</param>
		static void MoveCameraDistance(float target, int spendFrame, EaseType easetype);


		/// <summary>
		/// カメラシェイク開始関数
		/// </summary>
		/// <param name="intensity">シェイクの最大振幅</param>
		/// <param name="duration">シェイクの継続時間</param>
		/// <param name="frequency">振動の速度 既定値は25.0f</param>
		static void StartCameraShake(float intensity, float duration, float frequency = 25.0f);

		/// <summary>
		/// 今シェイクしているか
		/// </summary>
		/// <returns>今シェイクしているか</returns>
		static bool IsCameraShaking();

		/// <summary>
		/// シェイク停止関数
		/// </summary>
		static void StopCameraShake();


		/// <summary>
		/// 描画範囲内にAABBがあるか
		/// </summary>
		/// <param name="aabb">検索対象のAABB</param>
		/// <returns>結果</returns>
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