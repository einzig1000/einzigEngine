#pragma once
#include "definition/definition.h"
#include <array>
#include <vector>
#include <string>
#include <wrl/client.h>

class WindowManager;
class DirectXManager;
class CameraController;
class MouseController;

class Engine
{
public:
	// 初期化
	void Initialize(int width, int height, const std::wstring& title);

	// メインループ用
	bool ProcessMessage();
	void BeginFrame();
	void EndFrame();

	void UpdateTransforms();

	// 終了処理
	void Finalize();

	// リソース読み込み
	uint32_t LoadOBJ(const std::string& directoryPath, const std::string& filename);
	uint32_t LoadTexture(const std::string& filePath);
	uint32_t LoadAudio(const std::string& filePath);

	TextureData* GetTexture(uint32_t textureNumber);

	// 描画
	void Drawobj(const Transforms& transform, const Vector3& center, uint32_t objectNumber, uint32_t textureNumber, const uint32_t& materialColor, const DrawOptions drawOptions);
	void DrawSphere(const Transforms& transform, const Vector3& center, uint32_t kSubdivision, uint32_t textureNumber, const uint32_t& materialColor, const DrawOptions drawOptions);
	void DrawTriangle(const Transforms& transform, const Vector3& pos1, const Vector3& pos2, const Vector3& pos3, uint32_t textureNumber, const uint32_t& materialColor, const DrawOptions drawOptions);
	void DrawSprite(const Transforms& transform, const Vector2& center, uint32_t textureNumber, const uint32_t& materialColor, const DrawOptions drawOptions);
	void DrawLine(const Vector3& start, const Vector3& end, const uint32_t& materialColor);

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
	void SetMouseRay();
	Ray GetMouseRay();
	bool IsCollisionMouseRayAABB(uint32_t objectNumber, const Transforms& data);
	bool IsPressMouse(int i);
	uint32_t GetMouseWheel();

	// カメラ
	void MoveCenterTarget(Vector3 target, int spendFrame);
	void MoveRotateTarget(Vector3 target, int spendFrame);
	void MoveDistanceTarget(float target, int spendFrame);
	void SetControlModeCamera(bool mode);
	void SetControlModeCameraCenter(bool mode);
	void SetControlModeCameraRotate(bool mode);
	void SetControlModeCameraDistance(bool mode);

	// AABBの作成
	AABB CreateAABB(const Transforms& transforms, uint32_t objectNumber);

	// プリミティブモードの設定
	void toggleWireframeMode();

private:
	void UpdateCamera();
	void UpdateLight();
	AABB CreateLocalAABB(const ModelData& model);
	void InitializeLineResources(ID3D12Device* device);

#pragma region
	/// <summary>
	/// Draw用データ作成するやつ
	/// </summary>
	/// <param name="dstBufferSize">Map先バッファサイズ</param>
	/// <param name="srcVertexData">コピー元頂点データ</param>
	/// <param name="vertexCount">頂点数</param>
	/// <param name="vertexResource">頂点リソース</param>
	/// <param name="vertexResourceSize">頂点リソースサイズ</param>
	/// <param name="material">マテリアル</param>
	/// <param name="materialColor">マテリアル色</param>
	/// <param name="enableLighting">ライティングするか</param>
	/// <param name="uvTransform">UV変換</param>
	/// <param name="wvp">WVP</param>
	/// <param name="world">ワールド行列</param>
	/// <param name="wvpMatrix">WVP行列</param>
	/// <param name="textureNumber">テクスチャ番号</param>
	/// <returns> Draw用データ</returns>
	DrawData SetupDrawData(
		size_t dstBufferSize,
		const VertexData* srcVertexData,
		size_t vertexCount,
		Microsoft::WRL::ComPtr<ID3D12Resource>& vertexResource,
		UINT& vertexResourceSize,
		Material* material,
		const uint32_t& materialColor,
		bool enableLighting,
		const Matrix4x4& uvTransform,
		TransformationMatrix* wvp,
		const Matrix4x4& world,
		const Matrix4x4& wvpMatrix,
		uint32_t textureNumber);

#pragma endregion

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
	size_t kMaxDrawCallPerFrame = 256;
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> materialResources;
	std::vector<Material*> materialData;
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> wvpResources;
	std::vector<TransformationMatrix*> wvpData;
	size_t drawCallIndex;

	// ラインはその他の3Dオブジェクトと比べて必要な情報量が少ないから他のと一緒に扱ったら余計な容量使う。はず
	size_t kMaxDrawLineCallPerFrame = 1280;
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

	// マウスホイール量
	MouseController* mouseController;
	int wheelDelta;

	// プリミティブモード
	bool WireframeMode;
};
