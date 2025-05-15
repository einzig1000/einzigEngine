#pragma once
#include "WindowManager.h"
#include "DirectXManager.h"
#include "definition.h"
#include <array>
#include <vector>
#include <string>
#include <wrl/client.h>

class Game {
public:
    Game(int width, int height, const std::wstring& title);
    ~Game();

    // 初期化
    static void Initialize(int width, int height, const std::wstring& title);

    // メインループ用
    static bool ProcessMessage();
    static void BeginFrame();
    static void EndFrame();

    // 解放
    static void Finalize();

    // LoadResource
    static int LoadOBJ(const std::string& directoryPath, const std::string& filename);
    static int LoadTexture(const std::string& filePath);

    // 描画
    static void Drawobj(const Transforms& localTransform, uint32_t objectNumber, uint32_t textureNumber, size_t matrixIndex);

private:
    static void UpdateCameraAndLight();

    // 静的メンバ（ここでは宣言のみ。初期化はGame.cppで行う）
    static WindowManager* windowManager;
    static DirectXManager* dxManager;

    static std::vector<Object3D> objects;
    static uint32_t objectSum;

    static std::vector<textureData> textures;
    static uint32_t textureSum;

    static Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource;
    static DirectionalLigft* directionalLightData;

    static Transforms cameraTransform;
    static Matrix4x4 viewMatrix;
    static Matrix4x4 projectionMatrix;

    static int uvCheckerTex;
};
