#pragma once
#include "definition/definition.h"
#include "Game.h"

class DirectXManager;

class DrawSystem
{
public:
	DrawSystem(DirectXManager* dxManager);
	~DrawSystem();
	void Update();
	void Draw();

	void SetViewProjectionMatrix(const Matrix4x4& viewProjectionMatrix) { viewProjectionMatrix_ = viewProjectionMatrix; }

	void AddModelDrawList(RenderData_Model* renderData);
	void DrawAllModel();
	void AddTriangleDrawList(RenderData_Triangle* renderData);
	void DrawAllTriangle();
	void AddRectDrawList(RenderData_Rect* renderData);
	void DrawAllRect();
	void AddSpriteDrawList(RenderData_Sprite* renderData);
	void DrawAllSprite();
	void AddLineDrawList(RenderData_Line* renderData);
	void DrawAllLine();
	void AddParticleDrawList(RenderData_Particle* renderData);
	void DrawAllParticle();


	void AddSphere(Vector3 pos, Vector3 radius, uint32_t color);
	void AddAABB(AABB aabb, uint32_t color);
	void AddLine(Vector3 start, Vector3 end, uint32_t color);

	void SetLightColor(const Vector4 color) { directionalLightData_->color = color; }
	void SetLightDirection(const Vector3 direction) { directionalLightData_->direction = direction.Normalized(); }
	void SetLightIntensity(float intensity) { directionalLightData_->intensity = intensity; }
	void ToggleLightMode(const LightMode mode) { directionalLightData_->mode = mode; }

	void toggleWireframeMode() { wireframeMode_ = !wireframeMode_; }

private:
	std::vector<RenderData_Model*> modelDrawList_{};
	std::vector<RenderData_Triangle*> triangleDrawList_{};
	std::vector<RenderData_Rect*> rectDrawList_{};
	std::vector<RenderData_Sprite*> spriteDrawList_{};
	std::vector<RenderData_Line*> lineDrawList_{};
	std::vector<RenderData_Particle*> particleDrawList_{};

	void InitializeResource_Light();
	void InitializeResource_LightPerObject();
	void InitializeResource_Camera();
	void InitializeResource_Material();
	void InitializeResource_WVPMatrix();
	void InitializeResource_VertexBuffer();
	void InitializeResource_IndexBuffer();


	// 動的頂点バッファの確保
	bool EnsureDynamicVB(size_t requiredVertexCount);

	// DirectXマネージャー
	DirectXManager* dxManager_ = nullptr;

	// カメラビュープロジェクション行列
	Matrix4x4 viewProjectionMatrix_;
	// 正射影行列
	Matrix4x4 orthoProjectionMatrix_;


	// プリミティブモード
	bool wireframeMode_ = false;




	// マテリアル
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> materialResources_{};
	// マテリアルデータの永続Mapポインタ
	std::vector<Material*> materialData_{};

	// ワールドビュー射影行列
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> wvpResources_{};
	// ワールドビュー射影行列の永続Mapポインタ
	std::vector<TransformationMatrix*> wvpData_{};

	// オブジェクト固有ライト
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> lightResources_{};
	// オブジェクト固有ライトデータの永続Mapポインタ
	std::vector<DirectionalLight*> lightData_{};

	// 頂点バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_ = nullptr;
	// 頂点データの永続Mapポインタ
	VertexData* vertexMappedPtr_ = nullptr;
	std::vector<VertexData> vertexData_{};
	UINT vertexResourceSize_ = 0;

	// カメラ
	Microsoft::WRL::ComPtr<ID3D12Resource> cameraResource_ = nullptr;
	// カメラデータの永続Mapポインタ
	CameraForGPU* cameraData_ = nullptr;

	// 共有ライト
	Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource_ = nullptr;
	// 共有ライトデータの永続Mapポインタ
	DirectionalLight* directionalLightData_ = nullptr;


	// 現フレームで描画されている頂点数(モデルは除く)
	size_t vertexDataUsed_ = 0;

	// 描画コールカウント
	size_t drawCallIndex_ = 0;

	// 1フレームに呼び出せる描画コールの最大数
	size_t kMaxDrawCallPerFrame_ = 1024;





	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource;
	D3D12_INDEX_BUFFER_VIEW indexBufferView;




private:
	// リサイズで入れ替えた古いVBをフレーム終了まで保持
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> vbHoldUntilSubmit_;

};

