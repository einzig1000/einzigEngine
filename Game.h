#pragma once
#include "WindowManager.h"  // ウィンドウ管理
#include "DirectXManager.h" // DirectX管理
#include "definition.h"


class Game {
public:
    Game(WindowManager& windowManager, DirectXManager& dxManager);
    void Run();

    // objファイルで読み込むタイプのものを読み込む関数
    void LoadOBJ(const std::string& directoryPath, const std::string& filename);

private:
    WindowManager& windowManager;
    DirectXManager& dxManager;

    void Update();
    void Render();



    // objファイルで読み込むタイプのもの
    std::vector<Object3D> objects;

    // 光源
    Microsoft::WRL::ComPtr<ID3D12Resource>  directionalLightResource;
    DirectionalLigft* directionalLightData;
};
