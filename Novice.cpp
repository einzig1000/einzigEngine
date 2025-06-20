#include "Novice.h"

//std::unique_ptr<Game> Novice::game = nullptr;
//game = std::make_unique<Game>();


static Game* game = new Game;

void Novice::Initialize(int width, int height, const std::wstring& title)
{
	game->Initialize(width, height, title);
}

bool Novice::ProcessMessage()
{
	return game->ProcessMessage();
}

void Novice::BeginFrame()
{
	game->BeginFrame();
}

void Novice::EndFrame()
{
	game->EndFrame();
}

void Novice::Finalize()
{
	game->Finalize();
}

uint32_t Novice::LoadOBJ(const std::string & directoryPath, const std::string & filename)
{
	return game->LoadOBJ(directoryPath, filename);
}

uint32_t Novice::LoadTexture(const std::string& filePath)
{
	return game->LoadTexture(filePath);
}

uint32_t Novice::LoadAudio(const std::string& filePath)
{
	return game->LoadAudio(filePath);
}

void Novice::Drawobj(const Transforms& transform, const Vector3& center, uint32_t objectNumber, uint32_t textureNumber, const uint32_t& materialColor)
{
	game->Drawobj(transform, center, objectNumber, textureNumber, materialColor);
}

void Novice::DrawTriangle(const Transforms & localTransform, const Transforms & worldTransform, const VertexData * vertexData, uint32_t textureNumber, const uint32_t & materialColor)
{
	game->DrawTriangle(localTransform, worldTransform, vertexData, textureNumber, materialColor);
}


void Novice::DrawSphere(const Transforms & localTransform, VertexData * vertexData, uint32_t kSubdivision, uint32_t textureNumber, const uint32_t & materialColor)
{
	game->DrawSphere(localTransform, vertexData, kSubdivision, textureNumber, materialColor);
}


void Novice::DrawSprite(const Transforms & localTransform, VertexData * vertexData, uint32_t textureNumber, const uint32_t & materialColor)
{
	game->DrawSprite(localTransform, vertexData, textureNumber, materialColor);
}

void Novice::DrawLine(const Vector3 & start, const Vector3 & end, const uint32_t & materialColor)
{
	game->DrawLine(start, end, materialColor);
}

void Novice::PlayAudio(const uint32_t & audioId, bool loop)
{
	game->PlayAudio(audioId, loop);
}

void Novice::StopAudio(const uint32_t & audioId)
{
	game->StopAudio(audioId);
}

void Novice::SetAudioVolume(const uint32_t & audioId, float volume)
{
	game->SetAudioVolume(audioId, volume);
}

void Novice::SetMasterVolume(float volume)
{
	game->SetMasterVolume(volume);
}

float Novice::GetVolume(const uint32_t & audioId)
{
	return game->GetVolume(audioId);
}

float Novice::GetMasterVolume()
{
	return game->GetMasterVolume();
}

bool Novice::IsAudioPlaying(const uint32_t& audioId)
{
	return game->IsAudioPlaying(audioId);
}

void Novice::GetMousePosition(Vector2* position)
{
	return game->GetMousePosition(position);
}

void Novice::SetMouseRay()
{
	return game->SetMouseRay();
}

bool Novice::IsCollisionMouseRayAABB(AABB aabb, int objNum)
{
	return game->IsCollisionMouseRayAABB(aabb, objNum);
}

bool Novice::IsPressMouse(int i)
{
	return game->IsPressMouse(i);
}

uint32_t Novice::GetWheel()
{
	return game->GetWheel();
}

void Novice::MoveCenterTarget(Vector3 target, int spendFrame)
{
	game->MoveCenterTarget(target, spendFrame);
}

void Novice::MoveRotateTarget(Vector3 target, int spendFrame)
{
	game->MoveRotateTarget(target, spendFrame);
}

void Novice::MoveDistanceTarget(float target, int spendFrame)
{
	game->MoveDistanceTarget(target, spendFrame);
}

AABB Novice::CreateAABB(const Transforms & transforms, uint32_t objectNumber)
{
	return game->CreateAABB(transforms, objectNumber);
}
