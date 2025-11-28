#pragma once
#include "definition/definition.h"
#include <array>
#include <vector>
#include <string>
#include <wrl/client.h>
#include "ResourceLoder/ResourceID.h"


class WindowManager;
class DirectXManager;
class DrawSystem;
class Input;
class CameraManager;

class RenderData_Model;
class RenderData_Triangle;
class RenderData_Rect;
class RenderData_Sprite;
class RenderData_Line;
class RenderData_MinecraftMap;
class RenderData_Particle;
class RenderData_Particle3;



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
	void UpdateParticles();
	void EndFrame();
	void Finalize();

	// リソース
	uint32_t LoadModel(const std::string& directoryPath, const std::string& filename);
	uint32_t LoadTexture(const std::string& filePath);
	uint32_t LoadAudio(const std::string& filePath);
	Object3D* GetModelData(uint32_t modelNumber);
	TextureData* GetTextureData(uint32_t textureNumber);
	size_t GetTextureCount();
	size_t GetModelCount();

	// 描画
	void AddModelDrawList(RenderData_Model& renderData);
	void AddTriangleDrawList(RenderData_Triangle& renderData);
	void AddRectDrawList(RenderData_Rect& renderData);
	void AddSpriteDrawList(RenderData_Sprite& renderData);
	void AddLineDrawList(RenderData_Line& renderData);
	void AddParticleDrawList(RenderData_Particle3& renderData);

	void DrawMinecraftMap(RenderData_MinecraftMap& renderData);
	void AddSphere(Vector3 pos, Vector3 radius, uint32_t color);
	void AddAABB(AABB aabb, uint32_t color);
	void AddLine(Vector3 start, Vector3 end, uint32_t color);

	void DrawParticle(RenderData_Particle& renderData);

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
	void SetLightColor(const Vector4 color);
	void SetLightDirection(const Vector3 direction);
	void SetLightIntensity(float intensity);
	void ToggleLightMode(const LightMode mode);

	// マウス
	Vector2 GetMousePosition();
	Vector3 GetMouseWorldPosition();
	uint32_t GetMouseWheel();
	Ray GetMouseRay();
	bool IsMouseHeld(int i);// 今押しているか
	bool IsMouseJustPressed(int i);// 押した瞬間（今フレームで押された）
	bool IsMouseJustReleased(int i);// 離した瞬間（今フレームで離れた）
	uint32_t MouseHoldFrames(int i);// 押されてからの経過フレーム数
	void ToggleMouseCursorVisible();

	// キーボード
	bool IsKeyHeld(BYTE key);// 今押しているか
	bool IsKeyJustPressed(BYTE key);// 押した瞬間（今フレームで押された）
	bool IsKeyJustReleased(BYTE key);// 離した瞬間（今フレームで離れた）
	uint32_t KeyHoldFrames(BYTE key);// 押されてからの経過フレーム数
	int TestTapLong(int n, BYTE key);// 0: なし  1:単押し  2:長押し(n = 長押し判定)

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
	void ToggleCameraMode();
	void ToggleCurrentOrbitMode();
	void StopCameraShake();
	CameraManager* GetCameraManager() { return cameraManager; }


	// フルスクリーン切り替え
	void ToggleFullscreen();

	// AABBの作成
	std::vector<AABB>  CreateAABB(RenderData_Model* data);

	// プリミティブモードの設定
	void toggleWireframeMode();


	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(size_t sizeInBytes);

	Microsoft::WRL::ComPtr<ID3D12Resource> CreateConstantBufferResource(size_t sizeInBytes);

	DirectXManager* GetDirectXManager() { return dxManager; }


	const std::vector<Object3D> GetAllObject3D();

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

};