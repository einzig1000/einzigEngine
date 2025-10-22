#include "DrawSystem/RenderData/RenderData.h"
#include "Game.h"
#include "Camera/CameraController.h"
using namespace DirectX;

std::vector<RenderData_Model*> RenderData_Model::renderModels;
std::vector<RenderData_Sprite*> RenderData_Sprite::renderSprites;
std::vector<RenderData_Triangle*> RenderData_Triangle::renderTriangles;
std::vector<RenderData_Line*> RenderData_Line::renderLines;
std::vector<RenderData_Particle*> RenderData_Particle::renderParticles;

#pragma region model

RenderData_Model::RenderData_Model()
{
	renderModels.push_back(this);
	this->ID = int(renderModels.size());
}

RenderData_Model::~RenderData_Model()
{
	auto it = std::find(renderModels.begin(), renderModels.end(), this);
	if (it != renderModels.end())
	{
		renderModels.erase(it);
	}
}

void RenderData_Model::Update(std::vector<Object3D>& objects)
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

	auto changed = [](float a, float b) { return ((a - b) > eps) || ((a - b) < eps); };
	constexpr float epsT = 1e-6f;
	constexpr float epsR = 1e-6f;
	constexpr float epsS = 1e-6f;

	// S/R/Tに変化があったか
	this->movedThisFrame =
		changed(this->lastMove.x, 0.0f) ||
		changed(this->lastMove.y, 0.0f) ||
		changed(this->lastMove.z, 0.0f) ||
		changed(this->transforms.rotate.x, this->preTransforms.rotate.x) ||
		changed(this->transforms.rotate.y, this->preTransforms.rotate.y) ||
		changed(this->transforms.rotate.z, this->preTransforms.rotate.z) ||
		changed(this->transforms.scale.x, this->preTransforms.scale.x) ||
		changed(this->transforms.scale.y, this->preTransforms.scale.y) ||
		changed(this->transforms.scale.z, this->preTransforms.scale.z);


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
		//for (auto& rd : GetModelList())
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
		this->aabb = Game::CreateAABB(this->transforms, this->model);
	}

#pragma endregion

#pragma region 描画範囲内判定

	bool inFrustum = false;
	for (const auto& aabb : this->aabb)
	{
		if (Game::InFrustum(aabb))
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
			}

			this->aabb = Game::CreateAABB(this->transforms, this->model);


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
					XMFLOAT4X4 parentF4; std::memcpy(&parentF4, this->transforms.parentWorld, sizeof(parentF4));
					XMMATRIX parentM = XMLoadFloat4x4(&parentF4);
					world = parentM * world;
				}
				XMFLOAT4X4 tmp; XMStoreFloat4x4(&tmp, world);
				for (int i = 0; i < 4; ++i)
					for (int j = 0; j < 4; ++j)
						this->transforms.World.m[i][j] = tmp.m[i][j];

				this->aabb = Game::CreateAABB(this->transforms, this->model);
			}

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
}

// 他のオブジェクトとの衝突判定
bool RenderData_Model::isCollision(RenderData_Model& target) const
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

void RenderData_Model::SetBlock(RenderData_Model& target)
{
	// すでに登録済みなら追加しない
	if (std::find(blockList.begin(), blockList.end(), &target) == blockList.end())
	{
		blockList.push_back(&target);
	}
}

// ワールド行列とワールド座標の取得
Matrix4x4 RenderData_Model::GetWorldMatrix() const
{
	if (transforms.parentWorld)
	{
		return (*transforms.parentWorld) * transforms.World;
	}
	return transforms.World;
}
Vector3 RenderData_Model::GetWorldPosition() const
{
	return Transform(Vector3(0.0f, 0.0f, 0.0f), GetWorldMatrix());
}

// 任意のポイントを向く
void RenderData_Model::LookAtOnce(const Vector3& targetWorldPos, float roll)
{
	Vector3 direction = (targetWorldPos - this->GetWorldPosition()).Normalized();
	float yaw = std::atan2(direction.x, direction.z); // Y軸回り
	float pitch = std::asin(-direction.y);            // X軸回り

	transforms.rotate = { pitch, yaw, roll };
}
void RenderData_Model::LookAtOnce(const RenderData_Model& other, float roll)
{
	LookAtOnce(other.GetWorldPosition(), roll);
}
void RenderData_Model::LookAtCamera(float roll)
{
	LookAtOnce(Game::GetCamera()->transform_.translate, roll);
}
void RenderData_Model::LookAtFront(float roll)
{
	Vector3 frontPos = this->GetWorldPosition() + Vector3{ 0.0f, 0.0f, 1.0f };
	LookAtOnce(frontPos, roll);
}

