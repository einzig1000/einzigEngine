#include "Engine/Game.h"
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

uint32_t Game::LoadModel(const std::string& directoryPath, const std::string& filename)
{
	return Engine::Instance().LoadModel(directoryPath, filename);
}

uint32_t Game::LoadTexture(const std::string& filePath)
{
	return Engine::Instance().LoadTexture(filePath);
}

uint32_t Game::LoadAudio(const std::string& filePath)
{
	return Engine::Instance().LoadAudio(filePath);
}

TextureData* Game::GetTexture(uint32_t textureNumber)
{
	return Engine::Instance().GetTexture(textureNumber);
}

size_t Game::GetTextureCount()
{
	return Engine::Instance().GetTextureCount();
}

size_t Game::GetModelCount()
{
	return Engine::Instance().GetModelCount();
}

void Game::DrawModel(RenderData_Model& renderData)
{
	Engine::Instance().DrawModel(renderData);
}

void Game::DrawSprite(RenderData_Sprite& renderData)
{
	Engine::Instance().DrawSprite(renderData);
}

void Game::DrawTriangle(RenderData_Triangle& renderData)
{
	Engine::Instance().DrawTriangle(renderData);
}

void Game::DrawLine(RenderData_Line& renderData)
{
	Engine::Instance().DrawLine(renderData);
}

void Game::DrawParticle(RenderData_Particle& renderData)
{
	Engine::Instance().DrawParticle(renderData);
}

void Game::AddSphere(Vector3 pos, Vector3 radius, uint32_t color)
{
	Engine::Instance().AddSphere(pos, radius, color);
}

void Game::AddAABB(AABB aabb, uint32_t color)
{
	Engine::Instance().AddAABB(aabb, color);
}

void Game::PlayAudio(const uint32_t& audioId, bool loop)
{
	Engine::Instance().PlayAudio(audioId, loop);
}

void Game::StopAudio(const uint32_t& audioId)
{
	Engine::Instance().StopAudio(audioId);
}

void Game::SetAudioVolume(const uint32_t& audioId, float volume)
{
	Engine::Instance().SetAudioVolume(audioId, volume);
}

void Game::SetMasterVolume(float volume)
{
	Engine::Instance().SetMasterVolume(volume);
}

float Game::GetVolume(const uint32_t& audioId)
{
	return Engine::Instance().GetVolume(audioId);
}

float Game::GetMasterVolume()
{
	return Engine::Instance().GetMasterVolume();
}

bool Game::IsAudioPlaying(const uint32_t& audioId)
{
	return Engine::Instance().IsAudioPlaying(audioId);
}

void Game::SetLightColor(const Vector4 color)
{
	Engine::Instance().SetLightColor(color);
}

void Game::SetLightDirection(const Vector3 direction)
{
	Engine::Instance().SetLightDirection(direction);
}

void Game::SetLightIntensity(float intensity)
{
	Engine::Instance().SetLightIntensity(intensity);
}

void Game::ToggleLightMode(const uint32_t mode)
{
	Engine::Instance().ToggleLightMode(mode);
}


Vector2 Game::GetMousePosition()
{
	return Engine::Instance().GetMousePosition();
}

Ray Game::GetMouseRay()
{
	return Engine::Instance().GetMouseRay();
}

bool Game::GetMousePress(int i)
{
	return Engine::Instance().GetMousePress(i);
}

bool Game::GetMousePrePress(int i)
{
	return Engine::Instance().GetMousePrePress(i);
}

uint32_t Game::GetMouseWheel()
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


