#pragma once
#include "WindowManager.h"  // ウィンドウ管理
#include "DirectXManager.h" // DirectX管理


class Game {
public:
    Game(WindowManager& windowManager, DirectXManager& dxManager);
    void Run();

private:
    WindowManager& windowManager;
    DirectXManager& dxManager;

    void Update();
    void Render();
};
