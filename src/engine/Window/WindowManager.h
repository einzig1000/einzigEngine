#pragma once
#include <Windows.h>        // ウィンドウ関連のAPI
#include <string>           // std::wstring 用

#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);



class WindowManager {
public:
    WindowManager(int width, int height, const std::wstring& title);
    ~WindowManager();

    HWND GetHwnd() const { return hwnd; }

    // 追加: フルスクリーン制御API
    void SetFullscreen(bool enable);
    void ToggleFullscreen();
    bool IsFullscreen() const { return isFullscreen; }

    // ウィンドウサイズ
    static uint32_t winWidth_;
    static uint32_t winHeight_;

private:
    HWND hwnd;
    WNDCLASS wc;


    // フルスクリーン制御
    bool isFullscreen = false;
    DWORD windowedStyle = 0;
    DWORD windowedExStyle = 0;
    WINDOWPLACEMENT windowedPlacement{};

	// フルスクリーン制御用関数
	// フルスクリーンモードにする
    void EnterBorderlessFullscreen();
	// フルスクリーンモード解除
    void ExitBorderlessFullscreen();
	// クライアントサイズ更新
    void UpdateClientSize();

    void RegisterWindowClass();
    void CreateMainWindow(int width, int height, const std::wstring& title);
};