#pragma once
#include "Engine/Engine.h"
#include <numbers>

#include "definition/definition.h"
#include "Utilities/Quaternion.h"
#include "input/GetHitKey.h"
#include "input/GetPadState.h"
#include "Utilities/Easings.h"
#include "Utilities/functions.h"

#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"

class Game
{
public:
	enum class LookAtMode
	{
		None,
		StaticVector,
		StaticTransform
	};

	struct LookAtTarget
	{
		LookAtMode mode = LookAtMode::None;
		Vector3 staticTarget = { 0, 0, 0 };
		Transforms* dynamicTransform = nullptr;
	};

	// 初期化
	static void Initialize(int width, int height, const std::wstring& title);

	// メインループ用
	static bool ProcessMessage();
	static void BeginFrame();
	static void EndFrame();

	// 終了処理
	static void Finalize();

	// リソース読み込み
	static uint32_t LoadOBJ(const std::string& directoryPath, const std::string& filename);
	static uint32_t LoadTexture(const std::string& filePath);
	static uint32_t LoadAudio(const std::string& filePath);
	static TextureData* GetTexture(uint32_t textureNumber);

	// 描画
	static void Drawobj(const Transforms& transform, const Vector3& center, uint32_t objectNumber, uint32_t textureNumber, const uint32_t& materialColor, const DrawOptions drawOptions);
	static void DrawSphere(const Transforms& transform, const Vector3& center, uint32_t kSubdivision, uint32_t textureNumber, const uint32_t& materialColor, const DrawOptions drawOptions);
	static void DrawSprite(const Transforms& transform, const Vector2& center, uint32_t textureNumber, const uint32_t& materialColor, const DrawOptions drawOptions);
	static void DrawTriangle(const Transforms& transform, const Vector3& pos1, const Vector3& pos2, const Vector3& pos3, uint32_t textureNumber, const uint32_t& materialColor, const DrawOptions drawOptions);
	static void DrawLine(const Vector3& start, const Vector3& end, const uint32_t& materialColor);

	// 音
	static void PlayAudio(const uint32_t& audioId, bool loop);
	static void StopAudio(const uint32_t& audioId);
	static void SetAudioVolume(const uint32_t& audioId, float volume);
	static void SetMasterVolume(float volume);
	static float GetVolume(const uint32_t& audioId);
	static float GetMasterVolume();
	static bool IsAudioPlaying(const uint32_t& audioId);

	// ライト
	static void SetLightColor(const Vector4 color);
	static void SetLightDirection(const Vector3 direction);
	static void ToggleLightMode(const uint32_t mode);
	static void SetLightIntensity(float intensity);

	// マウス
	static Vector2 GetMousePosition();
	static void SetMouseRay();
	static Ray GetMouseRay();
	static bool IsCollisionMouseRayAABB(uint32_t objectNumber, const Transforms& data);
	// 0 = 左クリック  1 = 右クリック  2 = ミドルボタン
	static bool GetMousePress(int i);
	static uint32_t GetMouseWheel();

	// カメラ
	static void MoveCenterTarget(Vector3 target, int spendFrame, EaseType easetype);
	static void MoveRotateTarget(Vector3 target, int spendFrame, EaseType easetype);
	static void MoveDistanceTarget(float target, int spendFrame, EaseType easetype);
	static void SetControlModeCamera(bool mode);
	static void SetControlModeCameraCenter(bool mode);
	static void SetControlModeCameraRotate(bool mode);
	static void SetControlModeCameraDistance(bool mode);
	static CameraController* GetCamera();

	// AABBの作成
	static AABB CreateAABB(const Transforms& transforms, uint32_t objectNumber);

	// プリミティブモードの設定
	static void toggleWireframeMode();

	class RenderData_Model
	{
	public:
		// 位置、回転、スケール
		Transforms transforms;
		// 回転の中心点
		Vector3 pivot;
		// 向き
		LookAtTarget target;
		// 色
		uint32_t color = 0xFFFFFFFF;
		// 3Dモデル
		uint32_t model = 0;
		// テクスチャ
		uint32_t texture = 0;
		// 描画オプション
		DrawOptions options;
		// 衝突判定用AABB
		AABB AABB;
		// マウスと衝突してるか？
		//bool 

		// 自身のワールド位置
		Vector3 GetWorldPosition() const
		{
			if (transforms.parentWorld)
				return Transform(transforms.translate, *transforms.parentWorld);
			else
				return transforms.translate;
		}
		// ターゲットのワールド位置
		Vector3 GetTargetWorldPosition() const
		{
			switch (target.mode)
			{
			case LookAtMode::StaticVector:
				return target.staticTarget;

			case LookAtMode::StaticTransform:
				if (target.dynamicTransform)
				{
					return target.dynamicTransform->parentWorld
						? Transform(target.dynamicTransform->translate, *target.dynamicTransform->parentWorld)
						: target.dynamicTransform->translate;
				}
				break;

			default:
				break;
			}
			return GetWorldPosition() + Vector3(0, 0, 1);
		}


