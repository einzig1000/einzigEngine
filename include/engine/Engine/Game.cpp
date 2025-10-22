#include "Engine/Game.h"
#include "Engine/Engine.h"
#include "Camera/CameraController.h"
using namespace DirectX;

static Engine* engine = new Engine;

//std::vector<Game::RenderData_Model*> Game::renderModels;


void Game::Initialize(int width, int height, const std::wstring& title)
{
	engine->Initialize(width, height, title);
}

bool Game::ProcessMessage()
{
	return engine->ProcessMessage();
}

void Game::BeginFrame()
{
	engine->BeginFrame();
}

void Game::UpdateTransforms()
{
	engine->UpdateTransforms();
}

void Game::EndFrame()
{
	engine->EndFrame();
}

void Game::Finalize()
{
	engine->Finalize();
	delete engine;
	engine = nullptr;
}

uint32_t Game::LoadModel(const std::string& directoryPath, const std::string& filename)
{
	return engine->LoadModel(directoryPath, filename);
}

uint32_t Game::LoadTexture(const std::string& filePath)
{
	return engine->LoadTexture(filePath);
}

uint32_t Game::LoadAudio(const std::string& filePath)
{
	return engine->LoadAudio(filePath);
}

TextureData* Game::GetTexture(uint32_t textureNumber)
{
	return engine->GetTexture(textureNumber);
}

void Game::DrawModel(RenderData_Model& renderData)
{
	engine->DrawModel(renderData);
}

void Game::DrawSprite(RenderData_Sprite& renderData)
{
	engine->DrawSprite(renderData);
}

void Game::DrawTriangle(RenderData_Triangle& renderData)
{
	engine->DrawTriangle(renderData);
}

void Game::DrawLine(RenderData_Line& renderData)
{
	engine->DrawLine(renderData);
}

void Game::DrawParticle(RenderData_Particle& renderData)
{
	engine->DrawParticle(renderData);
}

void Game::DrawSphere(const Transforms& transform, const Vector3& center, uint32_t kSubdivision, uint32_t textureNumber, const uint32_t& materialColor, const DrawOptions drawOptions)
{
	engine->DrawSphere(transform, center, kSubdivision, textureNumber, materialColor, drawOptions);
}

void Game::PlayAudio(const uint32_t& audioId, bool loop)
{
	engine->PlayAudio(audioId, loop);
}

void Game::StopAudio(const uint32_t& audioId)
{
	engine->StopAudio(audioId);
}

void Game::SetAudioVolume(const uint32_t& audioId, float volume)
{
	engine->SetAudioVolume(audioId, volume);
}

void Game::SetMasterVolume(float volume)
{
	engine->SetMasterVolume(volume);
}

float Game::GetVolume(const uint32_t& audioId)
{
	return engine->GetVolume(audioId);
}

float Game::GetMasterVolume()
{
	return engine->GetMasterVolume();
}

bool Game::IsAudioPlaying(const uint32_t& audioId)
{
	return engine->IsAudioPlaying(audioId);
}

void Game::SetLightColor(const Vector4 color)
{
	engine->SetLightColor(color);
}

void Game::SetLightDirection(const Vector3 direction)
{
	engine->SetLightDirection(direction);
}

void Game::SetLightIntensity(float intensity)
{
	engine->SetLightIntensity(intensity);
}

void Game::ToggleLightMode(const uint32_t mode)
{
	engine->ToggleLightMode(mode);
}


Vector2 Game::GetMousePosition()
{
	return engine->GetMousePosition();
}

Ray Game::GetMouseRay()
{
	return engine->GetMouseRay();
}

//bool Game::IsCollisionMouseRayObject(uint32_t objectNumber, const Transforms& data)
//{
//	return engine->IsCollisionMouseRayObject(objectNumber, data);
//}

bool Game::GetMousePress(int i)
{
	return engine->GetMousePress(i);
}

bool Game::GetMousePrePress(int i)
{
	return engine->GetMousePrePress(i);
}

uint32_t Game::GetMouseWheel()
{
	return engine->GetMouseWheel();
}

void Game::MoveCameraCenter(Vector3 target, int spendFrame, EaseType easetype)
{
	engine->MoveCameraCenter(target, spendFrame, easetype);
}

void Game::MoveCameraRotate(Vector3 target, int spendFrame, EaseType easetype)
{
	engine->MoveCameraRotate(target, spendFrame, easetype);
}

void Game::MoveCameraDistance(float target, int spendFrame, EaseType easetype)
{
	engine->MoveCameraDistance(target, spendFrame, easetype);
}

void Game::SetControlModeCamera(bool mode)
{
	engine->SetControlModeCamera(mode);
}

CameraController* Game::GetCamera()
{
	return engine->GetCamera();
}

CameraController* Game::GetDebugCamera()
{
	return engine->GetDebugCamera();
}

//// カメラシェイク開始
//void Game::StartCameraShake(float intensity, float duration, float frequency)
//{
//	engine->StartCameraShake(intensity, duration, frequency);
//}
//
//// カメラシェイク中かどうか
//bool Game::IsCameraShaking()
//{
//	return engine->IsCameraShaking();
//}

std::vector<AABB> Game::CreateAABB(const Transforms& transforms, uint32_t objectNumber)
{
	return engine->CreateAABB(transforms, objectNumber);
}

void Game::toggleWireframeMode()
{
	engine->toggleWireframeMode();
}

bool Game::InFrustum(const AABB& aabb)
{
	return engine->InFrustum(aabb);
}


