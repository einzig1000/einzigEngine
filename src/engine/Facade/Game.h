#pragma once
#include <numbers>

#include "definition/definition.h"
#include "input/Input.h"
#include "Utilities/Easings.h"
#include "Utilities/functions.h"
#include "Utilities/JsonManager.h"
#include "DrawSystem/RenderData/RenderData.h"
#include "ResourceLoder/ResourceID.h"
#include "ImGuiManager/ImGuiManager.h"
#include <algorithm>

class IWorldCollider;


class Game
{
public:

	class Resource
	{
	public:

		/// <summary>
		/// モデル読み込み
		/// </summary>
		/// <param name="directoryPath">例:"Resources/Prototypes/model/"</param>
		/// <param name="filename">"cube.obj"</param>
		/// <returns>モデルID</returns>
		static uint32_t LoadModel(const std::string& directoryPath, const std::string& filename);

		/// <summary>
		/// テクスチャ読み込み
		/// </summary>
		/// <param name="filePath">例:"Resources/Prototypes/texture/uvChecker.png"</param>
		/// <returns>テクスチャID</returns>
		static uint32_t LoadTexture(const std::string& filePath);

		/// <summary>
		/// テクスチャ配列読み込み
		/// </summary>
		/// <param name="filePaths">例:{"Resources/Prototypes/texture/uvChecker1.png","Resources/Prototypes/texture/uvChecker2.png"}</param>
		/// <returns>テクスチャID</returns>
		static uint32_t LoadTextureArray(const std::vector<std::string>& filePaths);

		/// <summary>
		/// オーディオ読み個み
		/// </summary>
		/// <param name="filePath">例:"Resources/Prototypes/audio/BGM/InGame.mp3"</param>
		/// <returns></returns>
		static uint32_t LoadAudio(const std::string& filePath);

		/// <summary>
		/// テクスチャデータ取得
		/// </summary>
		/// <param name="textureNumber">テクスチャID</param>
		/// <returns>メタデータを含むテクスチャデータ</returns>
		static TextureData* GetTextureData(uint32_t textureNumber);

		/// <summary>
		/// 読み込んだテクスチャ数取得
		/// </summary>
		/// <returns>読み込んだテクスチャ数</returns>
		static size_t GetTextureCount();

		/// <summary>
		/// 読み込んだモデル数取得
		/// </summary>
		/// <returns>読み込んだモデル数</returns>
		static size_t GetModelCount();
	};

	class DebugDraw
	{
	public:

		/// <summary>
		/// ワイヤーフレーム球描画
		/// </summary>
		/// <param name="pos">ワールド座標</param>
		/// <param name="radius">半径</param>
		/// <param name="color">色</param>
		static void AddSphere(Vector3 pos, Vector3 radius, uint32_t color);

		/// <summary>
		/// ワイヤーフレームAABB描画
		/// </summary>
		/// <param name="aabb">AABB</param>
		/// <param name="color">色</param>
		static void AddAABB(AABB aabb, uint32_t color);

		/// <summary>
		/// ライン描画
		/// </summary>
		/// <param name="start">ワールド始点</param>
		/// <param name="end">ワールド終点</param>
		/// <param name="color">色</param>
		static void AddLine(Vector3 start, Vector3 end, uint32_t color);
	};

	class Audio
	{
	public:

		/// <summary>
		/// オーディオ再生
		/// </summary>
		/// <param name="audioId">Resource::LoadAudioで取得したオーディオID</param>
		/// <param name="loop">trueなら自動ループ</param>
		static void PlayAudio(const uint32_t& audioId, bool loop);

		/// <summary>
		/// オーディオ停止
		/// </summary>
		/// <param name="audioId">Resource::LoadAudioで取得したオーディオID</param>
		static void StopAudio(const uint32_t& audioId);

		/// <summary>
		/// オーディオボリューム設定
		/// </summary>
		/// <param name="audioId"Resource::LoadAudioで取得したオーディオID></param>
		/// <param name="volume">0.0f～1.0f</param>
		static void SetAudioVolume(const uint32_t& audioId, float volume);

		/// <summary>
		/// マスターボリューム設定
		/// </summary>
		/// <param name="volume">0.0f～1.0f</param>
		static void SetMasterVolume(float volume);

		/// <summary>
		/// オーディオボリューム取得
		/// </summary>
		/// <param name="audioId">Resource::LoadAudioで取得したオーディオID</param>
		/// <returns>現在の音量</returns>
		static float GetVolume(const uint32_t& audioId);

		/// <summary>
		/// マスターボリューム取得
		/// </summary>
		/// <returns>現在のマスター音量</returns>
		static float GetMasterVolume();

		/// <summary>
		/// オーディオが再生中か？
		/// </summary>
		/// <param name="audioId">Resource::LoadAudioで取得したオーディオID</param>
		/// <returns>bool </returns>
		static bool IsAudioPlaying(const uint32_t& audioId);
	};

	class Light
	{
	public:
		/// <summary>
		/// 共通ライトカラー設定
		/// </summary>
		/// <param name="color">Vector4のcolor</param>
		static void SetLightColor(const Vector4 color);