void RenderData_Model::Draw()
{
	Game::DrawModel(*this);
}

void RenderData_Model::DrawAABB()
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

		RenderData_Line line;
		line.color = 0xFF0000FF;
		line.points.push_back(p[0]);
		line.points.push_back(p[1]);
		line.points.push_back(p[2]);
		line.points.push_back(p[3]);
		line.points.push_back(p[0]);

		line.points.push_back(p[4]);
		line.points.push_back(p[5]);
		line.points.push_back(p[6]);
		line.points.push_back(p[7]);
		line.points.push_back(p[4]);

		line.points.push_back(p[5]);
		line.points.push_back(p[1]);
		line.points.push_back(p[2]);
		line.points.push_back(p[6]);
		line.points.push_back(p[7]);
		line.points.push_back(p[3]);

		line.Draw();
	}
}

void RenderData_Model::DrawImGui()
{
	std::string str;
	if (this->name != "NULL") str = this->name;
	else str = "object : " + std::to_string(this->ID);
	
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
		Game::GetDebugCamera()->SetCenterTarget(transforms.translate, 0, EaseType::IN_BACK);
	}
	ImGui::Text("isCollisionMouse : %d", isCollisionMouseRay);

	ImGui::End();
}

void RenderData_Model::CollisionAction(const Vector3& depth, RenderData_Model& target)
{}

std::optional<CollisionInf> RenderData_Model::isCollisionAABBInf(RenderData_Model& target) const
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

#pragma endregion

#pragma region sprite

RenderData_Sprite::RenderData_Sprite()
{
	renderSprites.push_back(this);
	this->ID = int(renderSprites.size());
}

RenderData_Sprite::~RenderData_Sprite()
{
	auto it = std::find(renderSprites.begin(), renderSprites.end(), this);
	if (it != renderSprites.end())
	{
		renderSprites.erase(it);
	}
}

void RenderData_Sprite::Draw()
{
	Game::DrawSprite(*this);
}

void RenderData_Sprite::DrawImGui()
{
	std::string str;
	if (this->name != "NULL") str = this->name;
	else str = "sprite : " + std::to_string(this->ID);

	std::string num = std::to_string(this->ID) + ":";

	ImGui::Begin(str.c_str());
	ImGui::Text("transforms");
	ImGui::DragFloat2((num + "scale").c_str(), &transforms.scale.x, 0.01f);
	ImGui::DragFloat2((num + "translate").c_str(), &transforms.translate.x, 1.0f);
	ImGui::DragFloat3((num + "rotate").c_str(), &transforms.rotate.x, 0.01f);
	ImGui::Text("uvTransform");
	ImGui::DragFloat2((num + "UVscale").c_str(), &uvTransform.scale.x, 0.01f);
	ImGui::DragFloat2((num + "UVtranslate").c_str(), &uvTransform.translate.x, 0.01f);
	ImGui::DragFloat((num + "UVrotate").c_str(), &uvTransform.rotate.z, 0.01f);
	ImGui::Text("pivot");
	ImGui::DragFloat2((num + "pivot").c_str(), &pivot.x, 0.1f);
	ImGui::Text("color");
	Vector4 preColor = ConvertUintToVector4(color);
	float floatColor[4] = { preColor.x, preColor.y, preColor.z, preColor.w };
	ImGui::ColorEdit4((num + "color").c_str(), floatColor, 1);
	Vector4 vector4Color = { floatColor[0], floatColor[1], floatColor[2], floatColor[3] };
	color = ConvertVector4ToUint(vector4Color);
	ImGui::Text("Anker");
	const char* items[] =
	{ "Center","CenterLeft","CenterRight","CenterTop","CenterDown","LeftTop","RightTop","LeftDown","RightDown" };
	int item_current = int(this->anker);
	ImGui::Combo((num + "Anker").c_str(), &item_current, items, IM_ARRAYSIZE(items));
	{
		if (item_current == 0)anker = Anker::Center;
		if (item_current == 1)anker = Anker::CenterLeft;
		if (item_current == 2)anker = Anker::CenterRight;
		if (item_current == 3)anker = Anker::CenterTop;
		if (item_current == 4)anker = Anker::CenterDown;
		if (item_current == 5)anker = Anker::LeftTop;
		if (item_current == 6)anker = Anker::RightTop;
		if (item_current == 7)anker = Anker::LeftDown;
		if (item_current == 8)anker = Anker::RightDown;
	}

	ImGui::Text("option");
	if (isCollisionMouseRay)ImGui::Text("isCollisionMouse : true");
	else ImGui::Text("isCollisionMouse : false");
	ImGui::End();
}

