#include "Engine/Game.h"
#include "Engine/Engine.h"
#include "Camera/CameraController.h"
using namespace DirectX;

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
	delete engine;
	engine = nullptr;
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

std::vector<AABB> Game::CreateAABB(const Transforms& transforms, uint32_t objectNumber)
{
	return engine->CreateAABB(transforms, objectNumber);
}

void Game::toggleWireframeMode()
{
	engine->toggleWireframeMode();
}

bool Game::IsAABBInFrustum(const AABB& aabb, const Matrix4x4& worldMatrix)
{
	return engine->IsAABBInFrustum(aabb, worldMatrix);
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

void Game::RenderData_Model::Updata(std::vector<Object3D>& objects)
{
#pragma region 前フレーム情報保存

	this->preTransforms = this->transforms;
	this->preAABB = this->aabb;

#pragma endregion

#pragma region 座標更新

	// 座標更新
	this->velocity -= this->gravity;
	this->velocity += this->acceleration;
	this->transforms.translate += this->velocity;

	// 今フレームの移動量
	this->lastMove = this->transforms.translate - this->preTransforms.translate;

	auto changed = [](float a, float b, float eps) { return ((a - b) > eps) || ((a - b) < eps); };
	constexpr float epsT = 1e-6f;
	constexpr float epsR = 1e-6f;
	constexpr float epsS = 1e-6f;

	// S/R/Tに変化があったか
	this->movedThisFrame =
		changed(this->lastMove.x, 0.0f, epsT) ||
		changed(this->lastMove.y, 0.0f, epsT) || 
		changed(this->lastMove.z, 0.0f, epsT) ||
		changed(this->transforms.rotate.x, this->preTransforms.rotate.x, epsR) ||
		changed(this->transforms.rotate.y, this->preTransforms.rotate.y, epsR) ||
		changed(this->transforms.rotate.z, this->preTransforms.rotate.z, epsR) ||
		changed(this->transforms.scale.x, this->preTransforms.scale.x, epsS) ||
		changed(this->transforms.scale.y, this->preTransforms.scale.y, epsS) ||
		changed(this->transforms.scale.z, this->preTransforms.scale.z, epsS);


	//if (this->movedThisFrame)
	{
		// 移動マトリックス作成
		XMVECTOR scaleVec = XMVectorSet(this->transforms.scale.x, this->transforms.scale.y, this->transforms.scale.z, 0.0f);
		XMVECTOR pivotVec = XMVectorSet(this->pivot.x, this->pivot.y, this->pivot.z, 0.0f);
		XMVECTOR translateVec = XMVectorSet(this->transforms.translate.x, this->transforms.translate.y, this->transforms.translate.z, 0.0f);
		XMVECTOR rotEuler = XMVectorSet(this->transforms.rotate.x, this->transforms.rotate.y, this->transforms.rotate.z, 0.0f);


		// 1) スケール
		XMMATRIX S = XMMatrixScalingFromVector(scaleVec);


		// 2) ピボットオフセット（負）
		XMMATRIX Tneg = XMMatrixTranslationFromVector(XMVectorNegate(pivotVec));


		// 3) 回転（オイラー→クォータニオン→行列）
		XMVECTOR quatEuler = XMQuaternionRotationRollPitchYawFromVector(rotEuler);
		XMMATRIX R = XMMatrixRotationQuaternion(quatEuler);


		// 4) ピボットオフセット（正）
		XMMATRIX Tpos = XMMatrixTranslationFromVector(pivotVec);


		// 5) 平行移動
		XMMATRIX T = XMMatrixTranslationFromVector(translateVec);


		// 6) 合成: S → Tneg → R → Tpos → T
		XMMATRIX world = S * Tneg * R * Tpos * T;


		// 7) 親行列の適用
		if (this->transforms.parentWorld)
		{
			// parentWorld が Matrix4x4 ならまず XMFLOAT4X4 にコピー
			XMFLOAT4X4 parentF4;
			// 4×4 のメモリ配列を直接コピー
			std::memcpy(&parentF4, this->transforms.parentWorld, sizeof(parentF4));
			XMMATRIX parentM = XMLoadFloat4x4(&parentF4);
			world = parentM * world;
		}


		// 8) 結果を transforms.World に格納
		XMFLOAT4X4 tmp;
		XMStoreFloat4x4(&tmp, world);
		for (int i = 0; i < 4; ++i)
			for (int j = 0; j < 4; ++j)
				this->transforms.World.m[i][j] = tmp.m[i][j];
		//for (auto& rd : Game::GetModelList())
		//{
		//	// 1) スケール
		//	Matrix4x4 scale = Matrix4x4::MakeScaleMatrix(rd->transforms.scale);
		//
		//	// 2) ピボットオフセット
		//	Matrix4x4 pivotOffsetNeg = Matrix4x4::MakeTranslateMatrix(-rd->pivot);
		//
		//	// 3) 回転（X, Y, Z軸すべてを合成）
		//	Matrix4x4 rotateX = Matrix4x4::MakeRotateXMatrix(rd->transforms.rotate.x);
		//	Matrix4x4 rotateY = Matrix4x4::MakeRotateYMatrix(rd->transforms.rotate.y);
		//	Matrix4x4 rotateZ = Matrix4x4::MakeRotateZMatrix(rd->transforms.rotate.z);
		//	Matrix4x4 rotate = rotateZ * rotateX * rotateY;
		//
		//	// 4) ピボットへ戻す
		//	Matrix4x4 pivotOffset = Matrix4x4::MakeTranslateMatrix(rd->pivot);
		//
		//	// 5) 平行移動
		//	Matrix4x4 translate = Matrix4x4::MakeTranslateMatrix(rd->transforms.translate);
		//
		//	// 合成
		//	Matrix4x4 world = scale * pivotOffsetNeg * rotate * pivotOffset * translate;
		//
		//	// 6) 親行列があれば乗算
		//	if (rd->transforms.parentWorld)
		//	{
		//		Matrix4x4 parentMatrix = *rd->transforms.parentWorld;
		//		world = parentMatrix * world;
		//	}
		//
		//	// 7) Transforms.World に格納
		//	rd->transforms.World = world;
		//}

		// AABB更新
		this->aabb = CreateAABB(this->transforms, this->model);
	}

#pragma endregion

#pragma region 描画範囲内判定

	bool inFrustum = false;
	for (const auto& aabb : this->aabb)
	{
		if (IsAABBInFrustum(aabb, this->transforms.World))
		{
			inFrustum = true;
			break;
		}
	}
	this->inPicture = inFrustum;

#pragma endregion

#pragma region 衝突判定


	for (auto* target : blockList)
	{
		std::optional<CollisionInf> Inf = isCollisionAABBInf(*target);
		// 衝突していたら
		if (Inf != std::nullopt)
		{
			// 自分の方が軽かったら自分を動かす
			if (this->mass <= target->mass)
			{
				if (this->lastMove.x > 0.0f)
				{
					this->transforms.translate.x -= Inf->depth.x;
					velocity.x = 0.0f;
				}
				else if (this->lastMove.x < 0.0f)
				{
					this->transforms.translate.x += Inf->depth.x;
					velocity.x = 0.0f;
				}
				if (this->lastMove.z > 0.0f)
				{
					this->transforms.translate.z -= Inf->depth.z;
					velocity.z = 0.0f;
				}
				else if (this->lastMove.z < 0.0f)
				{
					this->transforms.translate.z += Inf->depth.z;
					velocity.z = 0.0f;
				}
				if (this->lastMove.y > 0.0f)
				{
					this->transforms.translate.y -= Inf->depth.y;
					velocity.y = 0.0f;
				}
				else if (this->lastMove.y < 0.0f)
				{
					this->transforms.translate.y += Inf->depth.y;
					velocity.y = 0.0f;
				}
			}

			this->aabb = CreateAABB(this->transforms, this->model);

			//Inf = isCollisionAABBInf(*target);



			//Vector3 offset = transforms.translate;
			//
			//// 左方向に移動しているとき
			//if (this->lastMove.x > 0.0f)
			//{
			//	offset.x += this->lastMove.x;
			//	velocity.x = 0.0f;
			//}
			//// 右方向に移動しているとき
			//else if (this->lastMove.x < 0.0f)
			//{
			//	offset.x -= this->lastMove.x;
			//	velocity.x = 0.0f;
			//}
			//
			//// 奥方向に移動しているとき
			//if (this->lastMove.z > 0.0f)
			//{
			//	offset.z += this->lastMove.z;
			//	velocity.z = 0.0f;
			//}
			//// 手前方向に移動しているとき
			//else if (this->lastMove.z < 0.0f)
			//{
			//	offset.z -= this->lastMove.z;
			//	velocity.z = 0.0f;
			//}
			//
			//// 上方向に移動しているとき
			//if (this->lastMove.y > 0.0f)
			//{
			//	offset.y += this->lastMove.y;
			//	velocity.y = 0.0f;
			//}
			//// 下方向に移動しているとき
			//else if (this->lastMove.y < 0.0f)
			//{
			//	offset.y -= this->lastMove.y;
			//	velocity.y = 0.0f;
			//}
			//
			//transforms.translate = { 0.0f,0.0f,0.0f };
			//transforms.translate += offset;
			//// 上方向に移動しているとき
			//if (this->lastMove.y > 0.0f)
			//{
			//	float myHeight = myAABB.max.y - myAABB.min.y;
			//	transforms.translate.y = targetAABB.min.y - myHeight / 2.0f;
			//	velocity.y = 0.0f;
			//}
			//// 下方向に移動しているとき
			//else if (this->lastMove.y < 0.0f)
			//{
			//	float myHeight = myAABB.max.y - myAABB.min.y;
			//	transforms.translate.y = targetAABB.max.y + myHeight / 2.0f;
			//	velocity.y = 0.0f;
			//}
		}
	}

#pragma endregion
	{
		XMVECTOR scaleVec = XMVectorSet(this->transforms.scale.x, this->transforms.scale.y, this->transforms.scale.z, 0.0f);
		XMVECTOR pivotVec = XMVectorSet(this->pivot.x, this->pivot.y, this->pivot.z, 0.0f);
		XMVECTOR translateVec = XMVectorSet(this->transforms.translate.x, this->transforms.translate.y, this->transforms.translate.z, 0.0f);
		XMVECTOR rotEuler = XMVectorSet(this->transforms.rotate.x, this->transforms.rotate.y, this->transforms.rotate.z, 0.0f);

		XMMATRIX S = XMMatrixScalingFromVector(scaleVec);
		XMMATRIX Tneg = XMMatrixTranslationFromVector(XMVectorNegate(pivotVec));
		XMVECTOR quatEuler = XMQuaternionRotationRollPitchYawFromVector(rotEuler);
		XMMATRIX R = XMMatrixRotationQuaternion(quatEuler);
		XMMATRIX Tpos = XMMatrixTranslationFromVector(pivotVec);
		XMMATRIX T = XMMatrixTranslationFromVector(translateVec);

		XMMATRIX world = S * Tneg * R * Tpos * T;

		if (this->transforms.parentWorld)
		{
			XMFLOAT4X4 parentF4;
			std::memcpy(&parentF4, this->transforms.parentWorld, sizeof(parentF4));
			XMMATRIX parentM = XMLoadFloat4x4(&parentF4);
			world = parentM * world;
		}

		XMFLOAT4X4 tmp;
		XMStoreFloat4x4(&tmp, world);
		for (int i = 0; i < 4; ++i)
			for (int j = 0; j < 4; ++j)
				this->transforms.World.m[i][j] = tmp.m[i][j];

		// 最終AABB
		this->aabb = CreateAABB(this->transforms, this->model);
	}
}

// 他のオブジェクトとの衝突判定
bool Game::RenderData_Model::isCollision(RenderData_Model& target) const
{
	for (const auto& aabb1 : target.aabb)
	{
		for (const auto& aabb2 : this->aabb)
		{
			if (IsCollision(aabb1, aabb2))
			{
				return true;
			}
		}
	}
	
	return false;
}

void Game::RenderData_Model::SetBlock(RenderData_Model& target)
{
	// すでに登録済みなら追加しない
	if (std::find(blockList.begin(), blockList.end(), &target) == blockList.end())
	{
		blockList.push_back(&target);
	}
}

// ワールド行列とワールド座標の取得
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
	LookAtOnce(Game::GetCamera()->transform_.translate, roll);
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
	for (const auto& theOne : aabb)
	{
		Vector3 p[8];
		p[0] = { theOne.min.x, theOne.min.y, theOne.min.z };
		p[1] = { theOne.max.x, theOne.min.y, theOne.min.z };
		p[2] = { theOne.max.x, theOne.max.y, theOne.min.z };
		p[3] = { theOne.min.x, theOne.max.y, theOne.min.z };
		p[4] = { theOne.min.x, theOne.min.y, theOne.max.z };
		p[5] = { theOne.max.x, theOne.min.y, theOne.max.z };
		p[6] = { theOne.max.x, theOne.max.y, theOne.max.z };
		p[7] = { theOne.min.x, theOne.max.y, theOne.max.z };

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
	ImGui::DragFloat3((num + "lastMove").c_str(), &lastMove.x, 0.01f);
	ImGui::DragFloat3((num + "velocity").c_str(), &velocity.x, 0.01f);
	ImGui::DragFloat3((num + "acceleration").c_str(), &acceleration.x, 0.01f);
	ImGui::DragFloat3((num + "gravity").c_str(), &gravity.x, 0.01f);
	ImGui::Text("color");
	Vector4 preColor = ConvertUintToVector4(color);
	float floatColor[4] = { preColor.x, preColor.y, preColor.z, preColor.w };
	ImGui::ColorEdit4((num + "color").c_str(), floatColor, 1);
	Vector4 vector4Color = { floatColor[0], floatColor[1], floatColor[2], floatColor[3] };
	color = ConvertVector4ToUint(vector4Color);
	ImGui::Text("option");
	ImGui::Checkbox("wireFrame", &options.wireframe);
	ImGui::Checkbox("lighting", &options.enableLighting);
	bool lookAt = false;
	ImGui::Checkbox("lookAt", &lookAt);
	if (lookAt)
	{
		GetDebugCamera()->SetCenterTarget(transforms.translate, 0, EaseType::IN_BACK);
	}
	ImGui::Text("isCollisionMouse : %d", isCollisionMouseRay);

	ImGui::End();
}

std::optional<CollisionInf> Game::RenderData_Model::isCollisionAABBInf(RenderData_Model& target) const
{
	CollisionInf result;
	result.pair = { -1, -1 };

	// どのAABB同士が衝突しているか
	for (size_t i = 0; i < target.aabb.size(); ++i)
	{
		for (size_t j = 0; j < this->aabb.size(); ++j)
		{
			if (IsLooseCollision(target.aabb[i], this->aabb[j], 0.001f))
			{
				Vector3 depth = this->aabb[j].GetCollisionDepth(target.aabb[i]);
				result.pair = { static_cast<int>(i), static_cast<int>(j) };
				result.depth = depth;
				break;
			}
		}
	}
	if (result.pair == Vector2int{ -1, -1 }) return std::nullopt;
	return result;
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