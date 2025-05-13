#include "WindowManager.h"  // ウィンドウ管理
#include "DirectXManager.h" // DirectX管理
#include "Game.h"           // ゲームロジック
#include "functions.h"      // 関数s




int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    // COM の初期化
    HRESULT hr = CoInitializeEx(0, COINIT_MULTITHREADED);
    assert(SUCCEEDED(hr));
    // 例外ハンドラの設定
    SetUnhandledExceptionFilter(ExportDump);

    WindowManager windowManager(1280, 720, L"CG2");
    DirectXManager dxManager(windowManager.GetHwnd(), 1280, 720);
    Game game(windowManager, dxManager);


    // リソース読み込み
    game.LoadOBJ("resources", "axis.obj");
    game.LoadOBJ("resources", "axis.obj");

    game.Run();
    return 0;
}
