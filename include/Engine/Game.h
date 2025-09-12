#pragma once
#include <numbers>

#include "definition/definition.h"
#include "input/Input.h"
#include "Utilities/Easings.h"
#include "Utilities/functions.h"

#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"

class CameraController;

class Game
{
public:
	class RenderData_Model
	{
	public:
		RenderData_Model();
		~RenderData_Model();


		// 位置、回転、スケール
		Transforms transforms;
		// 回転の中心点
		Vector3 pivot;
		// UV座標
		Transforms uvTransform;
		// 速度
		Vector3 velocity;
		// 加速度
		Vector3 acceleration;
		// 重力加速度
		float gravity = 0.01f;
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
		// ID
		int ID = 0;
		// マウスと衝突してるか？
		bool isCollisionMouseRay();

		// 任意のポイントを向く
		void LookAtOnce(const Vector3& targetWorldPos, float roll = 0);
		void LookAtOnce(const RenderData_Model* other, float roll = 0);
		void LookAtCamera(float roll = 0);
		void LookAtFront(float roll = 0);

		// ワールド位置を返す
		Matrix4x4 GetWorldMatrix() const;
		Vector3 GetWorldPosition() const;


		// 描画
		void Draw();
		void DrawAABB();
		void DrawImGui();

	private:
		Vector3 rotationEuler;
		// LookAtOnce の引数を保持
		Vector3    lastTargetWorldPos{};
	};

	class RenderData_Sprite
	{
	public:
		// 位置、回転、スケール
		Transforms transforms;
		// アンカー
		Anker anker = Anker::Center;
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
		bool isCollisionMouseRay = false;

		void Draw();
	};

	class RenderData_Triangle
	{
	public:
		// 位置、回転、スケール
		Transforms transform;
		// 上
		Vector3 pos1;
		// 右下
		Vector3 pos2;
		// 左下
		Vector3 pos3;
		// テクスチャ
		uint32_t texture = 0;
		// 色
		uint32_t color = 0xFFFFFFFF;
		// 描画オプション
		DrawOptions options;

		void Draw();
	};

	class RenderData_Particle
	{
	public:
		// エミッター範囲
		AABB emitterAABB;
		SphereXYZ emitterSphere;
		// エミッター範囲中心から見た時の飛んでく方向
		Vector3 target;
		// パーティクルが飛んでく扇形の中心角　0ならdirection方向まっすぐ 2πなら全方位(0～2π)
		Vector2 centralAngle = { 0.0f, 0.0f };
		// 速度
		float velocity = 1.0f;
		// emissionDelayフレームごとにパーティクルが生まれる
		int emissionDelay = 10;
		// emissionDelayフレームごとに〇個生まれる
		int particlesPerEmission = 1;
		// 誕生クールタイム
		int frame = 0;
		// 死亡クールタイム
		int liveMax = 10;
		// 描画オプション
		DrawParticleOptions option;


		//// 消滅方法
		//DestructionType destructionType = DestructionType::ToSmall;
		// パーティクルの毎フレーム回転量
		Vector3 AddRotate;
		// パーティクルの毎フレーム拡縮量
		Vector3 AddScale;
		// パーティクルの毎フレームカラー変化量
		Vector4 AddColor;
		// パーティクル一粒の情報
		RenderData_Model mono;

		// 描画
		void Draw();
		void DrawEmitter();

		void Reset()
		{
			modelList.clear();
			InfList.clear();
		};

		// constのない意味なしゲッター
		std::vector<Game::RenderData_Model>& GetModelList() { return modelList; }
		std::vector<ParticleInf>& GetInfList() { return InfList; }


	private:

		std::vector<Game::RenderData_Model>modelList;
		std::vector<ParticleInf> InfList;
	};




	// 初期化
	static void Initialize(int width, int height, const std::wstring& title);

	// メインループ用
	static bool ProcessMessage();
	static void BeginFrame();
	static void UpdateTransforms();
	static void EndFrame();

	// 終了処理
	static void Finalize();

	// リソース読み込み
	static uint32_t LoadOBJ(const std::string& directoryPath, const std::string& filename);
	static uint32_t LoadTexture(const std::string& filePath);
	static uint32_t LoadAudio(const std::string& filePath);
	static TextureData* GetTexture(uint32_t textureNumber);

	// 描画
	static void Drawobj(Game::RenderData_Model& renderData);
	static void DrawSprite(Game::RenderData_Sprite& renderData);
	static void DrawTriangle(Game::RenderData_Triangle& renderData);
	static void DrawSphere(const Transforms& transform, const Vector3& center, uint32_t kSubdivision, uint32_t textureNumber, const uint32_t& materialColor, const DrawOptions drawOptions);
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
	static CameraController* GetDebugCamera();

	// カメラシェイク

	/// <summary>
	/// カメラシェイクを行う
	/// </summary>
	/// <param name="intensity">初期振幅</param>
	/// <param name="duration">時間</param>
	/// <param name="frequency">揺れる速さ</param>
	//static void StartCameraShake(float intensity, float duration, float frequency = 25.0f);
	//static bool IsCameraShaking();

	// AABBの作成
	static AABB CreateAABB(const Transforms& transforms, uint32_t objectNumber);

	// プリミティブモードの設定
	static void toggleWireframeMode();

	// モデルリストの取得
	static std::vector<Game::RenderData_Model*> GetModelList() { return renderModels; }

private:
	static std::vector<Game::RenderData_Model*> renderModels;

	static void AddModel(Game::RenderData_Model* model)
	{
		renderModels.push_back(model);
	}
	static void SubModel(Game::RenderData_Model* model)
	{
		auto it = std::find(renderModels.begin(), renderModels.end(), model);
		if (it != renderModels.end())
		{
			renderModels.erase(it);
		}
	}
};