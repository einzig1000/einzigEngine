#include "Camera/CameraManager.h"
#include "Facade/Game.h"
#include "Utilities/Easing/Easing.h"
#include <Utilities/Converter/ColorConverter/ColorConverter.h>
#include <Utilities/Converter/CoordinateConverter/CoordinateConverter.h>
#include <Utilities/Converter/AngleConverter/AngleConverter.h>
#include <Utilities/Random/Random.h>
#include "Engine.h"


namespace Game
{
	namespace Resource
	{
		uint32_t LoadModel(const std::string& directoryPath, const std::string& filename)
		{
			return Engine::Instance().LoadModel(directoryPath, filename);
		}
		uint32_t LoadTexture(const std::string& filePath)
		{
			return Engine::Instance().LoadTexture(filePath);
		}
		uint32_t LoadAudio(const std::string& filePath)
		{
			return Engine::Instance().LoadAudio(filePath);
		}
		TextureData* GetTextureData(uint32_t textureNumber)
		{
			return Engine::Instance().GetTextureData(textureNumber);
		}
		size_t GetTextureCount()
		{
			return Engine::Instance().GetTextureCount();
		}
		size_t GetModelCount()
		{
			return Engine::Instance().GetModelCount();
		}
	}

	namespace DebugDraw
	{
		void AddSphere(const Sphere& sphere, uint32_t color)
		{
			Engine::Instance().AddSphere(sphere, color);
		}
		void AddSphereXYZ(const SphereXYZ& sphere, uint32_t color)
		{
			Engine::Instance().AddSphereXYZ(sphere, color);
		}
		void AddCylinder(const Cylinder& cylinder, uint32_t color)
		{
			Engine::Instance().AddCylinder(cylinder, color);
		}
		void AddAABB(const AABB& aabb, uint32_t color)
		{
			Engine::Instance().AddAABB(aabb, color);
		}
		void AddLine(Vector3 start, Vector3 end, uint32_t color)
		{
			Engine::Instance().AddLine(start, end, color);
		}
	}

	namespace Audio
	{
		void PlayAudio(const uint32_t& audioId, bool loop)
		{
			Engine::Instance().PlayAudio(audioId, loop);
		}
		void StopAudio(const uint32_t& audioId)
		{
			Engine::Instance().StopAudio(audioId);
		}
		void SetAudioVolume(const uint32_t& audioId, float volume)
		{
			Engine::Instance().SetAudioVolume(audioId, volume);
		}
		void SetMasterVolume(float volume)
		{
			Engine::Instance().SetMasterVolume(volume);
		}
		float GetVolume(const uint32_t& audioId)
		{
			return Engine::Instance().GetVolume(audioId);
		}
		float GetMasterVolume()
		{
			return Engine::Instance().GetMasterVolume();
		}
		bool IsAudioPlaying(const uint32_t& audioId)
		{
			return Engine::Instance().IsAudioPlaying(audioId);
		}
	}

	namespace Light
	{
		void SetLightColor(const Vector4 color)
		{
			Engine::Instance().SetLightColor(color);
		}
		void SetLightDirection(const Vector3 direction)
		{
			Engine::Instance().SetLightDirection(direction);
		}
		void SetLightIntensity(float intensity)
		{
			Engine::Instance().SetLightIntensity(intensity);
		}
		void ToggleLightMode(const LightMode mode)
		{
			Engine::Instance().ToggleLightMode(mode);
		}
	}

	namespace IO
	{
		namespace Mouse
		{
			// マウス関連
			Vector2 GetPosition()
			{
				return Engine::Instance().GetMousePosition();
			}
			Vector2 GetPositionDelta()
			{
				return Engine::Instance().GetMousePositionDelta();
			}
			Vector3 GetWorldPosition()
			{
				return Engine::Instance().GetMouseWorldPosition();
			}
			Ray GetRay()
			{
				return Engine::Instance().GetMouseRay();
			}
			bool IsHeld(int i)
			{
				return Engine::Instance().IsMouseHeld(i);
			}
			bool IsJustPressed(int i)
			{
				return Engine::Instance().IsMouseJustPressed(i);
			}
			bool IsJustReleased(int i)
			{
				return Engine::Instance().IsMouseJustReleased(i);
			}
			uint32_t HoldFrames(int i)
			{
				return Engine::Instance().MouseHoldFrames(i);
			}
			int32_t GetWheel()
			{
				return Engine::Instance().GetMouseWheel();
			}
			void ToggleMouseCursorVisible()
			{
				Engine::Instance().ToggleMouseCursorVisible();
			}
			void ShowCursor(bool visible)
			{
				Engine::Instance().SetMouseCursorVisible(visible);
			}
			void SetMouseSensitivity(float sensitivity)
			{
				Engine::Instance().SetMouseSensitivity(sensitivity);
			}
		}

