#pragma once
#include "definition/definition.h"
#include <array>
#include <vector>
#include <string>
#include <wrl/client.h>
#include "Game.h" 

#include "Window/WindowManager.h"
#include "DirectX/DirectXManager.h"
#include "Camera/CameraController.h"
#include "DrawSystem/DrawSystem.h"

class Engine
{
public:
	// 初期化
	void Initialize(int width, int height, const std::wstring& title);

	// メインループ用
	bool ProcessMessage();
	void BeginFrame();
	void UpdateTransforms();
	void EndFrame();

	// 終了処理
	void Finalize();

	// リソース
	uint32_t LoadModel(const std::string& directoryPath, const std::string& filename);
	uint32_t LoadTexture(const std::string& filePath);
	uint32_t LoadAudio(const std::string& filePath);
	TextureData* GetTexture(uint32_t textureNumber);

	// 描画
	void DrawModel(Game::RenderData_Model& renderData);
	//void DrawSphere(Game::RenderData_Sphere& renderData);
	void DrawTriangle(Game::RenderData_Triangle& renderData);
	void DrawSprite(Game::RenderData_Sprite& renderData);
	void DrawParticle(Game::RenderData_Particle& renderData);
	void DrawLine(const Vector3& start, const Vector3& end, const uint32_t& materialColor);
	void DrawSphere(const Transforms& transform, const Vector3& center, uint32_t kSubdivision, uint32_t textureNumber, const uint32_t& materialColor, const DrawOptions drawOptions);

	// AABBが視錐台内にあるか判定する関数
	bool InFrustum(const AABB& aabb);

	// 音
	void PlayAudio(const uint32_t& audioId, bool loop);
	void StopAudio(const uint32_t& audioId);
	void SetAudioVolume(const uint32_t& audioId, float volume);
	void SetMasterVolume(float volume);
	float GetVolume(const uint32_t& audioId);
	float GetMasterVolume();
	bool IsAudioPlaying(const uint32_t& audioId);

	// ライト
	void SetLightColor(const Vector4 color) { drawSystem->SetLightColor(color); }
	void SetLightDirection(const Vector3 direction) { drawSystem->SetLightDirection(direction); }
	void SetLightIntensity(float intensity) { drawSystem->SetLightIntensity(intensity); }
	void ToggleLightMode(const uint32_t mode) { drawSystem->ToggleLightMode(mode); }

	// マウス
	Vector2 GetMousePosition();
	uint32_t GetMouseWheel();
	Ray GetMouseRay();
	bool GetMousePress(int i);
	bool GetMousePrePress(int i);

	// カメラ
	void MoveCameraCenter(Vector3 target, int spendFrame, EaseType easetype);
	void MoveCameraRotate(Vector3 target, int spendFrame, EaseType easetype);
	void MoveCameraDistance(float target, int spendFrame, EaseType easetype);
	void SetControlModeCamera(bool mode);
	CameraController* GetCamera();
	CameraController* GetDebugCamera();

	// カメラシェイク
	//void StartCameraShake(float intensity, float duration, float frequency = 25.0f);
	//bool IsCameraShaking();

	// AABBの作成
	std::vector<AABB>  CreateAABB(const Transforms& transforms, uint32_t objectNumber);

	// プリミティブモードの設定
	void toggleWireframeMode(bool mode);

private:
	// カメラ更新
	void UpdateCamera();

	// ウィンドウ関連
	WindowManager* windowManager = nullptr;
	// DirectX関連
	DirectXManager* dxManager = nullptr;
	// 描画関連
	DrawSystem* drawSystem = nullptr;
	// 入力関連
	Input* inputManager_ = nullptr;
	// カメラ
	CameraController* cameraController = nullptr;
	CameraController* debugCameraController = nullptr;
	bool debugCamera = false;


	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource;
	D3D12_INDEX_BUFFER_VIEW indexBufferView;

};