#include "Camera/CameraManager.h"
#include "Facade/Game.h"
#include "Engine/Engine.h"

using namespace DirectX;

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


void Game::DebugDraw::AddSphere(Vector3 pos, Vector3 radius, uint32_t color)
{
	Engine::Instance().AddSphere(pos, radius, color);
}

void Game::DebugDraw::AddAABB(AABB aabb, uint32_t color)
{
	Engine::Instance().AddAABB(aabb, color);
}

void Game::DebugDraw::AddLine(Vector3 start, Vector3 end, uint32_t color)
{
	Engine::Instance().AddLine(start, end, color);
}

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


Vector2 Game::Input::Mouse::GetMousePosition()
{
	return Engine::Instance().GetMousePosition();
}

Vector2 Game::Input::Mouse::GetMousePositionDelta()
{
	return Engine::Instance().GetMousePositionDelta();
}

Vector3 Game::Input::Mouse::GetMouseWorldPosition()
{
	return Engine::Instance().GetMouseWorldPosition();
}

Ray Game::Input::Mouse::GetMouseRay()
{
	return Engine::Instance().GetMouseRay();
}

bool Game::Input::Mouse::IsHeld(int i)
{
	return Engine::Instance().IsMouseHeld(i);
}

bool Game::Input::Mouse::IsJustPressed(int i)
{
	return Engine::Instance().IsMouseJustPressed(i);
}

bool Game::Input::Mouse::IsJustReleased(int i)
{
	return Engine::Instance().IsMouseJustReleased(i);
}

uint32_t Game::Input::Mouse::HoldFrames(int i)
{
	return Engine::Instance().MouseHoldFrames(i);
}

int32_t Game::Input::Mouse::GetMouseWheel()
{
	return Engine::Instance().GetMouseWheel();
}

void Game::Input::Mouse::ToggleMouseCursorVisible()
{
	Engine::Instance().ToggleMouseCursorVisible();
}

void Game::Input::Mouse::ShowCursor(bool visible)
{
	Engine::Instance().SetMouseCursorVisible(visible);
}

void Game::Input::Mouse::SetMouseSensitivity(float sensitivity)
{
	Engine::Instance().SetMouseSensitivity(sensitivity);
}

bool Game::Input::Key::IsHeld(BYTE key)
{
	return Engine::Instance().IsKeyHeld(key);
}

bool Game::Input::Key::IsJustPressed(BYTE key)
{
	return Engine::Instance().IsKeyJustPressed(key);
}

bool Game::Input::Key::IsJustReleased(BYTE key)
{
	return Engine::Instance().IsKeyJustReleased(key);
}

uint32_t Game::Input::Key::HoldFrames(BYTE key)
{
	return Engine::Instance().KeyHoldFrames(key);
}

int Game::Input::Key::TestTapLong(int n, BYTE key)
{
	return Engine::Instance().TestTapLong(n, key);
}


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

void Game::Camera::SetCameraMode(CameraMode_ORBIT_FPS mode)
{
	Engine::Instance().GetCameraManager()->SetCameraMode(mode);
}


void Game::Utilitie::toggleWireframeMode()
{
	Engine::Instance().toggleWireframeMode();
}


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
