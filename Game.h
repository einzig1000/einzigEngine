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
    static void Drawobj(const Transforms& localTransform, const Transforms& worldTransform, uint32_t objectNumeber, uint32_t textureNumber, const Vector4& materialColor);
    static void DrawTriangle(const Transforms& localTransform, const Transforms& worldTransform, const VertexData* vertexData, uint32_t kSumVertex, uint32_t textureNumber, const Vector4& materialColor);
    static void DrawSphere(const Transforms& localTransform, VertexData* vertexData, uint32_t kSubdivision, uint32_t textureNumber, const Vector4& materialColor);
    static void DrawSprite(const Transforms& localTransform, VertexData* vertexData, uint32_t textureNumber, const Vector4& materialColor);

    // マウス
    static void GetMousePosition(Vector2* position);
    static bool IsPressMouse(int i);
    static int GetWheel();

    // ImGui
    //static Transforms* GetTransforms(int i) { return &objects[i].transform; }
    //static Transforms cameraTransform;

private:
    static void UpdateCameraAndLight();

    static WindowManager* windowManager;
    static DirectXManager* dxManager;

    static std::vector<Object3D> objects;
    static uint32_t objectSum;

    static std::vector<TextureData> textures;
    static uint32_t textureSum;

    static Microsoft::WRL::ComPtr<ID3D12Resource> indexResource;
    static D3D12_INDEX_BUFFER_VIEW indexBufferView;

    static Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource;
    static DirectionalLight* directionalLightData;

    //static Matrix4x4 viewMatrix;
    //static Matrix4x4 projectionMatrix;

    static CameraController* cameraController;
    static int wheelDelta_; // ホイール量累積
};
