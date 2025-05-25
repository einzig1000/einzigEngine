#pragma once
#include "WindowManager.h"
#include "DirectXManager.h"
#include "definition.h"
#include "CameraController.h"
#include <array>
#include <vector>
#include <string>
#include <wrl/client.h>

class Game {
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
    static void Drawobj(const Transforms& transform, const Vector3& center, uint32_t objectNumber, uint32_t textureNumber, const Vector4& materialColor);
    static void DrawTriangle(const Transforms& localTransform, const Transforms& worldTransform, const VertexData* vertexData, uint32_t kSumVertex, uint32_t textureNumber, const Vector4& materialColor);
    static void DrawSphere(const Transforms& localTransform, VertexData* vertexData, uint32_t kSubdivision, uint32_t textureNumber, const Vector4& materialColor);
    static void DrawSprite(const Transforms& localTransform, VertexData* vertexData, uint32_t textureNumber, const Vector4& materialColor);

    // マウス
    static void GetMousePosition(Vector2* position);
    static bool IsPressMouse(int i);
    static int GetWheel();


private:
    static void UpdateCameraAndLight();

    static WindowManager* windowManager;
    static DirectXManager* dxManager;

    // モデル
    static std::vector<Object3D> objects;
    static uint32_t objectSum;

    // 画像
    static std::vector<TextureData> textures;
    static uint32_t textureSum;

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
    static int wheelDelta;
};
