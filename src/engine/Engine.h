#pragma once
#include "definition/definition.h"
#include <array>
#include <vector>
#include <string>
#include <wrl/client.h>
#include "Game.h" 

#include "Window/WindowManager.h"
#include "DirectX/DirectXManager.h"
#include "Camera/CameraManager.h"
#include "DrawSystem/DrawSystem.h"

class Engine
{
public:
	// 唯一のインスタンス取得
	static Engine& Instance();

	// コピー・ムーブ禁止
	Engine(const Engine&) = delete;
	Engine& operator=(const Engine&) = delete;
	Engine(Engine&&) = delete;
	Engine& operator=(Engine&&) = delete;

	// メインループ用
	void Initialize(int width, int height, const std::wstring& title);
	bool ProcessMessage();
	void BeginFrame();
	void UpdateTransforms();
	void EndFrame();
	void Finalize();

	// リソース
	uint32_t LoadModel(const std::string& directoryPath, const std::string& filename);
	uint32_t LoadTexture(const std::string& filePath);
	uint32_t LoadAudio(const std::string& filePath);
	TextureData* GetTexture(uint32_t textureNumber);
	size_t GetTextureCount();
	size_t GetModelCount();

	// 描画
	void DrawModel(RenderData_Model& renderData);
	void DrawTriangle(RenderData_Triangle& renderData);
	void DrawSprite(RenderData_Sprite& renderData);
	void DrawParticle(RenderData_Particle& renderData);
	void DrawLine(RenderData_Line& renderData);
	void AddSphere(Vector3 pos, Vector3 radius, uint32_t color);
	void AddAABB(AABB aabb, uint32_t color);

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

	/// キーボード
	//bool GetKeyboardPress(int key);
	//bool GetKeyboardPrePress(int key);

	/// ゲームパッド
	//PadState GetPadState();
	//PadState GetPrePadState();

	// カメラ
	Vector3 GetCameraTranslate() const;
	void MoveCameraCenter(Vector3 target, int spendFrame, EaseType easetype);
	void MoveCameraRotate(Vector3 target, int spendFrame, EaseType easetype);
	void MoveCameraDistance(float target, int spendFrame, EaseType easetype);
	void StartCameraShake(float intensity, float duration, float frequency = 25.0f);
	bool IsCameraShaking();
	void StopCameraShake();

	/// カメラシェイク

	// フルスクリーン切り替え
	void ToggleFullscreen();

	// AABBの作成
	std::vector<AABB>  CreateAABB(const Transforms& transforms, uint32_t objectNumber);

	// プリミティブモードの設定
	void toggleWireframeMode();

private:
	Engine() = default;
	~Engine() = default;

	// カメラ更新
	void UpdateCamera();
	void UpdateDebugInfo();
	bool isDebugInfo = true;

	// ウィンドウ関連
	WindowManager* windowManager = nullptr;
	// DirectX関連
	DirectXManager* dxManager = nullptr;
	// 描画関連
	DrawSystem* drawSystem = nullptr;
	// 入力関連
	Input* inputManager_ = nullptr;
	// カメラ
	CameraManager* cameraManager = nullptr;
	//CameraController* cameraController = nullptr;
	//CameraController* debugCameraController = nullptr;
	//bool debugCamera = false;


};