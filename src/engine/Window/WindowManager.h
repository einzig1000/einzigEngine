#pragma once
#include <Windows.h>        // ウィンドウ関連のAPI
#include <string>           // std::wstring 用
#include <mmsystem.h>

class WindowManager {
public:
    WindowManager(int width, int height, const std::wstring& title);
    ~WindowManager();

    HWND GetHwnd() const { return hwnd; }

    // 追加: フルスクリーン制御API
    void SetFullscreen(bool enable);
    void ToggleFullscreen();
    bool IsFullscreen() const { return isFullscreen; }

    void Quit();

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