#include "Engine/Game.h"
#include "Engine/Engine.h"
#include "Camera/CameraController.h"

static Engine* engine = new Engine;

std::vector<Game::RenderData_Model*> Game::renderModels;


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

void Game::Drawobj(Game::RenderData_Model& renderData)
{
	engine->Drawobj(renderData);
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


Vector2 Game::GetMousePosition()
{
	return engine->GetMousePosition();
}

void Game::SetMouseRay()
{
	return engine->SetMouseRay();
}

Ray Game::GetMouseRay()
{
	return engine->GetMouseRay();
}

bool Game::IsCollisionMouseRayAABB(uint32_t objectNumber, const Transforms& data)
{
	return engine->IsCollisionMouseRayAABB(objectNumber,data);
}

bool Game::GetMousePress(int i)
{
	return engine->IsPressMouse(i);
}

uint32_t Game::GetMouseWheel()
{
	return engine->GetMouseWheel();
}

void Game::MoveCenterTarget(Vector3 target, int spendFrame, EaseType easetype)
{
	engine->MoveCenterTarget(target, spendFrame, easetype);
}

void Game::MoveRotateTarget(Vector3 target, int spendFrame, EaseType easetype)
{
	engine->MoveRotateTarget(target, spendFrame, easetype);
}

void Game::MoveDistanceTarget(float target, int spendFrame, EaseType easetype)
{
	engine->MoveDistanceTarget(target, spendFrame, easetype);
}

void Game::SetControlModeCamera(bool mode)
{
	engine->SetControlModeCamera(mode);
}

void Game::SetControlModeCameraCenter(bool mode)
{
	engine->SetControlModeCameraCenter(mode);
}

void Game::SetControlModeCameraRotate(bool mode)
{
	engine->SetControlModeCameraRotate(mode);
}

void Game::SetControlModeCameraDistance(bool mode)
{
	engine->SetControlModeCameraDistance(mode);
}

CameraController* Game::GetCamera()
{
	return engine->GetCamera();
}

AABB Game::CreateAABB(const Transforms& transforms, uint32_t objectNumber)
{
	return engine->CreateAABB(transforms, objectNumber);
}

void Game::toggleWireframeMode()
{
	engine->toggleWireframeMode();
}












Game::RenderData_Model::RenderData_Model()
{
	AddModel(this);
}

Vector3 Game::RenderData_Model::GetWorldPosition() const
{
	if (transforms.parentWorld)
	{
		// 親のワールド行列があるならローカル translate を合成
		return Transform(transforms.translate, *transforms.parentWorld);
	}
	// ルート直下ならそのまま
	return transforms.translate;
}

// 現在の向きに沿った前方方向のワールド座標を返す
Vector3 Game::RenderData_Model::GetTargetWorldPosition() const
{ 
	// 自身のワールド位置 ＋ ローカルZ軸(0,0,1)を四元数回転
	Vector3 forward = rotationQuat * Vector3{ 0.0f, 0.0f, 1.0f };
	return GetWorldPosition() + forward;
}


void Game::RenderData_Model::LookAtOnce(const Vector3& targetWorldPos)
{
	// 自身のワールド位置
	Vector3 worldPos = GetWorldPosition();

	// forward ベクトル
	Vector3 forward = (targetWorldPos - worldPos).Normalize();

	// up ベクトル（Y軸を上とする）
	Vector3 up = { 0.0f, 1.0f, 0.0f };
	// ほぼ平行ならZ軸を代替 up に
	if (std::abs(forward.Dot(up)) > 0.999f)
	{
		up = { 0.0f, 0.0f, 1.0f };
	}

	// 内部四元数を更新
	rotationQuat = Quaternion::LookRotation(forward, up);
}

void Game::RenderData_Model::LookAtOnce(const RenderData_Model* other)
{
	LookAtOnce(other->GetWorldPosition());
}

void Game::RenderData_Model::LookAtCamera()
{
	LookAtOnce(Game::GetCamera()->GetCenter());
}

void Game::RenderData_Model::LookAtFront()
{
	Vector3 frontPos = GetWorldPosition() + Vector3{ 0.0f, 0.0f, 1.0f };
	LookAtOnce(frontPos);
}


void Game::RenderData_Model::SetRotationEuler(const Vector3 & eulerRad)
{
	rotationQuat = Quaternion::MakeFromEulerAngles(eulerRad);
}

Vector3 Game::RenderData_Model::GetRotationEuler() const
{
	return rotationQuat.ToEulerDegrees();
}

void Game::RenderData_Model::Draw()
{
	Game::Drawobj(*this);
}

void Game::RenderData_Model::DrawAABB()
{
	CreateAABB();

	Vector3 p[8];
	p[0] = { AABB.min.x, AABB.min.y, AABB.min.z };
	p[1] = { AABB.max.x, AABB.min.y, AABB.min.z };
	p[2] = { AABB.max.x, AABB.max.y, AABB.min.z };
	p[3] = { AABB.min.x, AABB.max.y, AABB.min.z };
	p[4] = { AABB.min.x, AABB.min.y, AABB.max.z };
	p[5] = { AABB.max.x, AABB.min.y, AABB.max.z };
	p[6] = { AABB.max.x, AABB.max.y, AABB.max.z };
	p[7] = { AABB.min.x, AABB.max.y, AABB.max.z };

	// 下側
	Game::DrawLine(p[0], p[1], 0xFF0000FF);
	Game::DrawLine(p[1], p[2], 0xFF0000FF);
	Game::DrawLine(p[2], p[3], 0xFF0000FF);
	Game::DrawLine(p[3], p[0], 0xFF0000FF);

	// 上側
	Game::DrawLine(p[4], p[5], 0xFF0000FF);
	Game::DrawLine(p[5], p[6], 0xFF0000FF);
	Game::DrawLine(p[6], p[7], 0xFF0000FF);
	Game::DrawLine(p[7], p[4], 0xFF0000FF);

	// 側面
	Game::DrawLine(p[0], p[4], 0xFF0000FF);
	Game::DrawLine(p[1], p[5], 0xFF0000FF);
	Game::DrawLine(p[2], p[6], 0xFF0000FF);
	Game::DrawLine(p[3], p[7], 0xFF0000FF);
}

void Game::RenderData_Model::CreateAABB()
{
	this->AABB = Game::CreateAABB(this->transforms, this->model);
}
