#include "Facade/Game.h"
#include "Engine/Engine.h"
#include "Camera/CameraController.h"
using namespace DirectX;

uint32_t Game::Resource::LoadModel(const std::string& directoryPath, const std::string& filename)
{
	return Engine::Instance().LoadModel(directoryPath, filename);
}

uint32_t Game::Resource::LoadTexture(const std::string& filePath)
{
	return Engine::Instance().LoadTexture(filePath);
}

uint32_t Game::Resource::LoadAudio(const std::string& filePath)
{
	return Engine::Instance().LoadAudio(filePath);
}

TextureData* Game::Resource::GetTexture(uint32_t textureNumber)
{
	return Engine::Instance().GetTexture(textureNumber);
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

void Game::Light::ToggleLightMode(const uint32_t mode)
{
	Engine::Instance().ToggleLightMode(mode);
}


Vector2 Game::Input::Mouse::GetMousePosition()
{
	return Engine::Instance().GetMousePosition();
}

Ray Game::Input::Mouse::GetMouseRay()
{
	return Engine::Instance().GetMouseRay();
}

bool Game::Input::Mouse::GetMousePress(int i)
{
	return Engine::Instance().GetMousePress(i);
}

bool Game::Input::Mouse::GetMousePrePress(int i)
{
	return Engine::Instance().GetMousePrePress(i);
}

uint32_t Game::Input::Mouse::GetMouseWheel()
{
	return Engine::Instance().GetMouseWheel();
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

void Game::Camera::SetControlModeCamera(bool mode)
{
	Engine::Instance().SetControlModeCamera(mode);
}

CameraController* Game::Camera::GetCamera()
{
	return Engine::Instance().GetCamera();
}

CameraController* Game::Camera::GetDebugCamera()
{
	return Engine::Instance().GetDebugCamera();
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



void Game::Utilitie::toggleWireframeMode()
{
	Engine::Instance().toggleWireframeMode();
}

