#include "Facade/Game.h"
#include "Engine/Engine.h"
#include "Camera/CameraController.h"
using namespace DirectX;

void Game::Initialize(int width, int height, const std::wstring& title)
{
	Engine::Instance().Initialize(width, height, title);
}

bool Game::ProcessMessage()
{
	return Engine::Instance().ProcessMessage();
}

void Game::BeginFrame()
{
	Engine::Instance().BeginFrame();
}

void Game::UpdateTransforms()
{
	Engine::Instance().UpdateTransforms();
}

void Game::EndFrame()
{
	Engine::Instance().EndFrame();
}

void Game::Finalize()
{
	Engine::Instance().Finalize();
}

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


void Game::MoveCameraCenter(Vector3 target, int spendFrame, EaseType easetype)
{
	Engine::Instance().MoveCameraCenter(target, spendFrame, easetype);
}

void Game::MoveCameraRotate(Vector3 target, int spendFrame, EaseType easetype)
{
	Engine::Instance().MoveCameraRotate(target, spendFrame, easetype);
}

void Game::MoveCameraDistance(float target, int spendFrame, EaseType easetype)
{
	Engine::Instance().MoveCameraDistance(target, spendFrame, easetype);
}

void Game::SetControlModeCamera(bool mode)
{
	Engine::Instance().SetControlModeCamera(mode);
}

CameraController* Game::GetCamera()
{
	return Engine::Instance().GetCamera();
}

CameraController* Game::GetDebugCamera()
{
	return Engine::Instance().GetDebugCamera();
}

void Game::StartCameraShake(float intensity, float duration, float frequency)
{
	Engine::Instance().StartCameraShake(intensity, duration, frequency);
}

bool Game::IsCameraShaking()
{
	return Engine::Instance().IsCameraShaking();
}

void Game::StopCameraShake()
{
	Engine::Instance().StopCameraShake();
}

std::vector<AABB> Game::CreateAABB(const Transforms& transforms, uint32_t objectNumber)
{
	return Engine::Instance().CreateAABB(transforms, objectNumber);
}

void Game::toggleWireframeMode()
{
	Engine::Instance().toggleWireframeMode();
}

bool Game::InFrustum(const AABB& aabb)
{
	return Engine::Instance().InFrustum(aabb);
}