		namespace Key
		{
			// キー関連
			bool IsHeld(BYTE key)
			{
				return Engine::Instance().IsKeyHeld(key);
			}
			bool IsJustPressed(BYTE key)
			{
				return Engine::Instance().IsKeyJustPressed(key);
			}
			bool IsJustReleased(BYTE key)
			{
				return Engine::Instance().IsKeyJustReleased(key);
			}
			uint32_t HoldFrames(BYTE key)
			{
				return Engine::Instance().KeyHoldFrames(key);
			}
			int TestTapLong(int n, BYTE key)
			{
				return Engine::Instance().TestTapLong(n, key);
			}
		}

		namespace Pad
		{
			bool IsHeld(int padIndex, BYTE button)
			{
				return Engine::Instance().IsPadHeld(padIndex, button);
			}
			bool IsJustPressed(int padIndex, BYTE button)
			{
				return Engine::Instance().IsPadJustPressed(padIndex, button);
			}
			bool IsJustReleased(int padIndex, BYTE button)
			{
				return Engine::Instance().IsPadJustReleased(padIndex, button);
			}
			uint32_t HoldFrames(int padIndex, BYTE button)
			{
				return Engine::Instance().PadHoldFrames(padIndex, button);
			}
			Vector2 GetLeftStick(int padIndex)
			{
				return Engine::Instance().GetLeftStick(padIndex);
			}
			Vector2 GetRightStick(int padIndex)
			{
				return Engine::Instance().GetRightStick(padIndex);
			}
			float GetLeftTrigger(int padIndex)
			{
				return Engine::Instance().GetLeftTrigger(padIndex);
			}
			float GetRightTrigger(int padIndex)
			{
				return Engine::Instance().GetRightTrigger(padIndex);
			}
			void SetVibration(int padIndex, float leftMotor, float rightMotor)
			{
				Engine::Instance().SetPadVibration(padIndex, leftMotor, rightMotor);
			}
			int32_t GetConnectedPadNum()
			{
				return Engine::Instance().GetConnectedPadNum();
			}

		}
	}

	namespace Camera
	{
		void MoveCameraCenter(Vector3 target, int spendFrame, EaseType easetype)
		{
			Engine::Instance().MoveCameraCenter(target, spendFrame, easetype);
		}
		void MoveCameraRotate(Vector3 target, int spendFrame, EaseType easetype)
		{
			Engine::Instance().MoveCameraRotate(target, spendFrame, easetype);
		}
		void MoveCameraDistance(float target, int spendFrame, EaseType easetype)
		{
			Engine::Instance().MoveCameraDistance(target, spendFrame, easetype);
		}
		void StartCameraShake(float intensity, float duration, float frequency)
		{
			Engine::Instance().StartCameraShake(intensity, duration, frequency);
		}
		bool IsCameraShaking()
		{
			return Engine::Instance().IsCameraShaking();
		}
		void StopCameraShake()
		{
			Engine::Instance().StopCameraShake();
		}
		bool InCamera(const AABB& aabb)
		{
			return Engine::Instance().InFrustum(aabb);
		}
		void SetEnableControl(bool enable)
		{
			Engine::Instance().SetEnableCameraControl(enable);
		}
		void SetCurrentCamera(const std::string name)
		{

		}
		void SetCameraMode(CameraMode_ORBIT_FPS mode)
		{
			Engine::Instance().GetCameraManager()->SetCameraMode(mode);
		}