#pragma endregion

#pragma region triangle

RenderData_Triangle::RenderData_Triangle()
{
	renderTriangles.push_back(this);
	this->ID = int(renderTriangles.size());
}

RenderData_Triangle::~RenderData_Triangle()
{
	auto it = std::find(renderTriangles.begin(), renderTriangles.end(), this);
	if (it != renderTriangles.end())
	{
		renderTriangles.erase(it);
	}
}

void RenderData_Triangle::Draw()
{
	Game::DrawTriangle(*this);
}

void RenderData_Triangle::DrawImGui()
{
	std::string str;
	if (this->name != "NULL") str = this->name;
	else str = "triangle : " + std::to_string(this->ID);

	std::string num = std::to_string(this->ID) + ":";

	ImGui::Begin(str.c_str());
	ImGui::Text("transforms");
	ImGui::DragFloat3((num + "scale").c_str(), &transforms.scale.x, 0.01f);
	ImGui::DragFloat3((num + "translate").c_str(), &transforms.translate.x, 1.0f);
	ImGui::DragFloat3((num + "rotate").c_str(), &transforms.rotate.x, 0.01f);
	ImGui::Text("uvTransform");
	ImGui::DragFloat3((num + "UVscale").c_str(), &uvTransform.scale.x, 0.01f);
	ImGui::DragFloat3((num + "UVtranslate").c_str(), &uvTransform.translate.x, 0.01f);
	ImGui::DragFloat((num + "UVrotate").c_str(), &uvTransform.rotate.z, 0.01f);
	ImGui::Text("pos");
	ImGui::DragFloat3((num + "pos1").c_str(), &pos1.x, 0.1f);
	ImGui::DragFloat3((num + "pos2").c_str(), &pos2.x, 0.1f);
	ImGui::DragFloat3((num + "pos3").c_str(), &pos3.x, 0.1f);
	ImGui::Text("pivot");
	//ImGui::DragFloat2((num + "pivot").c_str(), &pivot.x, 0.1f);
	ImGui::Text("color");
	Vector4 preColor = ConvertUintToVector4(color);
	float floatColor[4] = { preColor.x, preColor.y, preColor.z, preColor.w };
	ImGui::ColorEdit4((num + "color").c_str(), floatColor, 1);
	Vector4 vector4Color = { floatColor[0], floatColor[1], floatColor[2], floatColor[3] };
	color = ConvertVector4ToUint(vector4Color);

	ImGui::End();
}

#pragma endregion

#pragma region line

RenderData_Line::RenderData_Line()
{
	renderLines.push_back(this);
	this->ID = int(renderLines.size());
	this->points.push_back(Vector3{ 0.0f,0.0f,0.0f });
}

RenderData_Line::~RenderData_Line()
{
	auto it = std::find(renderLines.begin(), renderLines.end(), this);
	if (it != renderLines.end())
	{
		renderLines.erase(it);
	}
}

void RenderData_Line::Draw()
{
	if (this->points.size() < 3)
	{
		this->points.push_back(Vector3{ 0.0f,0.0f,0.0f });
		this->points.push_back(Vector3{ 0.0f,0.0f,0.0f });
	}
	Game::DrawLine(*this);
}

void RenderData_Line::DrawPoints()
{


}

