#pragma once
#include <Windows.h>        // ウィンドウ関連のAPI
#include <string>           // std::wstring 用



class WindowManager {
public:
    WindowManager(int width, int height, const std::wstring& title);
    ~WindowManager();

    HWND GetHwnd() const { return hwnd; }

private:
    HWND hwnd;
    WNDCLASS wc;

    void RegisterWindowClass();
    void CreateMainWindow(int width, int height, const std::wstring& title);
};