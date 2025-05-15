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
    uint32_t Getwidth() const { return width_; }
    uint32_t Getheight() const { return height_; }

private:
    HWND hwnd;
    WNDCLASS wc;

    uint32_t width_;
    uint32_t height_;

    void RegisterWindowClass();
    void CreateMainWindow(int width, int height, const std::wstring& title);
};