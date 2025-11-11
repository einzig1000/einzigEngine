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

class Game
{
public:

	class Resource
	{
	public:

		/// <summary>
		/// 
		/// </summary>
		/// <param name="directoryPath"></param>
		/// <param name="filename"></param>
		/// <returns></returns>
		static uint32_t LoadModel(const std::string& directoryPath, const std::string& filename);
		static uint32_t LoadTexture(const std::string& filePath);
		static uint32_t LoadAudio(const std::string& filePath);

		/// <summary>
		/// テクスチャデータ取得
		/// 
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
		static void ToggleLightMode(const LightMode mode);
		static void SetLightIntensity(float intensity);
	};

	class Input
	{
	public:
		class Mouse
		{
		public:
			/// <summary>
			/// マウスのスクリーン座標位置取得
			/// </summary>
			/// <returns></returns>
			static Vector2 GetMousePosition();

			/// <summary>
			/// マウスのワールド座標位置取得
			/// </summary>
			/// <returns></returns>
			static Vector3 GetMouseWorldPosition();

			/// <summary>
			/// マウスのワールド座標レイ取得
			/// </summary>
			/// <returns></returns>
			static Ray GetMouseRay();

			/// <summary>
			/// マウスボタンの入力取得(現在押されているか)
			/// </summary>
			/// <param name="i">0 = 左クリック  1 = 右クリック  2 = ミドルボタン</param>
			/// <returns></returns>
			static bool IsHeld(int i);

			/// <summary>
			/// マウスボタンの入力取得(押した瞬間)
			/// </summary>
			/// <param name="i">0 = 左クリック  1 = 右クリック  2 = ミドルボタン</param>
			/// <returns></returns>
			static bool IsJustPressed(int i);

			/// <summary>
			/// マウスボタンの入力取得(離した瞬間)
			/// </summary>
			/// <param name="i">0 = 左クリック  1 = 右クリック  2 = ミドルボタン</param>
			/// <returns></returns>
			static bool IsJustReleased(int i);

			/// <summary>
			/// マウスボタンの入力取得(押されてからの経過フレーム数)
			/// </summary>
			/// <param name="i">0 = 左クリック  1 = 右クリック  2 = ミドルボタン</param>
			/// <returns></returns>
			static uint32_t HoldFrames(int i);

			/// <summary>
			/// マウスホイールの回転量取得
			/// </summary>
			static uint32_t GetMouseWheel();

			// カーソルの表示・非表示切り替え
			static void ToggleMouseCursorVisible();
		};

		class Key
		{
		public:

			/// <summary>
			/// キーボードの入力取得(現在押されているか)
			/// </summary>
			static bool IsHeld(BYTE key);

			/// <summary>
			/// キーボードの入力取得(押した瞬間)
			/// </summary>
			static bool IsJustPressed(BYTE key);

			/// <summary>
			/// キーボードの入力取得(離した瞬間)
			/// </summary>
			static bool IsJustReleased(BYTE key);

			/// <summary>
			/// キーボードの入力取得(押されてからの経過フレーム数)
			/// </summary>
			static uint32_t HoldFrames(BYTE key);

			/// <summary>
			/// キーの単押し・長押し判定
			/// </summary>
			/// <param name="n">nフレーム以上で長押しと判定</param>
			/// <param name="key">キー</param>
			/// <returns>0: なし  1:単押し  2:長押し</returns>
			static int TestTapLong(int n, BYTE key);
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

	class Math
	{
	public:
		/// <summary>
		/// イージングfloat版
		/// </summary>
		static float Easing(float start, float end, float t, EaseType type)
		{
			return Easings::EasingFloat(start, end, type, t);
		}

		/// <summary>
		/// イージングVector3版
		/// </summary>
		static Vector3 Easing(Vector3 start, Vector3 end, float t, EaseType type)
		{
			return Easings::EasingVector3(start, end, type, t);
		}

		static float Lerp(float start, float end, float t)
		{
			return start + (end - start) * t;
		}
	
		static float RandFloat(float min, float max, int decimalPlaces)
		{
			return RandomFloat(min, max, decimalPlaces);
		}

		static int RandInt(int min, int max)
		{
			return RandomInt(min, max);
		}

		/// <summary>
		/// 度数法を弧度法に変換
		/// </summary>
		static float DegreeToRadian(float degree)
		{
			return degree * (std::numbers::pi_v<float> / 180.0f);
		}

		/// <summary>
		/// 弧度法を度数法に変換
		/// </summary>
		static float RadianToDegree(float radian)
		{
			return radian * (180.0f / std::numbers::pi_v<float>);
		}

		static Vector4 UintToVector4(uint32_t color)
		{
			return ConvertUintToVector4(color);
		}

		static uint32_t Vector4ToUint(Vector4 color)
		{
			return ConvertVector4ToUint(color);
		}
	};


private:
	Game() = delete;
	~Game() = delete;
	Game(const Game&) = delete;
	Game& operator=(const Game&) = delete;
	Game(Game&&) = delete;
	Game& operator=(Game&&) = delete;
};