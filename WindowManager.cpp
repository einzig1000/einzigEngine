#include "WindowManager.h"

// ウィンドウプロシージャ(クリックした、×を押した等のイベントを処理する関数)
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    //// Imgui用
    //if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam))
    //{
    //    return true;
    //}

    // メッセージに応じてゲーム固有の処理を行う
    switch (msg)
    {
        // ウィンドウが破壊された
    case WM_DESTROY:
        // OSに対してアプリの終了を伝える
        PostQuitMessage(0);
        return 0;
    }

    // 標準のメッセージ処理を行う
    return DefWindowProc(hwnd, msg, wparam, lparam);
}


WindowManager::WindowManager(int width, int height, const std::wstring& title) {
    RegisterWindowClass();
    CreateMainWindow(width, height, title);
}

WindowManager::~WindowManager() {
    CloseWindow(hwnd);
}

void WindowManager::RegisterWindowClass() {
    // ウィンドウクラス作成
    wc = {};
    // ウィンドウプロシージャ
    wc.lpfnWndProc = WindowProc;
    // ウィンドウクラス名
    wc.lpszClassName = L"CG2WindowClass";
    // インスタンスハンドル
    wc.hInstance = GetModuleHandle(nullptr);
    // カーソル
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    // ウィンドウクラスを登録する
    RegisterClass(&wc);
}

void WindowManager::CreateMainWindow(int width, int height, const std::wstring& title) {
    // ウィンドウサイズを表す構造体にクライアント領域を入れる
    RECT wrc = { 0,0,width,height };
    // クライアント領域を元に実際のサイズのwrcを変更してもらう
    AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

    // ウィンドウの生成
    hwnd = CreateWindow(
        wc.lpszClassName,		// 利用するウィンドウクラス名
        title.c_str(), 			// タイトルバーに表示する文字
        WS_OVERLAPPEDWINDOW,	// ウィンドウスタイルの選択
        CW_USEDEFAULT,			// 表示X座標
        CW_USEDEFAULT,			// 表示Y座標
        wrc.right - wrc.left,	// ウィンドウ横幅
        wrc.bottom - wrc.top,	// ウィンドウ縦幅
        nullptr,				// 親ウィンドウハンドル
        nullptr,				// メニューハンドル
        wc.hInstance,			// インスタンスハンドル
        nullptr					// オプション
    );


    ShowWindow(hwnd, SW_SHOW);
}
