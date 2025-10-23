#pragma once
#include <numbers>

#include "definition/definition.h"
#include "input/Input.h"
#include "Utilities/Easings.h"
#include "Utilities/functions.h"
#include "DrawSystem/RenderData/RenderData.h"

#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"
#include <algorithm>


class CameraController;

class Game
{
public:
	//class RenderData_Particle
	//{
	//public:
	//	// エミッター範囲
	//	AABB emitterAABB = { Vector3{ -1.0f, -1.0f, -1.0f }, Vector3{ 1.0f, 1.0f, 1.0f } };
	//	SphereXYZ emitterSphere;
	//	// エミッター範囲中心から見た時の飛んでく方向
	//	Vector3 target;
	//	// パーティクルが飛んでく扇形の中心角　0ならdirection方向まっすぐ 2πなら全方位(0～2π)
	//	Vector2 centralAngle = { 0.0f, 0.0f };
	//	// 速度
	//	float velocity = 1.0f;
	//	// emissionDelayフレームごとにパーティクルが生まれる
	//	int emissionDelay = 10;
	//	// emissionDelayフレームごとに〇個生まれる
	//	int particlesPerEmission = 1;
	//	// 誕生クールタイム
	//	int frame = 0;
	//	// 死亡クールタイム
	//	int liveMax = 10;
	//	// 描画オプション
	//	DrawParticleOptions option;
	//
	//
	//	//// 消滅方法
	//	//DestructionType destructionType = DestructionType::ToSmall;
	//	// パーティクルの毎フレーム回転量
	//	Vector3 AddRotate;
	//	// パーティクルの毎フレーム拡縮量
	//	Vector3 AddScale;
	//	// パーティクルの毎フレームカラー変化量
	//	Vector4 AddColor;
	//	// パーティクル一粒の情報
	//	RenderData_Model mono;
	//
	//	// 描画
	//	void Draw();
	//	void DrawImGui();
	//	void DrawEmitter();
	//
	//	//void Reset()
	//	//{
	//	//	modelList.clear();
	//	//	InfList.clear();
	//	//};
	//
	//	// constのない意味なしゲッター
	//	//std::vector<RenderData_Model>& GetModelList() { return modelList; }
	//	//std::vector<ParticleInf>& GetInfList() { return InfList; }
	//
	//
	//private:
	//
	//	//std::vector<RenderData_Model>modelList;
	//	//std::vector<ParticleInf> InfList;
	//};




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
	static uint32_t LoadModel(const std::string& directoryPath, const std::string& filename);
	static uint32_t LoadTexture(const std::string& filePath);
	static uint32_t LoadAudio(const std::string& filePath);
	static TextureData* GetTexture(uint32_t textureNumber);

	// 描画
	static void DrawModel(RenderData_Model& renderData);
	static void DrawSprite(RenderData_Sprite& renderData);
	static void DrawTriangle(RenderData_Triangle& renderData);
	static void DrawLine(RenderData_Line& renderData);
	static void DrawParticle(RenderData_Particle& renderData);
	static void DrawSphere(const Transforms& transform, const Vector3& center, uint32_t kSubdivision, uint32_t textureNumber, const uint32_t& materialColor, const DrawOptions drawOptions);

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
	//static bool IsCollisionMouseRayAABB(uint32_t objectNumber, const Transforms& data);
	// 0 = 左クリック  1 = 右クリック  2 = ミドルボタン
	static bool GetMousePress(int i);
	// 0 = 左クリック  1 = 右クリック  2 = ミドルボタン
	static bool GetMousePrePress(int i);
	static uint32_t GetMouseWheel();

	// カメラ
	static void MoveCameraCenter(Vector3 target, int spendFrame, EaseType easetype);
	static void MoveCameraRotate(Vector3 target, int spendFrame, EaseType easetype);
	static void MoveCameraDistance(float target, int spendFrame, EaseType easetype);
	static void SetControlModeCamera(bool mode);
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
	static std::vector<AABB> CreateAABB(const Transforms& transforms, uint32_t objectNumber);

	// プリミティブモードの設定
	static void toggleWireframeMode();

	// モデルリストの取得
	//static std::vector<Game::RenderData_Model*> GetModelList() { return renderModels; }

	// 描画オブジェクトは画面内か
	static bool InFrustum(const AABB& aabb);

private:
};