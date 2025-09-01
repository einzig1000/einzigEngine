#pragma once
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

class CameraController;
//class RenderData_Model;

class Game
{
public:
	class RenderData_Model
	{
	private:
	public:
		RenderData_Model();
		// 位置、回転、スケール
		Transforms transforms;
		// 回転の中心点
		Vector3 pivot;
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
		bool isCollisionMouseRay;

		////LookAtVector3(Game::GetCamera()->GetCenter());
		// 任意のポイントを向く
		void LookAtOnce(const Vector3& targetWorldPos);
		void LookAtOnce(const RenderData_Model* other);
		void LookAtCamera();
		void LookAtFront();

		// 外部から度数法のオイラー角を指定
		void    SetRotationEuler(const Vector3& eulerRad);
		Vector3 GetRotationEuler() const;    // 度数法で返す

		// ワールド位置を返す（デバッグライン描画用など）
		Vector3 GetWorldPosition()       const;
		Vector3 GetTargetWorldPosition() const;

		// 描画
		void Draw();
		void DrawAABB();
		void CreateAABB();  // AABBのみ更新

		// 内部クォータニオン取得
		const Quaternion& GetRotationQuaternion() const { return rotationQuat; }

	private:
		// 内部回転状態
		Quaternion rotationQuat;
		// LookAtOnce の引数を保持
		Vector3    lastTargetWorldPos{};

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
	//static void Drawobj(const Transforms& transform, const Vector3& center, uint32_t objectNumber, uint32_t textureNumber, const uint32_t& materialColor, const DrawOptions drawOptions);
	static void Drawobj(Game::RenderData_Model& renderData);
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

	// モデルリストの取得
	static std::vector<Game::RenderData_Model*> GetModelList() { return renderModels; }

	static void AddModel(Game::RenderData_Model* model)
	{
		renderModels.push_back(model);
	}
private:
	static std::vector<Game::RenderData_Model*> renderModels;




};
