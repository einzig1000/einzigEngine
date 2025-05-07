#include "WindowManager.h"

WindowManager::WindowManager(HINSTANCE hInstance, int width, int height, const std::wstring& title) {
    RegisterWindowClass(hInstance);
    CreateMainWindow(hInstance, width, height, title);
}

WindowManager::~WindowManager() {
    CloseWindow(hwnd);
}

void WindowManager::RegisterWindowClass(HINSTANCE hInstance) {
    wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.lpszClassName = L"CG2WindowClass";
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    RegisterClass(&wc);
}

void WindowManager::CreateMainWindow(HINSTANCE hInstance, int width, int height, const std::wstring& title) {
    RECT wrc = { 0, 0, width, height };
    AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

    hwnd = CreateWindow(
        wc.lpszClassName, title.c_str(), WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        wrc.right - wrc.left, wrc.bottom - wrc.top,
        nullptr, nullptr, hInstance, nullptr
    );

    ShowWindow(hwnd, SW_SHOW);
}