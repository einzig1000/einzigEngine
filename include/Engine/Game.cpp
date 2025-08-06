#include "Engine/Game.h"

static Engine* engine = new Engine;

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

void Game::EndFrame()
{
	engine->EndFrame();
}

void Game::Finalize()
{
	engine->Finalize();
}

uint32_t Game::LoadOBJ(const std::string& directoryPath, const std::string& filename)
{
	return engine->LoadOBJ(directoryPath, filename);
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

void Game::Drawobj(const Transforms& transform, const Vector3& center, uint32_t objectNumber, uint32_t textureNumber, const uint32_t& materialColor, const DrawOptions drawOptions)
{
	engine->Drawobj(transform, center, objectNumber, textureNumber, materialColor, drawOptions);
}

void Game::DrawSphere(const Transforms& transform, const Vector3& center, uint32_t kSubdivision, uint32_t textureNumber, const uint32_t& materialColor, const DrawOptions drawOptions)
{
	engine->DrawSphere(transform, center, kSubdivision, textureNumber, materialColor, drawOptions);
}

void Game::DrawTriangle(const Transforms& transform, const Vector3& pos1, const Vector3& pos2, const Vector3& pos3, uint32_t textureNumber, const uint32_t& materialColor, const DrawOptions drawOptions)
{
	engine->DrawTriangle(transform, pos1, pos2, pos3, textureNumber, materialColor, drawOptions);
}

void Game::DrawSprite(const Transforms& transform, const Vector2& center, uint32_t textureNumber, const uint32_t& materialColor, const DrawOptions drawOptions)
{
	engine->DrawSprite(transform, center, textureNumber, materialColor, drawOptions);
}

void Game::DrawLine(const Vector3& start, const Vector3& end, const uint32_t& materialColor)
{
	engine->DrawLine(start, end, materialColor);
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


void Game::GetMousePosition(Vector2* position)
{
	return engine->GetMousePosition(position);
}

void Game::SetMouseRay()
{
	return engine->SetMouseRay();
}

bool Game::IsCollisionMouseRayAABB(AABB aabb, int objNum)
{
	return engine->IsCollisionMouseRayAABB(aabb, objNum);
}

bool Game::IsPressMouse(int i)
{
	return engine->IsPressMouse(i);
}

uint32_t Game::GetMouseWheel()
{
	return engine->GetMouseWheel();
}

void Game::MoveCenterTarget(Vector3 target, int spendFrame)
{
	engine->MoveCenterTarget(target, spendFrame);
}

void Game::MoveRotateTarget(Vector3 target, int spendFrame)
{
	engine->MoveRotateTarget(target, spendFrame);
}

void Game::MoveDistanceTarget(float target, int spendFrame)
{
	engine->MoveDistanceTarget(target, spendFrame);
}

AABB Game::CreateAABB(const Transforms& transforms, uint32_t objectNumber)
{
	return engine->CreateAABB(transforms, objectNumber);
}

void Game::toggleWireframeMode()
{
	engine->toggleWireframeMode();
}
