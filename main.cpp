#include "WindowManager.h"  // ウィンドウ管理
#include "DirectXManager.h" // DirectX管理
#include "Game.h"           // ゲームロジック


int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    WindowManager windowManager(1280, 720, L"CG2");
    DirectXManager dxManager(windowManager.GetHwnd(), 1280, 720);
    Game game(windowManager, dxManager);

    game.Run();
    return 0;
}
