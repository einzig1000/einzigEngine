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

	// リソース読み込み
	uint32_t LoadOBJ(const std::string& directoryPath, const std::string& filename);
	uint32_t LoadTexture(const std::string& filePath);
	uint32_t LoadAudio(const std::string& filePath);

	TextureData* GetTexture(uint32_t textureNumber);

	// 描画
	void Drawobj(Game::RenderData_Model& renderData);
	void DrawTriangle(Game::RenderData_Triangle& renderData);
	void DrawSprite(Game::RenderData_Sprite& renderData);
	void DrawParticle(Game::RenderData_Particle& renderData);
	void DrawLine(const Vector3& start, const Vector3& end, const uint32_t& materialColor);
	void DrawSphere(const Transforms& transform, const Vector3& center, uint32_t kSubdivision, uint32_t textureNumber, const uint32_t& materialColor, const DrawOptions drawOptions);

	// カメラの更新時に視錐台の平面を計算する関数
	void CreateFrustumPlanes(const Matrix4x4& viewProjectionMatrix);
	// AABBが視錐台内にあるか判定する関数
	bool IsAABBInFrustum(const AABB& aabb, const Matrix4x4& worldMatrix);

	// 音
	void PlayAudio(const uint32_t& audioId, bool loop);
	void StopAudio(const uint32_t& audioId);
	void SetAudioVolume(const uint32_t& audioId, float volume);
	void SetMasterVolume(float volume);
	float GetVolume(const uint32_t& audioId);
	float GetMasterVolume();
	bool IsAudioPlaying(const uint32_t& audioId);

	// ライト
	void SetLightColor(const Vector4 color) { directionalLightData->color = color; }
	void SetLightDirection(const Vector3 direction) { directionalLightData->direction = direction; }
	void SetLightIntensity(float intensity) { directionalLightData->intensity = intensity; }
	void ToggleLightMode(const uint32_t mode) { directionalLightData->mode = mode; }

	// マウス
	Vector2 GetMousePosition();
	uint32_t GetMouseWheel();
	Ray GetMouseRay();
	bool IsCollisionMouseRayAABB(uint32_t objectNumber, const Transforms& data);
	bool IsPressMouse(int i);

	// カメラ
	void MoveCenterTarget(Vector3 target, int spendFrame, EaseType easetype);
	void MoveRotateTarget(Vector3 target, int spendFrame, EaseType easetype);
	void MoveDistanceTarget(float target, int spendFrame, EaseType easetype);
	void SetControlModeCamera(bool mode);
	CameraController* GetCamera();
	CameraController* GetDebugCamera();

	// カメラシェイク
	//void StartCameraShake(float intensity, float duration, float frequency = 25.0f);
	//bool IsCameraShaking();

	// AABBの作成
	AABB CreateAABB(const Transforms& transforms, uint32_t objectNumber);

	// プリミティブモードの設定
	void toggleWireframeMode();

private:
	void UpdateCamera();
	void UpdateLight();
	AABB CreateLocalAABB(const ModelData& model);
	void InitializeLineResources(ID3D12Device* device);

	WindowManager* windowManager;
	DirectXManager* dxManager;

	// 頂点リソースと頂点データと使用済み頂点数

	// モデル
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResourceObj;
	std::vector<Object3D> objects;
	UINT vertexResourceSizeObj;

	// 球
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResourceSphere;
	UINT vertexResourceSizeSphere;
	std::vector<VertexData> sphereVertexData;
	size_t sphereVertexDataUsed = 0;

	// スプライト
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResourceSprite;
	UINT vertexResourceSizeSprite;
	std::vector<VertexData> spriteVertexData;
	size_t spriteVertexDataUsed = 0;

	// 三角形
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResourceTriangle;
	UINT vertexResourceSizeTriangle;

	// 線
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResourceLine;
	UINT vertexResourceSizeLine;

	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource;
	D3D12_INDEX_BUFFER_VIEW indexBufferView;

	// 使い回す定数バッファ（マテリアル/WVP）をフレーム数分用意
	size_t kMaxDrawCallPerFrame = 1280;
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> materialResources;
	std::vector<Material*> materialData;
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> wvpResources;
	std::vector<TransformationMatrix*> wvpData;
	size_t drawCallIndex;

	// ラインはその他の3Dオブジェクトと比べて必要な情報量が少ないから他のと一緒に扱ったら余計な容量使う。はず
	size_t kMaxDrawLineCallPerFrame = 2560;
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> materialResourceLine;
	std::vector<Material*> materialDataLine;
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> wvpResourceLine;
	std::vector<TransformationMatrix*> wvpDataLine;
	size_t drawLineCallIndex;

	// ライト
	Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource;
	DirectionalLight* directionalLightData;

	// カメラ
	CameraController* cameraController;
	CameraController* debugCameraController;
	bool debugCamera;
	std::array<Plane, 6> frustumPlanes_;// 視錐台を構成する6つの平面

	// 入力
	Input* inputManager_;
	int wheelDelta;

	// プリミティブモード
	bool WireframeMode;
};