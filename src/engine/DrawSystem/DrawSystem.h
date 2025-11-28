#pragma once
#include "definition/definition.h"
#include "Game.h"

class DirectXManager;

namespace
{
	struct EmitterPool
	{
		Microsoft::WRL::ComPtr<ID3D12Resource> buffer;
		ParticleMonoInf* mapped = nullptr;
		uint32_t capacity = 0;
		uint32_t activeCount = 0;
		uint32_t srvIndex = UINT32_MAX;
		D3D12_CPU_DESCRIPTOR_HANDLE srvCPU{};
		D3D12_GPU_DESCRIPTOR_HANDLE srvGPU{};
	};
	std::unordered_map<RenderData_Particle*, EmitterPool> s_particlePools;
}

class DrawSystem
{
public:
	DrawSystem(DirectXManager* dxManager);
	~DrawSystem();
	void Update();
	void Draw();

	void SetViewProjectionMatrix(const Matrix4x4& viewProjectionMatrix) { viewProjectionMatrix_ = viewProjectionMatrix; }
	void Update_ParticleInstanceData();

	void AddModelDrawList(RenderData_Model& renderData);
	void DrawAllModel();
	void AddTriangleDrawList(RenderData_Triangle& renderData);
	void DrawAllTriangle();
	void AddRectDrawList(RenderData_Rect& renderData);
	void DrawAllRect();
	void AddSpriteDrawList(RenderData_Sprite& renderData);
	void DrawAllSprite();
	void AddLineDrawList(RenderData_Line& renderData);
	void DrawAllLine();
	void AddParticleDrawList(RenderData_Particle3& renderData);
	void DrawAllParticle();

	void DrawParticle(RenderData_Particle& renderData);


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
	std::vector<RenderData_Particle3*> particleDrawList_{};




	uint32_t instancingSrvIndex_ = UINT32_MAX;


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

	// ライト


	// 描画コールカウント
	size_t drawCallIndex_ = 0;
	// 1フレームに呼び出せる描画コールの最大数
	size_t kMaxDrawCallPerFrame_ = 1024;


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
	// 共有ライト
	Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource_ = nullptr;
	// 共有ライトデータの永続Mapポインタ
	DirectionalLight* directionalLightData_ = nullptr;
	// 現フレームで描画されている頂点数(モデルは除く)
	size_t vertexDataUsed_ = 0;

	// インスタンシング
	uint32_t kMaxInstanceCount_ = 4096;          // 最大インスタンス数


	// カメラ
	Microsoft::WRL::ComPtr<ID3D12Resource> cameraResource_ = nullptr;
	// カメラデータの永続Mapポインタ
	CameraForGPU* cameraData_ = nullptr;


	// 三角形
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_ = nullptr;
	VertexData* vertexMappedPtr_ = nullptr; // 永続Mapポインタ
	UINT vertexResourceSize_ = 0;
	std::vector<VertexData> vertexData_{};


	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource;
	D3D12_INDEX_BUFFER_VIEW indexBufferView;



	/////////////// マップ描画用 ///////////////////
	TransformationMatrix mapWVPMatrix_;
	const uint32_t knumInstance_Map = MAX_BLOCK_X * MAX_BLOCK_Y * MAX_BLOCK_Z;
	std::vector<Transforms> mapTransforms_;
	Microsoft::WRL::ComPtr<ID3D12Resource> mapInstanceResource;
	TransformationMatrix* mapInstanceData;
	SRVAllocation srvAlloc;


private:
	// リサイズで入れ替えた古いVBをフレーム終了まで保持
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> vbHoldUntilSubmit_;

};

