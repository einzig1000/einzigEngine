#include "Camera/CameraManager.h"
#include "Facade/Game.h"
#include "Engine/Engine.h"


// リソース関連
uint32_t Game::Resource::LoadModel(const std::string& directoryPath, const std::string& filename)
{
	return Engine::Instance().LoadModel(directoryPath, filename);
}
uint32_t Game::Resource::LoadTexture(const std::string& filePath)
{
	return Engine::Instance().LoadTexture(filePath);
}
uint32_t Game::Resource::LoadTextureArray(const std::vector<std::string>& filePaths)
{
	return Engine::Instance().LoadTextureArray(filePaths);
}
uint32_t Game::Resource::LoadAudio(const std::string& filePath)
{
	return Engine::Instance().LoadAudio(filePath);
}
TextureData* Game::Resource::GetTextureData(uint32_t textureNumber)
{
	return Engine::Instance().GetTextureData(textureNumber);
}
size_t Game::Resource::GetTextureCount()
{
	return Engine::Instance().GetTextureCount();
}
size_t Game::Resource::GetModelCount()
{
	return Engine::Instance().GetModelCount();
}

// デバッグ描画関連
void Game::DebugDraw::AddSphere(const Sphere& sphere, uint32_t color)
{
	Engine::Instance().AddSphere(sphere, color);
}
void Game::DebugDraw::AddSphereXYZ(const SphereXYZ& sphere, uint32_t color)
{
	Engine::Instance().AddSphereXYZ(sphere, color);
}
void Game::DebugDraw::AddCylinder(const Cylinder& cylinder, uint32_t color)
{
	Engine::Instance().AddCylinder(cylinder, color);
}
void Game::DebugDraw::AddAABB(const AABB& aabb, uint32_t color)
{
	Engine::Instance().AddAABB(aabb, color);
}
void Game::DebugDraw::AddLine(Vector3 start, Vector3 end, uint32_t color)
{
	Engine::Instance().AddLine(start, end, color);
}

// オーディオ関連
void Game::Audio::PlayAudio(const uint32_t& audioId, bool loop)
{
	Engine::Instance().PlayAudio(audioId, loop);
}
void Game::Audio::StopAudio(const uint32_t& audioId)
{
	Engine::Instance().StopAudio(audioId);
}
void Game::Audio::SetAudioVolume(const uint32_t& audioId, float volume)
{
	Engine::Instance().SetAudioVolume(audioId, volume);
}
void Game::Audio::SetMasterVolume(float volume)
{
	Engine::Instance().SetMasterVolume(volume);
}
float Game::Audio::GetVolume(const uint32_t& audioId)
{
	return Engine::Instance().GetVolume(audioId);
}
float Game::Audio::GetMasterVolume()
{
	return Engine::Instance().GetMasterVolume();
}
bool Game::Audio::IsAudioPlaying(const uint32_t& audioId)
{
	return Engine::Instance().IsAudioPlaying(audioId);
}

// ライト関連
void Game::Light::SetLightColor(const Vector4 color)
{
	Engine::Instance().SetLightColor(color);
}
void Game::Light::SetLightDirection(const Vector3 direction)
{
	Engine::Instance().SetLightDirection(direction);
}
void Game::Light::SetLightIntensity(float intensity)
{
	Engine::Instance().SetLightIntensity(intensity);
}
void Game::Light::ToggleLightMode(const LightMode mode)
{
	Engine::Instance().ToggleLightMode(mode);
}

