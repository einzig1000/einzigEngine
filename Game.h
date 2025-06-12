#pragma once
#include "WindowManager.h"
#include "DirectXManager.h"
#include "definition.h"
#include "CameraController.h"
#include "MouseController.h"
#include <array>
#include <vector>
#include <string>
#include <wrl/client.h>

class Game
{
public:
	// 初期化
	static void Initialize(int width, int height, const std::wstring& title);

	// メインループ用
	static bool ProcessMessage();
	static void BeginFrame();
	static void EndFrame();

	// 終了処理
	static void Finalize();

	// リソース読み込み
	static int LoadOBJ(const std::string& directoryPath, const std::string& filename);
	static int LoadTexture(const std::string& filePath);

	// 描画
	static void Drawobj(const Transforms& transform, const Vector3& center, uint32_t objectNumber, uint32_t textureNumber, const uint32_t& materialColor);
	static void DrawTriangle(const Transforms& localTransform, const Transforms& worldTransform, const VertexData* vertexData, uint32_t textureNumber, const uint32_t& materialColor);
	static void DrawSphere(const Transforms& localTransform, VertexData* vertexData, uint32_t kSubdivision, uint32_t textureNumber, const uint32_t& materialColor);
	static void DrawSprite(const Transforms& localTransform, VertexData* vertexData, uint32_t textureNumber, const uint32_t& materialColor);
	static TextureData* GetTexture(uint32_t textureNumber);

	// 音
	static uint32_t LoadAudio(const std::string& filePath);
	static void PlayAudio(const uint32_t& audioId, bool loop);
	static void StopAudio(const uint32_t& audioId);
	static void SetAudioVolume(const uint32_t& audioId, float volume);
	static void SetMasterVolume(float volume);

	// マウス
	static void GetMousePosition(Vector2* position);
	static void SetMouseRay();
	static bool IsCollisionMouseRayAABB(AABB aabb, int objNum);
	static bool IsPressMouse(int i);
	static int GetWheel();

	// カメラ
	static void MoveCenterTarget(Vector3 target, int spendFrame);
	static void MoveRotateTarget(Vector3 target, int spendFrame);
	static void MoveDistanceTarget(float target, int spendFrame);

	// AABBの作成
	static AABB CreateAABB(const Transforms& transforms, uint32_t objectNumber);


private:
	static void UpdateCamera();
	static void UpdateLight();
	static Vector4 ConvertUintToVector4(uint32_t color);
	static Vector4 ConvertARGBtoRGBA(const Vector4& argb);
	static AABB CreateLocalAABB(const ModelData& model);

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
	/// <param name="textures">テクスチャリスト</param>
	/// <returns> Draw用データ</returns>
	static DrawData SetupDrawData(
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
		uint32_t textureNumber,
		const std::vector<TextureData>& textures);

#pragma endregion

	static WindowManager* windowManager;
	static DirectXManager* dxManager;

	// モデル
	static std::vector<Object3D> objects;

	// 画像
	static std::vector<TextureData> textures;

	// 頂点リソースども
	static Microsoft::WRL::ComPtr<ID3D12Resource> vertexResourceSprite;
	static UINT vertexResourceSizeSprite;

	static Microsoft::WRL::ComPtr<ID3D12Resource> vertexResourceObj;
	static UINT vertexResourceSizeObj;

	static Microsoft::WRL::ComPtr<ID3D12Resource> vertexResourceTriangle;
	static UINT vertexResourceSizeTriangle;

	static Microsoft::WRL::ComPtr<ID3D12Resource> vertexResourceSphere;
	static UINT vertexResourceSizeSphere;


	static Microsoft::WRL::ComPtr<ID3D12Resource> indexResource;
	static D3D12_INDEX_BUFFER_VIEW indexBufferView;


	// 使い回す定数バッファ（マテリアル/WVP）をフレーム数分用意
	static constexpr size_t kMaxDrawCallPerFrame = 256;
	static std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> materialResources;
	static std::vector<Material*> materialData;
	static std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> wvpResources;
	static std::vector<TransformationMatrix*> wvpData;
	static size_t drawCallIndex;

	// ライト
	static Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource;
	static DirectionalLight* directionalLightData;

	// カメラ
	static CameraController* cameraController;

	// マウスホイール量
	static MouseController* mouseController;
	static int wheelDelta;
};
