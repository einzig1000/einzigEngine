#include "DrawSystem/RenderData/RenderData.h"
#include "Utilities/JsonManager.h"
#include "Game.h"
#include "Engine.h"
using namespace DirectX;

std::vector<RenderData_Model*> RenderData_Model::renderModels;
std::vector<CollisionInf*> RenderData_Model::collisionInfos;
std::vector<RenderData_Sprite*> RenderData_Sprite::renderSprites;
std::vector<RenderData_Triangle*> RenderData_Triangle::renderTriangles;
std::vector<RenderData_Line*> RenderData_Line::renderLines;
std::vector<RenderData_Particle*> RenderData_Particle::renderParticles;

#pragma region model

RenderData_Model::RenderData_Model()
{
	renderModels.push_back(this);
	this->ID = int(renderModels.size());
	this->parentModel = nullptr;
}

RenderData_Model::~RenderData_Model()
{
	auto it = std::find(renderModels.begin(), renderModels.end(), this);
	if (it != renderModels.end())
	{
		renderModels.erase(it);
	}
}

// 他のオブジェクトとの衝突判定
bool RenderData_Model::isCollision(RenderData_Model& target) const
{
	for (const auto& aabb1 : target.aabbs)
	{
		for (const auto& aabb2 : this->aabbs)
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
	return this->worldMatrix;
}
Vector3 RenderData_Model::GetWorldPosition() const
{
	return worldPos;
}

// 任意のポイントを向く
void RenderData_Model::LookAtOnce(const Vector3& targetWorldPos, float roll)
{
	Vector3 direction = (targetWorldPos - this->GetWorldPosition()).Normalized();
	float yaw = std::atan2(direction.x, direction.z); // Y軸回り
	float pitch = std::asin(-direction.y);            // X軸回り

	rotate.value = { pitch, yaw, roll };
}
void RenderData_Model::LookAtOnce(const RenderData_Model& other, float roll)
{
	LookAtOnce(other.GetWorldPosition(), roll);
}
void RenderData_Model::LookAtCamera(float roll)
{
	LookAtOnce(Game::Camera::Getter::GetTranslate("ReleaseCamera"), roll);
}
void RenderData_Model::LookAtFront(float roll)
{
	Vector3 frontPos = this->GetWorldPosition() + Vector3{ 0.0f, 0.0f, 1.0f };
	LookAtOnce(frontPos, roll);
}

void RenderData_Model::Draw()
{
	Engine::Instance().DrawModel(*this);
}

void RenderData_Model::DrawAABB()
{
	for (size_t i = 0; i < this->aabbs.size(); ++i)
	{
		Game::DebugDraw::AddAABB(this->aabbs[i], 0xFF0000FF);
	}
}

void RenderData_Model::DrawImGui()
{
	std::optional<std::string> str = "model : " + std::to_string(this->ID);
	if (this->name != std::nullopt) str = (this->name);

	std::string num = std::to_string(this->ID) + ":";

	ImGui::Begin(str->c_str());

	if (ImGui::TreeNode("----------transforms-----------"))
	{
		std::string dragId;

		ImGui::Text("Scale");
		dragId = std::string("##scale.value") + num;
		ImGui::Text("Val"); ImGui::SameLine();
		ImGui::DragFloat3(dragId.c_str(), &scale.value.x, 0.01f);
		dragId = std::string("##scale.velocity") + num;
		ImGui::Text("Vel"); ImGui::SameLine();
		ImGui::DragFloat3(dragId.c_str(), &scale.velocity.x, 0.01f);
		dragId = std::string("##scale.acceleration") + num;
		ImGui::Text("Acc"); ImGui::SameLine();
		ImGui::DragFloat3(dragId.c_str(), &scale.acceleration.x, 0.01f);

		ImGui::Text("translate");
		dragId = std::string("##translate.value") + num;
		ImGui::Text("Val"); ImGui::SameLine();
		ImGui::DragFloat3(dragId.c_str(), &translate.value.x, 0.01f);
		dragId = std::string("##translate.velocity") + num;
		ImGui::Text("Vel"); ImGui::SameLine();
		ImGui::DragFloat3(dragId.c_str(), &translate.velocity.x, 0.01f);
		dragId = std::string("##translate.acceleration") + num;
		ImGui::Text("Acc"); ImGui::SameLine();
		ImGui::DragFloat3(dragId.c_str(), &translate.acceleration.x, 0.01f);

		ImGui::Text("rotate");
		dragId = std::string("##rotate.value") + num;
		ImGui::Text("Val"); ImGui::SameLine();
		ImGui::DragFloat3(dragId.c_str(), &rotate.value.x, 0.01f);
		dragId = std::string("##rotate.velocity") + num;
		ImGui::Text("Vel"); ImGui::SameLine();
		ImGui::DragFloat3(dragId.c_str(), &rotate.velocity.x, 0.01f);
		dragId = std::string("##rotate.acceleration") + num;
		ImGui::Text("Acc"); ImGui::SameLine();
		ImGui::DragFloat3(dragId.c_str(), &rotate.acceleration.x, 0.01f);

		dragId = std::string("##pivot") + num;
		ImGui::Text("Pivot");
		ImGui::DragFloat3(dragId.c_str(), &pivot.x, 0.01f);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("----------uvTransforms---------"))
	{
		ImGui::DragFloat3((num + "UVscale").c_str(), &uvTransform.scale.x, 0.01f);
		ImGui::DragFloat3((num + "UVtranslate").c_str(), &uvTransform.translate.x, 0.01f);
		ImGui::DragFloat3((num + "UVrotate").c_str(), &uvTransform.rotate.x, 0.01f);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("----------texture--------------"))
	{
		size_t textureCount = Game::Resource::GetTextureCount();

		for (size_t i = 0; i < textureCount; ++i)
		{
			TextureData* texData = Game::Resource::GetTexture(static_cast<uint32_t>(i));
			if (texData)
			{
				ImGui::Image((ImTextureID)texData->textureSrvHandleGPU.ptr, ImVec2(32, 32));
				
				// 6個並べたら改行
				if ((i + 1) % 6 != 0 && i < textureCount - 1)
				{
					ImGui::SameLine();
				}
				if (ImGui::IsItemClicked())
				{
					this->texture = static_cast<uint32_t>(i);
				}
			}
		}

		ImGui::TreePop();
	}
	if (ImGui::TreeNode("----------model----------------"))
	{
		if (ImGui::Button("-"))this->model -= 1;

		ImGui::SameLine();

		// ラベルを非表示にするために "##" プレフィックスで ID を与える
		std::string dragId = std::string("##model") + num;
		ImGui::DragInt(dragId.c_str(), reinterpret_cast<int*>(&this->model));

		ImGui::SameLine();

		if (ImGui::Button("+"))this->model += 1;

		// クランプ
		if (this->model < 0) this->model = 0;
		if (this->model > int(Game::Resource::GetModelCount() - 1)) this->model = int(Game::Resource::GetModelCount() - 1);

		ImGui::TreePop();
	}
	if (ImGui::TreeNode("----------color----------------"))
	{
		float floatColor[4] = { this->color.x / 255.0f,  this->color.y / 255.0f,  this->color.z / 255.0f,  this->color.w / 255.0f };
		ImGui::ColorEdit4((num + "color").c_str(), floatColor, 1);
		color = { floatColor[0] * 255.0f, floatColor[1] * 255.0f, floatColor[2] * 255.0f, floatColor[3] * 255.0f };
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("----------blendMode------------"))
	{
		// 表示名を実際のモードに合わせる
		static const char* items[] = { "None", "Normal", "Add", "Sub", "Mul", "Screen", "Wireframe" };
		int current_item = static_cast<int>(options.blendMode);
		if (ImGui::Combo((num + "blendMode").c_str(), &current_item, items, IM_ARRAYSIZE(items)))
		{
			options.blendMode = static_cast<BlendMode>(current_item);
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("----------Light----------------"))
	{
		ImGui::Checkbox("useOwnLight", &options.useOwnLight);
		if (options.useOwnLight)
		{
			ImGui::ColorEdit4((num + "dirLight.color").c_str(), &options.dirLight.color.x, 1);
			ImGui::DragFloat3((num + "dirLight.direction").c_str(), &options.dirLight.direction.x, 0.01f, -1.0f, 1.0f);
			ImGui::DragFloat((num + "dirLight.intensity").c_str(), &options.dirLight.intensity, 0.01f, 0.0f, 1.0f);

			// 表示名を実際のモードに合わせる
			static const char* items[] = { "None", "Lambert", "HalfLambert" };
			int current_item = static_cast<int>(options.dirLight.mode);
			if (ImGui::Combo((num + "dirLight.mode").c_str(), &current_item, items, IM_ARRAYSIZE(items)))
			{
				options.dirLight.mode = static_cast<LightMode>(current_item);
			}

			ImGui::Checkbox((num + "dirLight.phong").c_str(), &options.dirLight.phong);
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("----------option---------------"))
	{
		ImGui::Checkbox("wireFrame", &options.wireframe);
		bool lookAt = false;
		ImGui::Checkbox("lookAt", &lookAt);
		if (lookAt)
		{
			Game::Camera::MoveCameraCenter(GetWorldPosition(), 0, EaseType::IN_BACK);
		}
		ImGui::Text("isCollisionMouse : %d", isCollisionMouseRay);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("----------load & save----------"))
	{
		char buf[256];
		if (this->filePath.size() < sizeof(buf)) memcpy(buf, this->filePath.c_str(), this->filePath.size() + 1);
		else buf[sizeof(buf) - 1] = '\0';
		if (ImGui::InputText(".json", buf, sizeof(buf)))
		{
			this->filePath = std::string(buf);
		}
		if (ImGui::Button("save"))
		{
			JsonManager::SaveToJson(*this, this->filePath);
		}
		ImGui::SameLine();
		if (ImGui::Button("load"))
		{
			JsonManager::LoadFromJson(*this, this->filePath);
		}
		ImGui::TreePop();
	}

	ImGui::End();
}

// 全オブジェクトのSRT更新,それに伴うワールド行列更新
void RenderData_Model::Update1()
{
	// S/R/Tの更新
	this->translate.velocity += this->translate.acceleration;
	this->translate.value += this->translate.velocity;
	this->rotate.velocity += this->rotate.acceleration;
	this->rotate.value += this->rotate.velocity;
	this->scale.velocity += this->scale.acceleration;
	this->scale.value += this->scale.velocity;

	// 今フレームでS/R/Tに変化があったか
	this->movedThisFrame =
		(this->translate.value != this->preTranslate.value) ||
		(this->rotate.value != this->preRotate.value) ||
		(this->scale.value != this->preScale.value) ||
		!this->initialized;

	// ワールド座標取得
	if (this->movedThisFrame)
	{
		// 移動マトリックス作成
		XMVECTOR scaleVec = XMVectorSet(this->scale.value.x, this->scale.value.y, this->scale.value.z, 0.0f);
		XMVECTOR pivotVec = XMVectorSet(this->pivot.x, this->pivot.y, this->pivot.z, 0.0f);
		XMVECTOR translateVec = XMVectorSet(this->translate.value.x, this->translate.value.y, this->translate.value.z, 0.0f);
		XMVECTOR rotEuler = XMVectorSet(this->rotate.value.x, this->rotate.value.y, this->rotate.value.z, 0.0f);
		// 1) スケール
		XMMATRIX S = XMMatrixScalingFromVector(scaleVec);
		// 2) ピボットオフセット（原点に戻す方）
		XMMATRIX Tneg = XMMatrixTranslationFromVector(XMVectorNegate(pivotVec));
		// 3) 回転（オイラー→クォータニオン→行列）
		XMVECTOR quatEuler = XMQuaternionRotationRollPitchYawFromVector(rotEuler);
		XMMATRIX R = XMMatrixRotationQuaternion(quatEuler);
		// 4) ピボットオフセット（もとの位置に戻す方）
		XMMATRIX Tpos = XMMatrixTranslationFromVector(pivotVec);
		// 5) 平行移動
		XMMATRIX T = XMMatrixTranslationFromVector(translateVec);
		// 6) 合成: S → Tneg → R → Tpos → T
		XMMATRIX world = S * Tneg * R * Tpos * T;
		// 8) 結果を transforms.World に格納
		XMFLOAT4X4 tmp;
		DirectX::XMStoreFloat4x4(&tmp, world);
		for (int i = 0; i < 4; ++i)
			for (int j = 0; j < 4; ++j)
				this->localWorldMatrix.m[i][j] = tmp.m[i][j];

	}

	initialized = true;
}

// 全オブジェクトの親を考慮したワールド行列更新,AABB更新
void RenderData_Model::Update2()
{
	// 階層を含めた最終ワールド行列を取得
	this->worldMatrix = this->SetWorldMatrix();

	// ワールド座標取得
	this->worldPos = Vector3(
		this->worldMatrix.m[3][0],
		this->worldMatrix.m[3][1],
		this->worldMatrix.m[3][2]
	);

	// AABB更新（最終ワールド行列に基づいて）
	this->aabbs = Engine::Instance().CreateAABB(this);
}

Matrix4x4 RenderData_Model::SetWorldMatrix()
{
	// 自身のローカル行列がベース
	Matrix4x4 result = this->localWorldMatrix;

	// 親が存在する場合は親の親の親...をウルトラ再帰する
	if (this->parentModel)
	{
		Matrix4x4 parentWorld = this->parentModel->SetWorldMatrix();
		result = parentWorld * result;
	}

	// 親がいなかったらそのまま、いたら親の行列を掛けたものを返す
	return result;
}

// 衝突判定,衝突ペア・深度の保存
void RenderData_Model::Update3()
{
	for (auto& blockTarget : blockList)
	{
		std::optional<CollisionInf> collisionInf = this->isCollisionAABBInf(*blockTarget);
		if (collisionInf)
		{
			collisionInfos.push_back(new CollisionInf(*collisionInf));
		}
	}
}

// 衝突時の更新,それに伴うワールド行列更新
void RenderData_Model::Update4()
{
	// collisionInfosを探査して、自分が軽い方のオブジェクトとして衝突しているものを処理
	for (const auto& collisionInfPtr : collisionInfos)
	{
		const CollisionInf& collisionInf = *collisionInfPtr;
		if (collisionInf.IDpair.light == this->ID)
		{
			/// 衝突したAABB面のペア・深度がわかっているので、それに基づいて移動
			if (collisionInf.face.light == AABBFace::LEFT)
			{
				this->translate.value.x += collisionInf.depth.x;
			}
			else if (collisionInf.face.light == AABBFace::RIGHT)
			{
				this->translate.value.x -= collisionInf.depth.x;
			}
			else if (collisionInf.face.light == AABBFace::BOTTOM)
			{
				this->translate.value.y += collisionInf.depth.y;
			}
			else if (collisionInf.face.light == AABBFace::TOP)
			{
				this->translate.value.y -= collisionInf.depth.y;
			}
			else if (collisionInf.face.light == AABBFace::BACK)
			{
				this->translate.value.z += collisionInf.depth.z;
			}
			else if (collisionInf.face.light == AABBFace::FRONT)
			{
				this->translate.value.z -= collisionInf.depth.z;
			}

			// 衝突しているのにその方向に加速度がかかっている場合はこのフレームで加速した分を打ち消す
			if (collisionInf.face.light == AABBFace::LEFT && this->translate.acceleration.x < 0.0f)
			{
				this->translate.velocity.x -= this->translate.acceleration.x;
				//this->translate.acceleration.x = 0.0f;
			}
			else if (collisionInf.face.light == AABBFace::RIGHT && this->translate.acceleration.x > 0.0f)
			{
				this->translate.velocity.x -= this->translate.acceleration.x;
				//this->translate.acceleration.x = 0.0f;
			}
			else if (collisionInf.face.light == AABBFace::BOTTOM && this->translate.acceleration.y < 0.0f)
			{
				this->translate.velocity.y -= this->translate.acceleration.y;
				//this->translate.acceleration.y = 0.0f;
			}
			else if (collisionInf.face.light == AABBFace::TOP && this->translate.acceleration.y > 0.0f)
			{
				this->translate.velocity.y -= this->translate.acceleration.y;
				//this->translate.acceleration.y = 0.0f;
			}
			else if (collisionInf.face.light == AABBFace::BACK && this->translate.acceleration.z < 0.0f)
			{
				this->translate.velocity.z -= this->translate.acceleration.z;
				//this->translate.acceleration.z = 0.0f;
			}
			else if (collisionInf.face.light == AABBFace::FRONT && this->translate.acceleration.z > 0.0f)
			{
				this->translate.velocity.z -= this->translate.acceleration.z;
				//this->translate.acceleration.z = 0.0f;
			}

			// translate.velocityの分めり込んだ状態で固定されてしまうので、velocity分座標を戻す。velocityは変えない。
			this->translate.value -= this->translate.velocity;

			// ワールド行列更新
			XMVECTOR scaleVec = XMVectorSet(this->scale.value.x, this->scale.value.y, this->scale.value.z, 0.0f);
			XMVECTOR pivotVec = XMVectorSet(this->pivot.x, this->pivot.y, this->pivot.z, 0.0f);
			XMVECTOR translateVec = XMVectorSet(this->translate.value.x, this->translate.value.y, this->translate.value.z, 0.0f);
			XMVECTOR rotEuler = XMVectorSet(this->rotate.value.x, this->rotate.value.y, this->rotate.value.z, 0.0f);
			// 1) スケール
			XMMATRIX S = XMMatrixScalingFromVector(scaleVec);
			// 2) ピボットオフセット（原点に戻す方）
			XMMATRIX Tneg = XMMatrixTranslationFromVector(XMVectorNegate(pivotVec));
			// 3) 回転（オイラー→クォータニオン→行列）
			XMVECTOR quatEuler = XMQuaternionRotationRollPitchYawFromVector(rotEuler);
			XMMATRIX R = XMMatrixRotationQuaternion(quatEuler);
			// 4) ピボットオフセット（もとの位置に戻す方）
			XMMATRIX Tpos = XMMatrixTranslationFromVector(pivotVec);
			// 5) 平行移動
			XMMATRIX T = XMMatrixTranslationFromVector(translateVec);
			// 6) 合成: S → Tneg → R → Tpos → T
			XMMATRIX world = S * Tneg * R * Tpos * T;
			// 8) 結果を transforms.World に格納
			XMFLOAT4X4 tmp;
			DirectX::XMStoreFloat4x4(&tmp, world);
			for (int i = 0; i < 4; ++i)
				for (int j = 0; j < 4;
					++j)
					this->localWorldMatrix.m[i][j] = tmp.m[i][j];
		}
	}
}

// 全オブジェクトの描画範囲内判定,前フレーム情報保存
void RenderData_Model::Update5()
{
	collisionInfos.clear();

#pragma region 描画範囲内判定

	bool inFrustum = false;
	for (const auto& aabb : this->aabbs)
	{
		if (Game::Camera::InCamera(aabb))
		{
			inFrustum = true;
			break;
		}
	}
	this->inPicture = inFrustum;

#pragma endregion

#pragma region 前フレーム情報保存

	this->preScale = this->scale;
	this->preTranslate = this->translate;
	this->preRotate = this->rotate;
	this->preAABB = this->aabbs;

#pragma endregion

}

//std::optional<CollisionInf> RenderData_Model::isCollisionAABBInf(RenderData_Model& target) const
//{
//	CollisionInf result;
//	// 衝突しているオブジェクトIDの保存（軽い方・重い方）
//	result.IDpair.light = target.ID;
//	result.IDpair.heavy = this->ID;
//	if (this->mass < target.mass)
//	{
//		std::swap(result.IDpair.light, result.IDpair.heavy);
//	}
//	result.AABBpair = CollisionPair{ -1, -1 };
//
//	// AABB同士の衝突判定
//	for (size_t i = 0; i < target.aabbs.size(); ++i)
//	{
//		for (size_t j = 0; j < this->aabbs.size(); ++j)
//		{
//			// 緩めの衝突判定
//			if (IsLooseCollision(target.aabbs[i], this->aabbs[j], 0.001f))
//			{
//				// 衝突深度取得
//				Vector3 depth = this->aabbs[j].GetCollisionDepth(target.aabbs[i]);
//
//				// 衝突ペア・深度の保存
//				result.AABBpair = CollisionPair{ static_cast<int>(i), static_cast<int>(j) };
//				result.depth = -depth;
//				if (this->mass < target.mass)
//				{
//					std::swap(result.AABBpair.light, result.AABBpair.heavy);
//					result.depth = depth;
//				}
//				break;
//			}
//		}
//	}
//	if (result.AABBpair == CollisionPair{ -1, -1 }) return std::nullopt;
//	return result;
//}

std::optional<CollisionInf> RenderData_Model::isCollisionAABBInf(RenderData_Model& target) const {
	CollisionInf best;
	bool found = false;
	float bestPen = std::numeric_limits<float>::infinity();

	for (size_t i = 0; i < target.aabbs.size(); ++i)
	{ 
		// AABB中心点取得
		Vector3 centerT = target.aabbs[i].center();
		for (size_t j = 0; j < this->aabbs.size(); ++j) 
		{
			// AABB中心点取得
			Vector3 centerS = this->aabbs[j].center();

			// 衝突判定　衝突していなければスキップ
			if (!IsLooseCollision(target.aabbs[i], this->aabbs[j], 0.001f)) continue;

			// 深度取得
			Vector3 overlap = this->aabbs[j].GetCollisionDepth(target.aabbs[i]);


			// 一番浅い軸を探す
			float pen = overlap.x;
			int axis = 0; // 0:x, 1:y, 2:z
			if (overlap.y < pen) { pen = overlap.y; axis = 1; }
			if (overlap.z < pen) { pen = overlap.z; axis = 2; }

			// 衝突面の特定
			CollisionAABBFace faces{ AABBFace::NONE, AABBFace::NONE };

			if (axis == 0) { // x
				if (centerT.x < centerS.x) { faces.light = AABBFace::RIGHT; faces.heavy = AABBFace::LEFT; }
				else { faces.light = AABBFace::LEFT; faces.heavy = AABBFace::RIGHT; }
			}
			else if (axis == 1) { // y
				if (centerT.y < centerS.y) { faces.light = AABBFace::TOP; faces.heavy = AABBFace::BOTTOM; }
				else { faces.light = AABBFace::BOTTOM; faces.heavy = AABBFace::TOP; }
			}
			else { // z
				if (centerT.z < centerS.z) { faces.light = AABBFace::FRONT; faces.heavy = AABBFace::BACK; }
				else { faces.light = AABBFace::BACK; faces.heavy = AABBFace::FRONT; }
			}

			// 最小貫通深度の更新
			if (pen < bestPen) {
				bestPen = pen;
				found = true;
				best.AABBpair = CollisionPair{ static_cast<int>(i), static_cast<int>(j) };
				best.depth = overlap;
				best.face = faces;
			}
		}
	}

	// 衝突ペアが見つからなかった場合
	if (!found) return std::nullopt;

	// 衝突しているオブジェクトIDの保存
	best.IDpair.light = target.ID;
	best.IDpair.heavy = this->ID;

	// 軽い方・重い方の入れ替え
	if (this->mass < target.mass) {
		std::swap(best.IDpair.light, best.IDpair.heavy);
		std::swap(best.AABBpair.light, best.AABBpair.heavy);
		std::swap(best.face.light, best.face.heavy);
	}

	return best;
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
	Engine::Instance().DrawSprite(*this);
}

void RenderData_Sprite::DrawImGui()
{
	std::optional<std::string> str = "NULL";
	if (this->name != std::nullopt) str = (this->name);
	else str = "sprite : " + std::to_string(this->ID);

	std::string num = std::to_string(this->ID) + ":";

	ImGui::Begin(str->c_str());

	if (ImGui::TreeNode("----------transforms-----------"))
	{
		ImGui::DragFloat2((num + "scale").c_str(), &transforms.scale.x, 0.01f);
		ImGui::DragFloat2((num + "translate").c_str(), &transforms.translate.x, 1.0f);
		ImGui::DragFloat((num + "rotate").c_str(), &transforms.rotate.z, 0.01f);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("----------uvTransforms---------"))
	{
		ImGui::DragFloat2((num + "UVscale").c_str(), &uvTransform.scale.x, 0.01f);
		ImGui::DragFloat2((num + "UVtranslate").c_str(), &uvTransform.translate.x, 0.01f);
		ImGui::DragFloat((num + "UVrotate").c_str(), &uvTransform.rotate.z, 0.01f);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("----------cutImage-------------"))
	{
		ImGui::DragInt2((num + "cutImageLeftTop").c_str(), &cutImageLeftTop.x, 1);
		ImGui::DragInt2((num + "cutImageSize").c_str(), &cutImageSize.x, 1);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("----------pivot----------------"))
	{
		ImGui::DragFloat2((num + "pivot").c_str(), &pivot.x, 0.1f);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("----------color----------------"))
	{
		Vector4 preColor = ConvertUintToVector4(color);
		float floatColor[4] = { preColor.x, preColor.y, preColor.z, preColor.w };
		ImGui::ColorEdit4((num + "color").c_str(), floatColor, 1);
		Vector4 vector4Color = { floatColor[0], floatColor[1], floatColor[2], floatColor[3] };
		color = ConvertVector4ToUint(vector4Color);

		ImGui::TreePop();
	}
	if (ImGui::TreeNode("----------texture--------------"))
	{
		size_t textureCount = Game::Resource::GetTextureCount();

		for (size_t i = 0; i < textureCount; ++i)
		{
			TextureData* texData = Game::Resource::GetTexture(static_cast<uint32_t>(i));
			if (texData)
			{
				ImGui::Image((ImTextureID)texData->textureSrvHandleGPU.ptr, ImVec2(32, 32));

				// 6個並べたら改行
				if ((i + 1) % 6 != 0 && i < textureCount - 1)
				{
					ImGui::SameLine();
				}
				if (ImGui::IsItemClicked())
				{
					this->texture = static_cast<uint32_t>(i);
				}
			}
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("----------Anchor---------------"))
	{
		const char* items[] =
		{ "Center","CenterLeft","CenterRight","CenterTop","CenterDown","LeftTop","RightTop","LeftDown","RightDown" };
		int item_current = int(this->anchor);
		ImGui::Combo((num + "Anchor").c_str(), &item_current, items, IM_ARRAYSIZE(items));
		{
			if (item_current == 0)anchor = Anchor::Center;
			if (item_current == 1)anchor = Anchor::CenterLeft;
			if (item_current == 2)anchor = Anchor::CenterRight;
			if (item_current == 3)anchor = Anchor::CenterTop;
			if (item_current == 4)anchor = Anchor::CenterDown;
			if (item_current == 5)anchor = Anchor::LeftTop;
			if (item_current == 6)anchor = Anchor::RightTop;
			if (item_current == 7)anchor = Anchor::LeftDown;
			if (item_current == 8)anchor = Anchor::RightDown;
		}

		ImGui::TreePop();
	}
	if (ImGui::TreeNode("----------option---------------"))
	{
		if (isCollisionMouseRay)ImGui::Text("isCollisionMouse : true");
		else ImGui::Text("isCollisionMouse : false");

		ImGui::TreePop();
	}
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
	Engine::Instance().DrawTriangle(*this);
}

void RenderData_Triangle::DrawImGui()
{
	std::optional<std::string> str = "triangle : " + std::to_string(this->ID);
	if (this->name != std::nullopt) str = (this->name);

	std::string num = std::to_string(this->ID) + ":";

	ImGui::Begin(str->c_str());

	if (ImGui::TreeNode("----------transforms-----------"))
	{
		ImGui::DragFloat3((num + "scale").c_str(), &transforms.scale.x, 0.01f);
		ImGui::DragFloat3((num + "translate").c_str(), &transforms.translate.x, 1.0f);
		ImGui::DragFloat3((num + "rotate").c_str(), &transforms.rotate.x, 0.01f);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("----------uvTransforms---------"))
	{
		ImGui::DragFloat3((num + "UVscale").c_str(), &uvTransform.scale.x, 0.01f);
		ImGui::DragFloat3((num + "UVtranslate").c_str(), &uvTransform.translate.x, 0.01f);
		ImGui::DragFloat((num + "UVrotate").c_str(), &uvTransform.rotate.z, 0.01f);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("----------position-------------"))
	{
		ImGui::DragFloat3((num + "pos1").c_str(), &pos1.x, 0.1f);
		ImGui::DragFloat3((num + "pos2").c_str(), &pos2.x, 0.1f);
		ImGui::DragFloat3((num + "pos3").c_str(), &pos3.x, 0.1f);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("----------texture--------------"))
	{
		for (size_t i = 0; i < Game::Resource::GetTextureCount(); ++i)
		{
			TextureData* texData = Game::Resource::GetTexture(static_cast<uint32_t>(i));
			if (texData)
			{
				ImGui::Image((ImTextureID)texData->textureSrvHandleGPU.ptr, ImVec2(32, 32));

				// 6個並べたら改行
				if ((i + 1) % 6 != 0 && i < Game::Resource::GetTextureCount() - 1)
				{
					ImGui::SameLine();
				}
				if (ImGui::IsItemClicked())
				{
					this->texture = static_cast<uint32_t>(i);
				}
			}
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("----------pivot----------------"))
	{
		//ImGui::DragFloat2((num + "pivot").c_str(), &pivot.x, 0.1f);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("----------color----------------"))
	{
		Vector4 preColor = ConvertUintToVector4(color);
		float floatColor[4] = { preColor.x, preColor.y, preColor.z, preColor.w };
		ImGui::ColorEdit4((num + "color").c_str(), floatColor, 1);
		Vector4 vector4Color = { floatColor[0], floatColor[1], floatColor[2], floatColor[3] };
		color = ConvertVector4ToUint(vector4Color);
		ImGui::TreePop();
	}

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
	Engine::Instance().DrawLine(*this);
}

void RenderData_Line::DrawPoints()
{


}

void RenderData_Line::DrawImGui()
{
	std::optional<std::string> str = "line : " + std::to_string(this->ID);
	if (this->name != std::nullopt) str = (this->name);

	std::string num = std::to_string(this->ID) + ":";

	ImGui::Begin(str->c_str());

	if (ImGui::TreeNode("----------color----------------"))
	{
		Vector4 preColor = ConvertUintToVector4(color);
		float floatColor[4] = { preColor.x, preColor.y, preColor.z, preColor.w };
		ImGui::ColorEdit4((num + " : color").c_str(), floatColor, 1);
		Vector4 vector4Color = { floatColor[0], floatColor[1], floatColor[2], floatColor[3] };
		color = ConvertVector4ToUint(vector4Color);

		ImGui::TreePop();
	}
	if (ImGui::TreeNode("----------LineType-------------"))
	{
		const char* items[] =
		{ "Line","BezierCurve", "SplineCurve" };
		int item_current = int(this->lineType);
		ImGui::Combo((num + "LineType").c_str(), &item_current, items, IM_ARRAYSIZE(items));
		{
			if (item_current == 0)lineType = LineType::Line;
			if (item_current == 1)lineType = LineType::BezierCurve;
			if (item_current == 2)lineType = LineType::SplineCurve;
		}

		ImGui::TreePop();
	}
	if (ImGui::TreeNode("----------kSubdivision---------"))
	{
		ImGui::DragInt((num + " : kSubdivision").c_str(), (int*)&kSubdivision, 1, 1, 100);

		ImGui::TreePop();
	}
	if (ImGui::TreeNode("----------points---------------"))
	{
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

		ImGui::TreePop();
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

bool RenderData_Particle::LoadJson()
{
	return JsonManager::LoadFromJson(*this, this->filePath);
}

void RenderData_Particle::Draw()
{
	Engine::Instance().DrawParticle(*this);
}

void RenderData_Particle::DrawImGui()
{
	std::optional<std::string> str = "particle : " + std::to_string(this->ID);
	if (this->name != std::nullopt) str = (this->name);

	std::string num = ":" + std::to_string(this->ID);

	ImGui::Begin(str->c_str());

	if (ImGui::TreeNode("----------Emitter--------------"))
	{
		ImGui::Checkbox(("ToggleShape" + num).c_str(), &this->GetParticleInf().emitter.useSphereEmitter);
		ImGui::Checkbox(("emitFromInside" + num).c_str(), &this->GetParticleInf().emitter.emitFromInside);
		if (this->GetParticleInf().emitter.useSphereEmitter)
		{
			ImGui::DragFloat3(("Sphere.radius" + num).c_str(), &this->GetParticleInf().emitter.emitterSphere.radius.x, 0.1f);
			ImGui::DragFloat3(("Sphere.center" + num).c_str(), &this->GetParticleInf().emitter.emitterSphere.center.x, 0.1f);
		}
		else
		{
			ImGui::DragFloat3(("AABB.min" + num).c_str(), &this->GetParticleInf().emitter.emitterAABB.min.x, 0.1f);
			ImGui::DragFloat3(("AABB.max" + num).c_str(), &this->GetParticleInf().emitter.emitterAABB.max.x, 0.1f);
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("----------scale----------------"))
	{
		ImGui::Checkbox(("S.isRandom" + num).c_str(), &this->GetParticleInf().scale.isRandom_value);
		if (this->GetParticleInf().scale.isRandom_value)
		{
			ImGui::DragFloat3(("S.min" + num).c_str(), &this->GetParticleInf().scale.randomRange_value.min.x, 0.01f);
			ImGui::DragFloat3(("S.max" + num).c_str(), &this->GetParticleInf().scale.randomRange_value.max.x, 0.01f);
		}
		else
		{
			ImGui::DragFloat3(("S.val" + num).c_str(), &this->GetParticleInf().scale.value.x, 0.01f);
		}
		ImGui::Checkbox(("S.vel.isRandom" + num).c_str(), &this->GetParticleInf().scale.isRandom_velocity);
		if (this->GetParticleInf().scale.isRandom_velocity)
		{
			ImGui::DragFloat3(("S.vel.min" + num).c_str(), &this->GetParticleInf().scale.randomRange_velocity.min.x, 0.01f);
			ImGui::DragFloat3(("S.vel.max" + num).c_str(), &this->GetParticleInf().scale.randomRange_velocity.max.x, 0.01f);
		}
		else
		{
			ImGui::DragFloat3(("S.vel" + num).c_str(), &this->GetParticleInf().scale.velocity.x, 0.01f);
		}
		ImGui::Checkbox(("S.acc.isRandom" + num).c_str(), &this->GetParticleInf().scale.isRandom_acceleration);
		if (this->GetParticleInf().scale.isRandom_acceleration)
		{
			ImGui::DragFloat3(("S.acc.min" + num).c_str(), &this->GetParticleInf().scale.randomRange_acceleration.min.x, 0.01f);
			ImGui::DragFloat3(("S.acc.max" + num).c_str(), &this->GetParticleInf().scale.randomRange_acceleration.max.x, 0.01f);
		}
		else
		{
			ImGui::DragFloat3(("S.acc" + num).c_str(), &this->GetParticleInf().scale.acceleration.x, 0.01f);
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("----------rotate---------------"))
	{
		ImGui::Checkbox(("R.isRandom" + num).c_str(), &this->GetParticleInf().rotate.isRandom_value);
		if (this->GetParticleInf().rotate.isRandom_value)
		{
			ImGui::DragFloat3(("R.min" + num).c_str(), &this->GetParticleInf().rotate.randomRange_value.min.x, 0.01f);
			ImGui::DragFloat3(("R.max" + num).c_str(), &this->GetParticleInf().rotate.randomRange_value.max.x, 0.01f);
		}
		else
		{
			ImGui::DragFloat3(("R.val" + num).c_str(), &this->GetParticleInf().rotate.value.x, 0.01f);
		}
		ImGui::Checkbox(("R.vel.isRandom" + num).c_str(), &this->GetParticleInf().rotate.isRandom_velocity);
		if (this->GetParticleInf().rotate.isRandom_velocity)
		{
			ImGui::DragFloat3(("R.vel.min" + num).c_str(), &this->GetParticleInf().rotate.randomRange_velocity.min.x, 0.01f);
			ImGui::DragFloat3(("R.vel.max" + num).c_str(), &this->GetParticleInf().rotate.randomRange_velocity.max.x, 0.01f);
		}
		else
		{
			ImGui::DragFloat3(("R.vel" + num).c_str(), &this->GetParticleInf().rotate.velocity.x, 0.01f);
		}
		ImGui::Checkbox(("R.acc.isRandom" + num).c_str(), &this->GetParticleInf().rotate.isRandom_acceleration);
		if (this->GetParticleInf().rotate.isRandom_acceleration)
		{
			ImGui::DragFloat3(("R.acc.min" + num).c_str(), &this->GetParticleInf().rotate.randomRange_acceleration.min.x, 0.01f);
			ImGui::DragFloat3(("R.acc.max" + num).c_str(), &this->GetParticleInf().rotate.randomRange_acceleration.max.x, 0.01f);
		}
		else
		{
			ImGui::DragFloat3(("R.acc" + num).c_str(), &this->GetParticleInf().rotate.acceleration.x, 0.01f);
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("----------translate------------"))
	{
		//ImGui::DragFloat3(("T.val" + num).c_str(), &this->translate.value.x, 0.01f);
		ImGui::Checkbox(("T.vel.isRandom" + num).c_str(), &this->GetParticleInf().translate.isRandom_velocity);
		if (this->GetParticleInf().translate.isRandom_velocity)
		{
			ImGui::DragFloat3(("T.vel.min" + num).c_str(), &this->GetParticleInf().translate.randomRange_velocity.min.x, 0.01f);
			ImGui::DragFloat3(("T.vel.max" + num).c_str(), &this->GetParticleInf().translate.randomRange_velocity.max.x, 0.01f);
		}
		else
		{
			ImGui::DragFloat3(("T.vel" + num).c_str(), &this->GetParticleInf().translate.velocity.x, 0.01f);
		}
		ImGui::Checkbox(("T.acc.isRandom" + num).c_str(), &this->GetParticleInf().translate.isRandom_acceleration);
		if (this->GetParticleInf().translate.isRandom_acceleration)
		{
			ImGui::DragFloat3(("T.acc.min" + num).c_str(), &this->GetParticleInf().translate.randomRange_acceleration.min.x, 0.01f);
			ImGui::DragFloat3(("T.acc.max" + num).c_str(), &this->GetParticleInf().translate.randomRange_acceleration.max.x, 0.01f);
		}
		else
		{
			ImGui::DragFloat3(("T.acc" + num).c_str(), &this->GetParticleInf().translate.acceleration.x, 0.01f);
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("----------target---------------"))
	{
		ImGui::Checkbox(("useTarget" + num).c_str(), &this->GetParticleInf().target.useTarget);
		ImGui::Checkbox(("spawnDependent" + num).c_str(), &this->GetParticleInf().target.spawnDependent);
		ImGui::DragFloat3(("target" + num).c_str(), &this->GetParticleInf().target.target.x, 0.01f);
		ImGui::DragFloat(("speed" + num).c_str(), &this->GetParticleInf().target.speed, 0.1f);
		ImGui::DragFloat(("angle" + num).c_str(), &this->GetParticleInf().target.spreadAngle, 0.1f);

		ImGui::TreePop();
	}
	if (ImGui::TreeNode("----------texture--------------"))
	{
		size_t textureCount = Game::Resource::GetTextureCount();

		for (size_t i = 0; i < textureCount; ++i)
		{
			TextureData* texData = Game::Resource::GetTexture(static_cast<uint32_t>(i));
			if (texData)
			{
				ImGui::Image((ImTextureID)texData->textureSrvHandleGPU.ptr, ImVec2(32, 32));

				// 6個並べたら改行
				if ((i + 1) % 6 != 0 && i < textureCount - 1)
				{
					ImGui::SameLine();
				}
				if (ImGui::IsItemClicked())
				{
					this->GetParticleInf().resource.texture = static_cast<uint32_t>(i);
				}
			}
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("----------model----------------"))
	{
		if (ImGui::Button("-"))this->GetParticleInf().resource.model -= 1;

		ImGui::SameLine();

		// ラベルを非表示にするために "##" プレフィックスで ID を与える
		std::string dragId = std::string("##model") + num;
		ImGui::DragInt(dragId.c_str(), reinterpret_cast<int*>(&this->GetParticleInf().resource.model));

		ImGui::SameLine();

		if (ImGui::Button("+"))this->GetParticleInf().resource.model += 1;

		// クランプ
		if (this->GetParticleInf().resource.model < 0) this->GetParticleInf().resource.model = 0;
		if (this->GetParticleInf().resource.model > int(Game::Resource::GetModelCount() - 1)) this->GetParticleInf().resource.model = int(Game::Resource::GetModelCount() - 1);

		ImGui::TreePop();
	}
	if (ImGui::TreeNode("----------density--------------"))
	{
		int particlesPerEmission = int(this->GetParticleInf().density.particlesPerEmission);
		ImGui::DragInt(("particlePerEmission" + num).c_str(), &particlesPerEmission);
		if (particlesPerEmission < 0)particlesPerEmission = 0;
		this->GetParticleInf().density.particlesPerEmission = uint32_t(particlesPerEmission);
		int emissionDelay = int(this->GetParticleInf().density.emissionDelay);
		ImGui::DragInt(("emissionDelay" + num).c_str(), &emissionDelay);
		if (emissionDelay < 1)emissionDelay = 1;
		this->GetParticleInf().density.emissionDelay = uint32_t(emissionDelay);
		ImGui::Text("lifetime");
		ImGui::DragInt(("liveMax" + num).c_str(), &this->GetParticleInf().density.liveMax);

		ImGui::TreePop();
	}
	if (ImGui::TreeNode("----------color----------------"))
	{
		Vector4 preColor = ConvertUintToVector4(this->GetParticleInf().material.color);
		float floatColor[4] = { preColor.x, preColor.y, preColor.z, preColor.w };
		ImGui::ColorEdit4((num + "color").c_str(), floatColor, 1);
		Vector4 vector4Color = { floatColor[0], floatColor[1], floatColor[2], floatColor[3] };
		this->GetParticleInf().material.color = ConvertVector4ToUint(vector4Color);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("----------option---------------"))
	{
		ImGui::Checkbox("Billboard", &this->GetParticleInf().option.isBillboard);
		ImGui::TreePop();
	}
	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	if (ImGui::TreeNode("----------load & save----------"))
	{
		char buf[256];
		if (this->filePath.size() < sizeof(buf)) memcpy(buf, this->filePath.c_str(), this->filePath.size() + 1);
		else buf[sizeof(buf) - 1] = '\0';
		if (ImGui::InputText(".json", buf, sizeof(buf)))
		{
			this->filePath = std::string(buf);
		}
		if (ImGui::Button("save"))
		{
			JsonManager::SaveToJson(*this, this->filePath);
		}
		ImGui::SameLine();
		if (ImGui::Button("load"))
		{
			JsonManager::LoadFromJson(*this, this->filePath);
		}
		ImGui::TreePop();
	}

	ImGui::End();
}

void RenderData_Particle::DrawEmitter()
{
	if (this->GetParticleInf().emitter.useSphereEmitter)
	{
		Game::DebugDraw::AddSphere(this->GetParticleInf().emitter.emitterSphere.center, this->GetParticleInf().emitter.emitterSphere.radius, 0xFFFFFF22);
	}
	else
	{
		Game::DebugDraw::AddAABB(this->GetParticleInf().emitter.emitterAABB, 0xFFFFFF22);
	}
}

#pragma endregion