		/// <summary>
		/// 共通ライト方向設定
		/// </summary>
		/// <param name="direction">Vector3の方向ベクトル</param>
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
			/// <returns>マウスのスクリーン座標</returns>
			static Vector2 GetPosition();

			/// <summary>
			/// マウスのスクリーン座標位置変化量取得
			/// </summary>
			/// <returns>マウスのスクリーン座標位置変化量</returns>
			static Vector2 GetPositionDelta();

			/// <summary>
			/// マウスのワールド座標位置取得
			/// </summary>
			/// <returns>マウスのワールド座標</returns>
			static Vector3 GetWorldPosition();

			/// <summary>
			/// マウスのワールド座標レイ取得
			/// </summary>
			/// <returns></returns>
			static Ray GetRay();

			/// <summary>
			/// マウスホイールの回転量取得
			/// </summary>
			/// <returns>マウスホイールの回転量</returns>
			static int32_t GetWheel();

			/// <summary>
			/// マウスボタンの入力取得(現在押されているか)
			/// </summary>
			/// <param name="i">0 = 左クリック  1 = 右クリック  2 = ミドルボタン</param>
			/// <returns>現在押されているか</returns>
			static bool IsHeld(int i);

			/// <summary>
			/// マウスボタンの入力取得(押した瞬間)
			/// </summary>
			/// <param name="i">0 = 左クリック  1 = 右クリック  2 = ミドルボタン</param>
			/// <returns>押した瞬間か</returns>
			static bool IsJustPressed(int i);

			/// <summary>
			/// マウスボタンの入力取得(離した瞬間)
			/// </summary>
			/// <param name="i">0 = 左クリック  1 = 右クリック  2 = ミドルボタン</param>
			/// <returns>離した瞬間か</returns>
			static bool IsJustReleased(int i);

			/// <summary>
			/// マウスボタンの入力取得(押されてからの経過フレーム数)
			/// </summary>
			/// <param name="i">0 = 左クリック  1 = 右クリック  2 = ミドルボタン</param>
			/// <returns>押されてからの経過フレーム数</returns>
			static uint32_t HoldFrames(int i);

			// カーソルの表示・非表示切り替え
			static void ToggleMouseCursorVisible();

			// カーソルの表示・非表示設定
			static void ShowCursor(bool visible);

			// マウス感度設定
			static void SetMouseSensitivity(float sensitivity);
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
			static Vector3 GetCurrentRotate();
			static Matrix4x4 GetCurrentViewProjectionMatrix(); // ビュープロジェクション行列
			static float GetCurrentDistance();			// カメラ距離
		};

		static void SetCameraMode(CameraMode_ORBIT_FPS mode);

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

		/// <summary>
		/// カメラコントロールの有効無効設定
		/// </summary>
		static void SetEnableControl(bool enable);

		/// <summary>
		/// カメラ切り替え
		/// </summary>
		static void SetCurrentCamera(const std::string name);
	};

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

		static Vector3 DirectionFromYawPitch(float yaw, float pitch)
		{
			float sp = std::sinf(pitch);
			float cp = std::cosf(pitch);
			float sy = std::sinf(yaw);
			float cy = std::cosf(yaw);

			Vector3 dir;
			dir.x = sy * cp;
			dir.y = -sp;
			dir.z = cy * cp;
			dir.Normalize();
			return dir;
		}

		static Vector3 YawPitchFromDirection(const Vector3& dir)
		{
			Vector3 normDir = dir;
			normDir.Normalize();
			float pitch = std::asinf(-normDir.y); // -sin(pitch) = y 成分
			float yaw = std::atan2f(normDir.x, normDir.z); // sin(yaw) = x 成分, cos(yaw) = z 成分
			return Vector3(pitch, yaw, 0.0f); // roll はここでは未使用
		}
	};

	class Time
	{
	public:
		/// <summary>
		/// デルタタイム取得
		/// </summary>
		/// <returns>デルタタイム</returns>
		static float GetDeltaTime();

		/// <summary>
		/// 起動からの経過時間取得
		/// </summary>
		static uint32_t GetElapsedTime();

		/// <summary>
		/// フレームレート取得
		/// </summary>
		static float GetFrameRate();

		/// <summary>
		/// タイムスケール設定　タイムスケールとは時間の進み具合を調整する値。1.0が通常速度、0.5が半分の速度、2.0が2倍の速度になる。
		/// </summary>
		static void SetTimeScale(float scale);
	};

	class Physics
	{
	public:
		/// <summary>
		/// WorldColliderの設定
		/// </summary>
		static void SetIWorldCollider(IWorldCollider* worldCollider);

		/// <summary>
		/// RenderData_Modelの物理演算有効化
		/// </summary>
		static void RegisterDynamic(RenderData_Model* model);

		/// <summary>
		/// RenderData_Modelの物理演算無効化
		/// </summary>
		static void UnregisterDynamic(RenderData_Model* model);
		
		/// <summary>
		/// 登録されている全てのRenderData_Modelの物理演算無効化
		/// </summary>
		static void ClearDynamicAll();
	};

private:
	Game() = delete;
	~Game() = delete;
	Game(const Game&) = delete;
	Game& operator=(const Game&) = delete;
	Game(Game&&) = delete;
	Game& operator=(Game&&) = delete;
};