void RenderData_Line::DrawImGui()
{
	std::string str;
	if (this->name != "NULL") str = this->name;
	else str = "line : " + std::to_string(this->ID);
	std::string num = std::to_string(this->ID);

	ImGui::Begin(str.c_str());

	ImGui::Text("color");
	Vector4 preColor = ConvertUintToVector4(color);
	float floatColor[4] = { preColor.x, preColor.y, preColor.z, preColor.w };
	ImGui::ColorEdit4((num + " : color").c_str(), floatColor, 1);
	Vector4 vector4Color = { floatColor[0], floatColor[1], floatColor[2], floatColor[3] };
	color = ConvertVector4ToUint(vector4Color);

	ImGui::Text("LineType");
	const char* items[] =
	{ "Line","BezierCurve", "SplineCurve" };
	int item_current = int(this->lineType);
	ImGui::Combo((num + "LineType").c_str(), &item_current, items, IM_ARRAYSIZE(items));
	{
		if (item_current == 0)lineType = LineType::Line;
		if (item_current == 1)lineType = LineType::BezierCurve;
		if (item_current == 2)lineType = LineType::SplineCurve;
	}

	ImGui::Text("kSubdivision");
	ImGui::DragInt((num + " : kSubdivision").c_str(), (int*)&kSubdivision, 1, 1, 100);

	ImGui::Text("points");
	size_t i = 1;
	for (i = 1; i < points.size(); ++i)
	{
		ImGui::DragFloat3((num + "." + std::to_string(i)).c_str(), &points[i].x, 0.1f);
		ImGui::SameLine();
		if (ImGui::Button(("delete" + num + "." + std::to_string(i)).c_str()))
		{
			points.erase(points.begin() + i);
			--i;
		}
	}

	ImGui::Text("addPoint");
	ImGui::DragFloat3((num + " : addPoint").c_str(), &points[0].x, 0.1f);
	if (ImGui::Button((num + " : AddPoint").c_str()))
	{
		Vector3 newPoint = points[0];
		points.push_back(newPoint);
	}
	ImGui::End();
}

#pragma endregion

#pragma region particle

RenderData_Particle::RenderData_Particle()
{
	renderParticles.push_back(this);
	this->ID = int(renderParticles.size());
}

RenderData_Particle::~RenderData_Particle()
{
	auto it = std::find(renderParticles.begin(), renderParticles.end(), this);
	if (it != renderParticles.end())
	{
		renderParticles.erase(it);
	}
}

void RenderData_Particle::Draw()
{
	Game::DrawParticle(*this);
}

void RenderData_Particle::DrawImGui()
{
	std::string str;
	if (this->name != "NULL") str = this->name;
	else str = "particle : " + std::to_string(this->ID);

	std::string num = std::to_string(this->ID) + ":";

	ImGui::Begin(str.c_str());
	ImGui::Text("scale");
	ImGui::DragFloat3(("S.val" + num).c_str(), &this->scale.value.x, 0.01f);
	ImGui::DragFloat3(("S.vel" + num).c_str(), &this->scale.velocity.x, 0.01f);
	ImGui::DragFloat3(("S.acc" + num).c_str(), &this->scale.acceleration.x, 0.01f);
	ImGui::Text("rotate");
	ImGui::DragFloat3(("R.val" + num).c_str(), &this->rotate.value.x, 0.01f);
	ImGui::DragFloat3(("R.vel" + num).c_str(), &this->rotate.velocity.x, 0.01f);
	ImGui::DragFloat3(("R.acc" + num).c_str(), &this->rotate.acceleration.x, 0.01f);
	ImGui::Text("translate");
	ImGui::DragFloat3(("T.val" + num).c_str(), &this->translate.value.x, 0.01f);
	ImGui::DragFloat3(("T.vel" + num).c_str(), &this->translate.velocity.x, 0.01f);
	ImGui::DragFloat3(("T.acc" + num).c_str(), &this->translate.acceleration.x, 0.01f);

	ImGui::Text("density");
	int particlesPerEmission = int(this->particlesPerEmission);
	ImGui::DragInt(("particlePerEmission" + num).c_str(), &particlesPerEmission);
	if (particlesPerEmission < 0)particlesPerEmission = 0;
	this->particlesPerEmission = uint32_t(particlesPerEmission);
	int emissionDelay = int(this->emissionDelay);
	ImGui::DragInt(("emissionDelay" + num).c_str(), &emissionDelay);
	if (emissionDelay < 1)emissionDelay = 1;
	this->emissionDelay = uint32_t(emissionDelay);

	ImGui::Text("lifetime");
	ImGui::DragInt(("liveMax" + num).c_str(), &this->liveMax);

	ImGui::Text("color");
	Vector4 preColor = ConvertUintToVector4(this->mono.color);
	float floatColor[4] = { preColor.x, preColor.y, preColor.z, preColor.w };
	ImGui::ColorEdit4((num + "color").c_str(), floatColor, 1);
	Vector4 vector4Color = { floatColor[0], floatColor[1], floatColor[2], floatColor[3] };
	this->mono.color = ConvertVector4ToUint(vector4Color);
	ImGui::Text("option");
	ImGui::Checkbox("Billboard", &this->isBillboard);

	if (ImGui::Button("save"))
	{
		this->mono.texture = Game::LoadTexture("resources/Prototypes/texture/uvChecker.png");
	}

	//ImGui::Text("Shape");
	//ImGui::Checkbox("toggleShape", &this->option.emitterShape);
	//ImGui::Checkbox("toggleInOut", &this->option.spawnInsideEmitter);
	//ImGui::DragFloat3("aabb.min	", &this->emitterAABB.min.x, 0.1f);
	//ImGui::DragFloat3("aabb.max	", &this->emitterAABB.max.x, 0.1f);
	//ImGui::DragFloat3("Sphere.center", &this->emitterSphere.center.x, 0.1f);
	//ImGui::DragFloat3("Sphere.radius", &this->emitterSphere.radius.x, 0.1f);
	//
	//ImGui::Text("InitializeTransforms");
	//ImGui::DragFloat3("scale	", &this->mono.transforms.scale.x, 0.1f);
	//ImGui::DragFloat3("rotate	", &this->mono.transforms.rotate.x, 0.1f);
	//ImGui::Text("AddTransforms/frame");
	//ImGui::DragFloat3("AddScale	", &this->AddScale.x, 0.1f);
	//ImGui::DragFloat3("AddRotate", &this->AddRotate.x, 0.1f);
	//ImGui::DragFloat("velocity	", &this->velocity, 0.01f);
	//ImGui::Checkbox("toggletarget", &this->option.targetDirection);
	//
	//ImGui::Text("direction");
	//ImGui::DragFloat3("target	", &this->target.x, 0.01f);
	//ImGui::DragInt("emissionDelay	", &this->emissionDelay);
	//ImGui::DragInt("particlesPerEmission	", &this->particlesPerEmission);

	ImGui::End();
}