		namespace Getter
		{
			Vector3 GetCenter(const std::string name)
			{
				return Engine::Instance().GetCameraManager()->GetCenter(name);
			}
			Vector3 GetTranslate(const std::string name)
			{
				return Engine::Instance().GetCameraManager()->GetTranslate(name);
			}
			Matrix4x4 GetViewProjectionMatrix(const std::string name)
			{
				return Engine::Instance().GetCameraManager()->GetViewProjectionMatrix(name);
			}
			float GetDistance(const std::string name)
			{
				return Engine::Instance().GetCameraManager()->GetDistance(name);
			}
			Vector3 GetCurrentCenter()
			{
				return Engine::Instance().GetCameraManager()->GetCurrentCenter();
			}
			Vector3 GetCurrentTranslate()
			{
				return Engine::Instance().GetCameraManager()->GetCurrentTranslate();
			}
			Vector3 GetCurrentRotate()
			{
				return Engine::Instance().GetCameraManager()->GetCurrentRotate();
			}
			Matrix4x4 GetCurrentViewProjectionMatrix()
			{
				return Engine::Instance().GetCameraManager()->GetCurrentViewProjectionMatrix();
			}
			float GetCurrentDistance()
			{
				return Engine::Instance().GetCameraManager()->GetCurrentDistance();
			}
		}
	}

	namespace Utilities
	{
		void ToggleWireframeMode()
		{
			Engine::Instance().toggleWireframeMode();
		}
	}

	namespace Math
	{
		float EasingFloat(float start, float end, EaseType easeType, float t)
		{
			return Easing::EasingFloat(start, end, easeType, t);
		}
		Vector3 EasingVector3(Vector3 start, Vector3 end, EaseType easeType, float t)
		{
			return Easing::EasingVector3(start, end, easeType, t);
		}

		Vector4 UintToVector4(uint32_t color)
		{
			return ColorConverter::ConvertUintToVector4(color);
		}
		uint32_t Vector4ToUint(Vector4 color)
		{
			return ColorConverter::ConvertVector4ToUint(color);
		}

		float DegreeToRadian(float degree)
		{
			return AngleConverter::ToRadian(degree);
		}
		float RadianToDegree(float radian)
		{
			return AngleConverter::ToDegree(radian);
		}

		float RandFloat(float min, float max, int decimalPlaces)
		{
			return Random::RandomFloat(min, max, decimalPlaces);
		}
		int RandInt(int min, int max)
		{
			return Random::RandomInt(min, max);
		}

		Vector3 DirectionFromYawPitch(float yaw, float pitch)
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
		Vector3 YawPitchFromDirection(const Vector3& dir)
		{
			Vector3 normDir = dir;
			normDir.Normalize();
			float pitch = std::asinf(-normDir.y); // -sin(pitch) = y 成分
			float yaw = std::atan2f(normDir.x, normDir.z); // sin(yaw) = x 成分, cos(yaw) = z 成分
			return Vector3(pitch, yaw, 0.0f); // roll はここでは未使用
		}
	}

	namespace Time
	{
		// 時間関連
		float GetDeltaTime()
		{
			return Engine::Instance().GetDeltaTime();
		}
		uint32_t GetElapsedTime()
		{
			return Engine::Instance().GetElapsedTime();
		}
		float GetFrameRate()
		{
			return Engine::Instance().GetFrameRate();
		}
		void SetTimeScale(float scale)
		{
			Engine::Instance().SetTimeScale(scale);
		}
	}

	namespace Physics
	{
		// マップ設定
		void AddWorldCollider(IWorldCollider* worldCollider)
		{
			Engine::Instance().AddWorldCollider(worldCollider);
		}

		// マップと衝突する動的オブジェクトの登録
		void RegisterDynamic(IPhysicsBody* b)
		{
			Engine::Instance().RegisterDynamic(b);
		}

		// マップと衝突する動的オブジェクトの登録解除
		void UnregisterDynamic(IPhysicsBody* b)
		{
			Engine::Instance().UnregisterDynamic(b);
		}

		// 全ての動的オブジェクトの登録解除
		void ClearDynamicAll()
		{
			Engine::Instance().ClearDynamicAll();
		}

		//void SetGravity(Vector3 gravity)
		//{
		//	Engine::Instance().SetGravity(gravity);
		//}
		//
		//Vector3 GetGravity()
		//{
		//	return Engine::Instance().GetGravity();
		//}
	}

	void Game::quit()
	{
		Engine::Instance().Quit();
	}

}