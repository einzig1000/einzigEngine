#pragma once
#include "Window/WindowManager.h"
#include "DirectX/DirectXManager.h"
#include "definition/definition.h"
#include "Camera/CameraController.h"
#include "input/MouseController.h"
#include <array>
#include <vector>
#include <string>
#include <wrl/client.h>

class Engine
{
public:
	// 初期化
	void Initialize(int width, int height, const std::wstring& title);

	// メインループ用
	bool ProcessMessage();
	void BeginFrame();
	void EndFrame();

	// 終了処理
	void Finalize();

	// リソース読み込み
	uint32_t LoadOBJ(const std::string& directoryPath, const std::string& filename);
	uint32_t LoadTexture(const std::string& filePath);
	uint32_t LoadAudio(const std::string& filePath);

	// 描画
	void Drawobj(const Transforms& transform, const Vector3& center, uint32_t objectNumber, uint32_t textureNumber, const uint32_t& materialColor);
	void Drawobj(const Transforms& transform, const Vector3& center, uint32_t objectNumber, uint32_t textureNumber, const uint32_t& materialColor, const DrawOptions drawOptions);
	void DrawSphere(const Transforms& transform, const Vector3& center, uint32_t kSubdivision, uint32_t textureNumber, const uint32_t& materialColor);
	void DrawSphere(const Transforms& transform, const Vector3& center, uint32_t kSubdivision, uint32_t textureNumber, const uint32_t& materialColor, const DrawOptions drawOptions);
	void DrawTriangle(const Transforms& localTransform, const Transforms& worldTransform, const VertexData* vertexData, uint32_t textureNumber, const uint32_t& materialColor);
	void DrawSprite(const Transforms& localTransform, VertexData* vertexData, uint32_t textureNumber, const uint32_t& materialColor);
	//void DrawSprite(const Transforms& localTransform, uint32_t textureNumber, const uint32_t& materialColor);
	void DrawLine(const Vector3& start, const Vector3& end, const uint32_t& materialColor);

	// 音
	void PlayAudio(const uint32_t& audioId, bool loop);
	void StopAudio(const uint32_t& audioId);
	void SetAudioVolume(const uint32_t& audioId, float volume);
	void SetMasterVolume(float volume);
	float GetVolume(const uint32_t& audioId);
	float GetMasterVolume();
	bool IsAudioPlaying(const uint32_t& audioId);

	// マウス
	void GetMousePosition(Vector2* position);
	void SetMouseRay();
	bool IsCollisionMouseRayAABB(AABB aabb, int objNum);
	bool IsPressMouse(int i);
	uint32_t GetWheel();

	// カメラ
	void MoveCenterTarget(Vector3 target, int spendFrame);
	void MoveRotateTarget(Vector3 target, int spendFrame);
	void MoveDistanceTarget(float target, int spendFrame);

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

	// モデル・球
	std::vector<Object3D> objects;
	std::vector<VertexData> trianglesVertexData;
	size_t trianglesVertexDataUsed = 0; // 使用済み頂点数

	// 頂点リソースども
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResourceSprite;
	UINT vertexResourceSizeSprite;

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResourceObj;
	UINT vertexResourceSizeObj;

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResourceTriangle;
	UINT vertexResourceSizeTriangle;

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResourceSphere;
	UINT vertexResourceSizeSphere;

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