void RenderData_Particle::DrawEmitter()
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

	RenderData_Line line;
	line.color = 0xFF0000FF;
	line.points.push_back(p[0]);
	line.points.push_back(p[1]);
	line.points.push_back(p[2]);
	line.points.push_back(p[3]);
	line.points.push_back(p[0]);

	line.points.push_back(p[4]);
	line.points.push_back(p[5]);
	line.points.push_back(p[6]);
	line.points.push_back(p[7]);
	line.points.push_back(p[4]);

	line.points.push_back(p[5]);
	line.points.push_back(p[1]);
	line.points.push_back(p[2]);
	line.points.push_back(p[6]);
	line.points.push_back(p[7]);
	line.points.push_back(p[3]);

	line.Draw();


	//// エミッターがAABB
	//if (this->option.emitterShape)
	//{
	//	Vector3 p[8];
	//	p[0] = { this->emitterAABB.min.x, this->emitterAABB.min.y, this->emitterAABB.min.z };
	//	p[1] = { this->emitterAABB.max.x, this->emitterAABB.min.y, this->emitterAABB.min.z };
	//	p[2] = { this->emitterAABB.max.x, this->emitterAABB.max.y, this->emitterAABB.min.z };
	//	p[3] = { this->emitterAABB.min.x, this->emitterAABB.max.y, this->emitterAABB.min.z };
	//	p[4] = { this->emitterAABB.min.x, this->emitterAABB.min.y, this->emitterAABB.max.z };
	//	p[5] = { this->emitterAABB.max.x, this->emitterAABB.min.y, this->emitterAABB.max.z };
	//	p[6] = { this->emitterAABB.max.x, this->emitterAABB.max.y, this->emitterAABB.max.z };
	//	p[7] = { this->emitterAABB.min.x, this->emitterAABB.max.y, this->emitterAABB.max.z };
	//
	//	RenderData_Line line;
	//	line.color = 0xFF0000FF;
	//	line.points.push_back(p[0]);
	//	line.points.push_back(p[1]);
	//	line.points.push_back(p[2]);
	//	line.points.push_back(p[3]);
	//	line.points.push_back(p[0]);
	//
	//	line.points.push_back(p[4]);
	//	line.points.push_back(p[5]);
	//	line.points.push_back(p[6]);
	//	line.points.push_back(p[7]);
	//	line.points.push_back(p[4]);
	//
	//	line.points.push_back(p[5]);
	//	line.points.push_back(p[1]);
	//	line.points.push_back(p[2]);
	//	line.points.push_back(p[6]);
	//	line.points.push_back(p[7]);
	//	line.points.push_back(p[3]);
	//
	//	line.Draw();
	//}
	//// 
	//else
	//{
	//	Transforms transforms;
	//	transforms.scale = this->emitterSphere.radius;
	//	DrawOptions option;
	//
	//	//engine->DrawSphere(transforms, this->emitterSphere.center, 12, 0, 0xFFFFFF22, option);
	//}
}

#pragma endregion