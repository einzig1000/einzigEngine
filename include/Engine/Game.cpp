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

void Game::DrawSphere(const Transforms& transform, const Vector3& center, uint32_t kSubdivision, uint32_t textureNumber, const uint32_t& materialColor, const DrawOptions drawOptions)
{
	engine->DrawSphere(transform, center, kSubdivision, textureNumber, materialColor, drawOptions);
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

Ray Game::GetMouseRay()
{
	return engine->GetMouseRay();
}

bool Game::IsCollisionMouseRayAABB(uint32_t objectNumber, const Transforms& data)
{
	return engine->IsCollisionMouseRayAABB(objectNumber, data);
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
	this->ID = int(GetModelList().size());
}

Game::RenderData_Model::~RenderData_Model()
{
	SubModel(this);
}

// 他のオブジェクトとの衝突判定
bool Game::RenderData_Model::isCollision(RenderData_Model& target) const
{
	for (auto& other : Game::GetModelList())
	{ 
		// 自分自身は除外
		if (&target == other) continue;
		// いずれはそれぞれに形に適したものに変更したい AABB組み合わせたりもしたい
		if (IsCollision(target.AABB, other->AABB))
		{
			return true;
		}
	}
	return false;
}

// ワールド
Matrix4x4 Game::RenderData_Model::GetWorldMatrix() const
{
	if (transforms.parentWorld)
	{
		return (*transforms.parentWorld) * transforms.World;
	}
	return transforms.World;
}
Vector3 Game::RenderData_Model::GetWorldPosition() const
{
	return Transform(Vector3(0.0f, 0.0f, 0.0f), GetWorldMatrix());
}

// 任意のポイントを向く
void Game::RenderData_Model::LookAtOnce(const Vector3& targetWorldPos, float roll)
{
	Vector3 direction = (targetWorldPos - this->GetWorldPosition()).Normalized();
	float yaw = std::atan2(direction.x, direction.z); // Y軸回り
	float pitch = std::asin(-direction.y);            // X軸回り

	transforms.rotate = { pitch, yaw, roll };
}
void Game::RenderData_Model::LookAtOnce(const RenderData_Model &other, float roll)
{
	LookAtOnce(other.GetWorldPosition(), roll);
}
void Game::RenderData_Model::LookAtCamera(float roll)
{
	LookAtOnce(Game::GetCamera()->GetCenter(), roll);
}
void Game::RenderData_Model::LookAtFront(float roll)
{
	Vector3 frontPos = this->GetWorldPosition() + Vector3{ 0.0f, 0.0f, 1.0f };
	LookAtOnce(frontPos, roll);
}



void Game::RenderData_Model::Draw()
{
	engine->Drawobj(*this);
}

void Game::RenderData_Model::DrawAABB()
{
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

void Game::RenderData_Model::DrawImGui()
{
	std::string str = "object : " + std::to_string(this->ID);
	std::string num = std::to_string(this->ID) + ":";

    ImGui::Begin(str.c_str());

	ImGui::Text("transforms");
	ImGui::DragFloat3((num + "scale").c_str(), &transforms.scale.x, 0.01f);
	ImGui::DragFloat3((num + "translate").c_str(), &transforms.translate.x, 0.01f);
	ImGui::DragFloat3((num + "rotate").c_str(), &transforms.rotate.x, 0.01f);
	ImGui::DragFloat3((num + "pivot").c_str(), &pivot.x, 0.01f);
	ImGui::Text("uvTransform");
	ImGui::DragFloat3((num + "UVscale").c_str(), &uvTransform.scale.x, 0.01f);
	ImGui::DragFloat3((num + "UVtranslate").c_str(), &uvTransform.translate.x, 0.01f);
	ImGui::DragFloat3((num + "UVrotate").c_str(), &uvTransform.rotate.x, 0.01f);
	ImGui::Text("velocity");
	ImGui::DragFloat3((num + "velocity").c_str(), &velocity.x, 0.01f);
	ImGui::DragFloat3((num + "acceleration").c_str(), &acceleration.x, 0.01f);
	ImGui::DragFloat((num + "gravity").c_str(), &gravity, 0.01f);
	ImGui::Text("color");
	static float floatColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	ImGui::ColorEdit4((num + "color").c_str(), floatColor, 1);
	Vector4 vector4Color = { floatColor[0], floatColor[1], floatColor[2], floatColor[3] };
	color = ConvertVector4ToUint(vector4Color);
	ImGui::Text("option");
	ImGui::Checkbox("wireFrame", &options.wireframe);
	ImGui::Checkbox("lighting", &options.enableLighting);


	ImGui::End();
}



void Game::RenderData_Sprite::Draw()
{
	engine->DrawSprite(*this);
}


void Game::RenderData_Triangle::Draw()
{
	engine->DrawTriangle(*this);
}


void Game::RenderData_Particle::Draw()
{
	engine->DrawParticle(*this);
}

void Game::RenderData_Particle::DrawEmitter()
{
	// エミッターがAABB
	if (this->option.emitterShape)
	{
		Vector3 p[8];
		p[0] = { this->emitterAABB.min.x, this->emitterAABB.min.y, this->emitterAABB.min.z };
		p[1] = { this->emitterAABB.max.x, this->emitterAABB.min.y, this->emitterAABB.min.z };
		p[2] = { this->emitterAABB.max.x, this->emitterAABB.max.y, this->emitterAABB.min.z };
		p[3] = { this->emitterAABB.min.x, this->emitterAABB.max.y, this->emitterAABB.min.z };
		p[4] = { this->emitterAABB.min.x, this->emitterAABB.min.y, this->emitterAABB.max.z };
		p[5] = { this->emitterAABB.max.x, this->emitterAABB.min.y, this->emitterAABB.max.z };
		p[6] = { this->emitterAABB.max.x, this->emitterAABB.max.y, this->emitterAABB.max.z };
		p[7] = { this->emitterAABB.min.x, this->emitterAABB.max.y, this->emitterAABB.max.z };

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
	// 
	else
	{
		Transforms transforms;
		transforms.scale = this->emitterSphere.radius;
		DrawOptions option;

		engine->DrawSphere(transforms, this->emitterSphere.center, 12, 0, 0xFFFFFF22, option);
	}
}