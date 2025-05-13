#pragma once
#include "WindowManager.h"  // ウィンドウ管理
#include "DirectXManager.h" // DirectX管理
#include "definition.h"


class Game {
public:
    Game(WindowManager& windowManager, DirectXManager& dxManager);
    void Run();

    // objファイルで読み込むタイプのものを読み込む関数
    int LoadOBJ(const std::string& directoryPath, const std::string& filename);

private:
    WindowManager& windowManager;
    DirectXManager& dxManager;

    void Update();
    void Render();

    void Drawobj(uint32_t objectNumeber);

    // objファイルで読み込むタイプのもの
    std::vector<Object3D> objects;
    uint32_t objectSum;// 読み込んだオブジェクトの合計

    // 光源
    Microsoft::WRL::ComPtr<ID3D12Resource>  directionalLightResource;
    DirectionalLigft* directionalLightData;

    // リソース読み込み
    int obj1 = LoadOBJ("resources", "plane.obj");
    int obj2 = LoadOBJ("resources", "axis.obj");
    int obj3 = LoadOBJ("resources", "multiMaterial.obj");
    int obj4 = LoadOBJ("resources", "multiMesh.obj");
};
