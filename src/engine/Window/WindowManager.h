#pragma once
#include <Windows.h>        // ウィンドウ関連のAPI
#include <string>           // std::wstring 用
#include <mmsystem.h>

class MouseController;  // 無念ながら前方宣言

class WindowManager {
public:
    WindowManager(int width, int height, const std::wstring& title);
    ~WindowManager();

    void AttachMouseController(MouseController* mc);

	// ウィンドウハンドル取得
    HWND GetHwnd() const { return hwnd; }

	// フルスクリーン制御
    void SetFullscreen(bool enable);
    void ToggleFullscreen();
    bool IsFullscreen() const { return isFullscreen; }

    void Quit();

    // ウィンドウサイズ
    static uint32_t winWidth_;
    static uint32_t winHeight_;

	// アクティブフラグ(ウィンドウが最小化されている時とかはfalse)
	bool isActive_ = true;

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

	// ウィンドウクラス登録
    void RegisterWindowClass();
    // 
    void RegisterMouseRawInput(HWND hwnd);
	// メインウィンドウ生成
    void CreateMainWindow(int width, int height, const std::wstring& title);
};