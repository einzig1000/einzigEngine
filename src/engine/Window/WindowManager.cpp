#include "Window/WindowManager.h"
#include "ImGuiManager/ImGuiManager.h"
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#pragma comment(lib, "winmm.lib")

uint32_t WindowManager::winWidth_;
uint32_t WindowManager::winHeight_;

// ウィンドウプロシージャ(クリックした、×を押した等のイベントを処理する関数)
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    // Imgui用
    if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam))
    {
        return true;
    }

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


WindowManager::WindowManager(int width, int height, const std::wstring& title)
{
	winWidth_ = width;
	winHeight_ = height;
	timeBeginPeriod(1); // タイマー精度を1msに設定
    RegisterWindowClass();
    CreateMainWindow(width, height, title);
}

WindowManager::~WindowManager()
{
    // フルスクリーン中なら復帰してから破棄
    if (isFullscreen)
    {
        ExitBorderlessFullscreen();
    }
    // DestroyWindow の方が確実に破棄できます
    if (hwnd)
    {
        DestroyWindow(hwnd);
        hwnd = nullptr;
    }
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

void WindowManager::CreateMainWindow(int width, int height, const std::wstring& title) 
{
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

void WindowManager::SetFullscreen(bool enable)
{
	// すでにその状態なら何もしない
    if (enable == isFullscreen) return;
	// フルスクリーン切り替え
    if (enable)
    {
        EnterBorderlessFullscreen();
    }
	// 解除
    else
    {
        ExitBorderlessFullscreen();
    }
	// ウィンドウサイズ更新
	UpdateClientSize();
}

void WindowManager::ToggleFullscreen()
{
    SetFullscreen(!isFullscreen);
}

void WindowManager::Quit()
{
	PostQuitMessage(0);
}

// isFullscreen = trueになる
void WindowManager::EnterBorderlessFullscreen()
{
    if (!hwnd) return;

    // 現在のウィンドウ情報を保存
    windowedStyle = static_cast<DWORD>(GetWindowLongPtr(hwnd, GWL_STYLE));
    windowedExStyle = static_cast<DWORD>(GetWindowLongPtr(hwnd, GWL_EXSTYLE));
    windowedPlacement.length = sizeof(WINDOWPLACEMENT);
    GetWindowPlacement(hwnd, &windowedPlacement);

    // 対象モニタのワークエリアではなくモニタ全体を使用
    HMONITOR hMon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    GetMonitorInfo(hMon, &mi);

    // ボーダーレスにしてモニタ全体へフィット
    SetWindowLongPtr(hwnd, GWL_STYLE, windowedStyle & ~(WS_OVERLAPPEDWINDOW));
    SetWindowLongPtr(hwnd, GWL_EXSTYLE, windowedExStyle | WS_EX_APPWINDOW);

    SetWindowPos(
        hwnd,
        HWND_TOP,
        mi.rcMonitor.left,
        mi.rcMonitor.top,
        mi.rcMonitor.right - mi.rcMonitor.left,
        mi.rcMonitor.bottom - mi.rcMonitor.top,
        SWP_NOOWNERZORDER | SWP_FRAMECHANGED | SWP_SHOWWINDOW
    );

    isFullscreen = true;
}

// isFullscreen = falseになる
void WindowManager::ExitBorderlessFullscreen()
{
    if (!hwnd) return;

    // 元のスタイルへ戻す
    SetWindowLongPtr(hwnd, GWL_STYLE, windowedStyle);
    SetWindowLongPtr(hwnd, GWL_EXSTYLE, windowedExStyle);

    // ウィンドウ配置と枠を復元
    SetWindowPlacement(hwnd, &windowedPlacement);
    SetWindowPos(
        hwnd,
        nullptr,
        0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED | SWP_SHOWWINDOW
    );

    isFullscreen = false;
}

void WindowManager::UpdateClientSize()
{
    if (!hwnd) return;
    RECT rc{};
    GetClientRect(hwnd, &rc);
    winWidth_ = static_cast<uint32_t>(rc.right - rc.left);
    winHeight_ = static_cast<uint32_t>(rc.bottom - rc.top);
}