// マウス関連
Vector2 Game::IO::Mouse::GetPosition()
{
	return Engine::Instance().GetMousePosition();
}
Vector2 Game::IO::Mouse::GetPositionDelta()
{
	return Engine::Instance().GetMousePositionDelta();
}
Vector3 Game::IO::Mouse::GetWorldPosition()
{
	return Engine::Instance().GetMouseWorldPosition();
}
Ray Game::IO::Mouse::GetRay()
{
	return Engine::Instance().GetMouseRay();
}
bool Game::IO::Mouse::IsHeld(int i)
{
	return Engine::Instance().IsMouseHeld(i);
}
bool Game::IO::Mouse::IsJustPressed(int i)
{
	return Engine::Instance().IsMouseJustPressed(i);
}
bool Game::IO::Mouse::IsJustReleased(int i)
{
	return Engine::Instance().IsMouseJustReleased(i);
}
uint32_t Game::IO::Mouse::HoldFrames(int i)
{
	return Engine::Instance().MouseHoldFrames(i);
}
int32_t Game::IO::Mouse::GetWheel()
{
	return Engine::Instance().GetMouseWheel();
}
void Game::IO::Mouse::ToggleMouseCursorVisible()
{
	Engine::Instance().ToggleMouseCursorVisible();
}
void Game::IO::Mouse::ShowCursor(bool visible)
{
	Engine::Instance().SetMouseCursorVisible(visible);
}
void Game::IO::Mouse::SetMouseSensitivity(float sensitivity)
{
	Engine::Instance().SetMouseSensitivity(sensitivity);
}

// キー関連
bool Game::IO::Key::IsHeld(BYTE key)
{
	return Engine::Instance().IsKeyHeld(key);
}
bool Game::IO::Key::IsJustPressed(BYTE key)
{
	return Engine::Instance().IsKeyJustPressed(key);
}
bool Game::IO::Key::IsJustReleased(BYTE key)
{
	return Engine::Instance().IsKeyJustReleased(key);
}
uint32_t Game::IO::Key::HoldFrames(BYTE key)
{
	return Engine::Instance().KeyHoldFrames(key);
}
int Game::IO::Key::TestTapLong(int n, BYTE key)
{
	return Engine::Instance().TestTapLong(n, key);
}

// パッド関連
bool Game::IO::Pad::IsHeld(int padIndex, BYTE button)
{
	return Engine::Instance().IsPadHeld(padIndex, button);
}
bool Game::IO::Pad::IsJustPressed(int padIndex, BYTE button)
{
	return Engine::Instance().IsPadJustPressed(padIndex, button);
}
bool Game::IO::Pad::IsJustReleased(int padIndex, BYTE button)
{
	return Engine::Instance().IsPadJustReleased(padIndex, button);
}
uint32_t Game::IO::Pad::HoldFrames(int padIndex, BYTE button)
{
	return Engine::Instance().PadHoldFrames(padIndex, button);
}
Vector2 Game::IO::Pad::GetLeftStick(int padIndex)
{
	return Engine::Instance().GetLeftStick(padIndex);
}
Vector2 Game::IO::Pad::GetRightStick(int padIndex)
{
	return Engine::Instance().GetRightStick(padIndex);
}
float Game::IO::Pad::GetLeftTrigger(int padIndex)
{
	return Engine::Instance().GetLeftTrigger(padIndex);
}
float Game::IO::Pad::GetRightTrigger(int padIndex)
{
	return Engine::Instance().GetRightTrigger(padIndex);
}
void Game::IO::Pad::SetVibration(int padIndex, float leftMotor, float rightMotor)
{
	Engine::Instance().SetPadVibration(padIndex, leftMotor, rightMotor);
}
int32_t Game::IO::Pad::GetConnectedPadNum()
{
	return Engine::Instance().GetConnectedPadNum();
}


