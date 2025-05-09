#include "Game.h"           // クラス定義


Game::Game(WindowManager& windowManager, DirectXManager& dxManager)
    : windowManager(windowManager), dxManager(dxManager) {
}

void Game::Run()
{
    MSG msg = {};
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            Update();
            Render();
        }
    }
}

void Game::Update()
{
    // ゲームロジックの更新


}

void Game::Render() 
{
    dxManager.BeginFrame();
    // 描画処理



    dxManager.EndFrame();
}