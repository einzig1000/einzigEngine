#pragma once
#include <Windows.h> // ウィンドウ関連のAPI
#include <string>    // std::wstring 用



class WindowManager {
public:
    WindowManager(HINSTANCE hInstance, int width, int height, const std::wstring& title);
    ~WindowManager();

    HWND GetHwnd() const { return hwnd; }

private:
    HWND hwnd;
    WNDCLASS wc;

    void RegisterWindowClass(HINSTANCE hInstance);
    void CreateMainWindow(HINSTANCE hInstance, int width, int height, const std::wstring& title);
};