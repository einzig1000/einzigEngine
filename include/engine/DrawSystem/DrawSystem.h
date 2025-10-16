#pragma once
#include "definition/definition.h"
#include "Game.h"

class DirectXManager;

class DrawSystem
{
public:
	DrawSystem(DirectXManager* dxManager);
	~DrawSystem();

	void BeginFrame(Matrix4x4& viewProjectionMatrix);

	void DrawModel(Game::RenderData_Model& renderData);
	void DrawTriangle(Game::RenderData_Triangle& renderData);
	void DrawSprite(Game::RenderData_Sprite& renderData);
	void DrawParticle(Game::RenderData_Particle& renderData);
	void DrawLine(const Vector3& start, const Vector3& end, const uint32_t& materialColor);

	void SetLightColor(const Vector4 color) { directionalLightData_->color = color; }
	void SetLightDirection(const Vector3 direction) { directionalLightData_->direction = direction; }
	void SetLightIntensity(float intensity) { directionalLightData_->intensity = intensity; }
	void ToggleLightMode(const uint32_t mode) { directionalLightData_->mode = mode; }

	void toggleWireframeMode(bool mode) { wireframeMode_ = mode; }

private:
	uint32_t instancingSrvIndex_ = UINT32_MAX;


	// 動的頂点バッファの確保
	bool EnsureDynamicVB(size_t requiredVertexCount);

	// インスタンシング用バッファの確保/拡張
	bool EnsureInstanceBuffer(size_t requiredInstanceCount);

	// DirectXマネージャー
	DirectXManager* dxManager_ = nullptr;

	// カメラビュープロジェクション行列
	Matrix4x4 viewProjectionMatrix_;

	// プリミティブモード
	bool wireframeMode_ = false;

	// ライト
	Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource_ = nullptr;
	DirectionalLight* directionalLightData_ = nullptr;


	// 描画コールカウント
	size_t drawCallIndex_ = 0;
	// 1フレームに呼び出せる描画コールの最大数
	size_t kMaxDrawCallPerFrame_ = 1280;
	// 現フレームで描画されている頂点数(モデルは除く)
	size_t vertexDataUsed_ = 0;

	// 描画コールごとのデータ
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> materialResources_{};
	// マテリアルデータの永続Mapポインタ
	std::vector<Material*> materialData_{};
	// ワールドビュー射影行列
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> wvpResources_{};
	// ワールドビュー射影行列の永続Mapポインタ
	std::vector<TransformationMatrix*> wvpData_{};

	// 三角形
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_ = nullptr;
	UINT vertexResourceSize_ = 0;
	std::vector<VertexData> vertexData_{};
	VertexData* vertexMappedPtr_ = nullptr; // 永続Mapポインタ


	// インスタンシング（構造化バッファ or Uploadバッファ＋SRV）
	Microsoft::WRL::ComPtr<ID3D12Resource> instancingResource_ = nullptr;
	// 
	TransformationMatrix* instancingData_ = nullptr; // 永続Mapポインタ

	uint32_t instancingCapacity_ = 0; // いま確保している最大インスタンス数
	const uint32_t kNumInstance_ = 10;
	D3D12_CPU_DESCRIPTOR_HANDLE instancingSrvHandleCPU_;
	D3D12_GPU_DESCRIPTOR_HANDLE instancingSrvHandleGPU_;



};