		void LookAtOnce(const Vector3& targetWorldPos)
		{
			target.mode = LookAtMode::StaticVector;
			target.staticTarget = targetWorldPos;
			target.dynamicTransform = nullptr;
		}

		void LookAtOnce(const Transforms& targetTransforms)
		{
			target.mode = LookAtMode::StaticTransform;
			target.dynamicTransform = const_cast<Transforms*>(&targetTransforms);
		}

		void LookAtFront()
		{
			LookAtOnce(GetWorldPosition() + Vector3(0, 0, 1));
		}



		// 描画処理
		void Draw()
		{
			// スケール
			Matrix4x4 scaleMatrix = Matrix4x4::MakeScaleMatrix(transforms.scale);

			// 移動
			Matrix4x4 translateMatrix = Matrix4x4::MakeTranslateMatrix(transforms.translate);

			// 回転中心への移動
			Matrix4x4 toPivot = Matrix4x4::MakeTranslateMatrix(-pivot);
			Matrix4x4 fromPivot = Matrix4x4::MakeTranslateMatrix(pivot);

			// 回転構築
			Quaternion finalRotation;

			// ターゲットのワールド位置を取得
			Vector3 targetWorldPos = GetTargetWorldPosition();
			// 自身のワールド位置を取得
			Vector3 worldPos = GetWorldPosition();

			// 既存のLookAtロジック
			if (target.mode != LookAtMode::None)
			{
				// 前方ベクトルを計算
				Vector3 forward = (targetWorldPos - worldPos).Normalized();

				// 上方向ベクトルを定義（Y軸を上とする）
				Vector3 upVector = Vector3(0, 1, 0);

				// forwardがupVectorとほぼ平行かどうかをチェック
				if (abs(forward.Dot(upVector)) > 0.999f)
				{
					// ターゲットが真上または真下にある場合、代替の上方向を使用
					upVector = Vector3(0, 0, 1);
				}

				// 新しいLookAtのロジック
				Quaternion lookAtRotation = Quaternion::LookRotation(forward, upVector);
				finalRotation = lookAtRotation;
			}
			else
			{
				finalRotation = Quaternion::MakeFromEulerAngles(transforms.rotate);
			}

			Matrix4x4 rotationMatrix = Matrix4x4::MakeFromQuaternion(finalRotation);

			Matrix4x4 local =
				scaleMatrix *
				toPivot *
				rotationMatrix *
				fromPivot *
				translateMatrix;

			if (transforms.parentWorld)transforms.World = local * (*transforms.parentWorld);
			else transforms.World = local;


			// AABB更新
			AABB = Game::CreateAABB(transforms, model);

			// モデル描画
			Game::Drawobj(transforms, pivot, model, texture, color, options);

			// ターゲット方向へのライン描画（デバッグ用）
			if (target.mode != LookAtMode::None)
			{
				Game::DrawLine(GetWorldPosition(), GetTargetWorldPosition(), 0xFF00FFFF);
			}
		}


		void DrawAABB()
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

		// AABBのみ更新
		void CreateAABB()
		{
			this->AABB = Game::CreateAABB(this->transforms, this->model);
		}

	};

	class RenderData_Sprite
	{
	public:
		// 位置、回転、スケール
		Transforms transforms;
		// 親のワールドマトリックス
		TransformationMatrix parentTransformationMatrix;
		// 回転の中心点
		Vector2 pivot = { 0,0 };
		// 色
		uint32_t color = 0xFFFFFFFF;
		// テクスチャ
		uint32_t texture = 0;
		// 描画オプション
		DrawOptions options;
		// マウスと衝突してるか？
		bool colisionMouseRay = false;

		void Draw()
		{
			Game::DrawSprite(this->transforms, this->pivot, this->texture, this->color, this->options);

			// スプライトの中心座標
			Vector2 center = { static_cast<float>(this->transforms.translate.x), static_cast<float>(this->transforms.translate.y) };

			TextureData* data = Game::GetTexture(this->texture);
			float width = static_cast<float>(data->metadata.width);
			float height = static_cast<float>(data->metadata.height);

			// スケール適用
			width *= this->transforms.scale.x;
			height *= this->transforms.scale.y;

			// スプライトのAABB
			float left = center.x - width * 0.5f;
			float right = center.x + width * 0.5f;
			float top = center.y - height * 0.5f;
			float bottom = center.y + height * 0.5f;

			// マウス座標取得
			Vector2 mousePos = Game::GetMousePosition();

			// 当たり判定
			this->colisionMouseRay = (mousePos.x >= left && mousePos.x <= right && mousePos.y >= top && mousePos.y <= bottom);
		}
	};
};
