#pragma once
#include "WindowManager.h"  // ウィンドウ管理
#include "DirectXManager.h" // DirectX管理
#include "definition.h"
#include <array>

class Game {
public:
    Game(WindowManager& windowManager, DirectXManager& dxManager);
    ~Game();
    void Run();


    // モデルデータ
    int LoadOBJ(const std::string& directoryPath, const std::string& filename);
    void Drawobj(const Transforms& localTransform, uint32_t objectNumeber, uint32_t textureNumber, size_t matrixIndex);

    // テクスチャデータ
    int LoadTexture(const std::string& filePath);

    void Update();
    void UpdateCameraAndLight();
    void ImGuiUpdate();

    void Render();
    void Draw();

    int ProcessMessage();
    void BeginFrame();
    void EndFrame();

    DirectXManager& dxManager;
private:
    WindowManager& windowManager;



    // モデル
    std::vector<Object3D> objects;
    uint32_t objectSum;// 読み込んだオブジェクトの合計

    // テクスチャ
    std::vector<textureData> textures;
    uint32_t textureSum;// 読み込んだテクスチャの合計

    // 光源
    Microsoft::WRL::ComPtr<ID3D12Resource>  directionalLightResource;
    DirectionalLigft* directionalLightData;

    // カメラ
    Transforms cameraTransform;
    Matrix4x4 viewMatrix;
    Matrix4x4 projectionMatrix;

    // リソース読み込み
    int uvCheckerTex;
    int monsterBallTex;
    int gold1x1Tex;

    int obj1;
    int obj2;
    int obj3;
    int obj4;


    Transforms transformOBJ1;
    Transforms transformOBJ2;



    // ImGui用変数
    int item_current;
    bool autoRotation[3] = { 0,0,0 };
};
