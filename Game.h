#pragma once
#include "WindowManager.h"  // ウィンドウ管理
#include "DirectXManager.h" // DirectX管理
#include "definition.h"


class Game {
public:
    Game(WindowManager& windowManager, DirectXManager& dxManager);
    void Run();

private:
    WindowManager& windowManager;
    DirectXManager& dxManager;

    void Update();
    void Render();



    // objファイルで読み込むタイプのもの
    std::vector<Object3D> objects;
    // objファイルで読み込むタイプのものを読み込む関数
    void LoadOBJ(const std::string& directoryPath, const std::string& filename);
};