// カメラ関連
void Game::Camera::MoveCameraCenter(Vector3 target, int spendFrame, EaseType easetype)
{
	Engine::Instance().MoveCameraCenter(target, spendFrame, easetype);
}
void Game::Camera::MoveCameraRotate(Vector3 target, int spendFrame, EaseType easetype)
{
	Engine::Instance().MoveCameraRotate(target, spendFrame, easetype);
}
void Game::Camera::MoveCameraDistance(float target, int spendFrame, EaseType easetype)
{
	Engine::Instance().MoveCameraDistance(target, spendFrame, easetype);
}
void Game::Camera::StartCameraShake(float intensity, float duration, float frequency)
{
	Engine::Instance().StartCameraShake(intensity, duration, frequency);
}
bool Game::Camera::IsCameraShaking()
{
	return Engine::Instance().IsCameraShaking();
}
void Game::Camera::StopCameraShake()
{
	Engine::Instance().StopCameraShake();
}
bool Game::Camera::InCamera(const AABB& aabb)
{
	return Engine::Instance().InFrustum(aabb);
}
void Game::Camera::SetEnableControl(bool enable)
{
	Engine::Instance().SetEnableCameraControl(enable);
}
void Game::Camera::SetCurrentCamera(const std::string name)
{
}
void Game::Camera::SetCameraMode(CameraMode_ORBIT_FPS mode)
{
	Engine::Instance().GetCameraManager()->SetCameraMode(mode);
}

Vector3 Game::Camera::Getter::GetCenter(const std::string name)
{
	return Engine::Instance().GetCameraManager()->GetCenter(name);
}
Vector3 Game::Camera::Getter::GetTranslate(const std::string name)
{
	return Engine::Instance().GetCameraManager()->GetTranslate(name);
}
Matrix4x4 Game::Camera::Getter::GetViewProjectionMatrix(const std::string name)
{
	return Engine::Instance().GetCameraManager()->GetViewProjectionMatrix(name);
}
float Game::Camera::Getter::GetDistance(const std::string name)
{
	return Engine::Instance().GetCameraManager()->GetDistance(name);
}
Vector3 Game::Camera::Getter::GetCurrentCenter()
{
	return Engine::Instance().GetCameraManager()->GetCurrentCenter();
}
Vector3 Game::Camera::Getter::GetCurrentTranslate()
{
	return Engine::Instance().GetCameraManager()->GetCurrentTranslate();
}
Vector3 Game::Camera::Getter::GetCurrentRotate()
{
	return Engine::Instance().GetCameraManager()->GetCurrentRotate();
}
Matrix4x4 Game::Camera::Getter::GetCurrentViewProjectionMatrix()
{
	return Engine::Instance().GetCameraManager()->GetCurrentViewProjectionMatrix();
}
float Game::Camera::Getter::GetCurrentDistance()
{
	return Engine::Instance().GetCameraManager()->GetCurrentDistance();
}

// その他ユーティリティ
void Game::Utilities::toggleWireframeMode()
{
	Engine::Instance().toggleWireframeMode();
}

// 時間関連
float Game::Time::GetDeltaTime()
{
	return Engine::Instance().GetDeltaTime();
}
uint32_t Game::Time::GetElapsedTime()
{
	return Engine::Instance().GetElapsedTime();
}
float Game::Time::GetFrameRate()
{
	return Engine::Instance().GetFrameRate();
}
void Game::Time::SetTimeScale(float scale)
{
	Engine::Instance().SetTimeScale(scale);
}

// マップ設定
void Game::Physics::AddWorldCollider(IWorldCollider* worldCollider)
{
	Engine::Instance().AddWorldCollider(worldCollider);
}

// マップと衝突する動的オブジェクトの登録
void Game::Physics::RegisterDynamic(IPhysicsBody* b)
{
	Engine::Instance().RegisterDynamic(b);
}

// マップと衝突する動的オブジェクトの登録解除
void Game::Physics::UnregisterDynamic(IPhysicsBody* b)
{
	Engine::Instance().UnregisterDynamic(b);
}

// 全ての動的オブジェクトの登録解除
void Game::Physics::ClearDynamicAll()
{
	Engine::Instance().ClearDynamicAll();
}

//void Game::Physics::SetGravity(Vector3 gravity)
//{
//	Engine::Instance().SetGravity(gravity);
//}
//
//Vector3 Game::Physics::GetGravity()
//{
//	return Engine::Instance().GetGravity();
//}

void Game::quit()
{
	Engine::Instance().Quit();
}

