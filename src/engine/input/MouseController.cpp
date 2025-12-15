#include "input/MouseController.h"
#include "Utilities/functions.h"
#include "Window/WindowManager.h"
#include "Camera/CameraManager.h"
#include "Game.h"


MouseController::MouseController(HWND hwnd, CameraManager* cameraManager)
	:cameraManager_(cameraManager)
{
    hwnd_ = hwnd;
    wheelDelta_ = 0;
	isVisible_ = true;
}

void MouseController::Update()
{
    // マウスポジション取得
    SetMousePosition();

    // マウスレイ取得
    SetMouseRay();

    // マウスボタン状態取得
    SetMouseButtenState();

    // FPSカメラでマウスカーソルがウィンドウ外に出ないようにする
    if (!isVisible_)
    {
		int screenX = WindowManager::winWidth_ / 2;
		int screenY = WindowManager::winHeight_ / 2;

        SetCursorPos(screenX, screenY);
    }
}

void MouseController::EndFrame()
{
    wheelDelta_ = 0;
}

// 今押しているか  i: 0=左ボタン、1=右ボタン、2=中ボタン
bool MouseController::IsHeld(int i)
{
    switch (i)
    {
    case 0:
        return leftButton_.curr;
    case 1:
        return rightButton_.curr;
    case 2:
        return middleButton_.curr;
    default:
        return false;
    }
}

// 押した瞬間（今フレームで押された） i: 0=左ボタン、1=右ボタン、2=中ボタン
bool MouseController::IsJustPressed(int i)
{
    switch (i)
    {
    case 0:
        return (!leftButton_.prev && leftButton_.curr);
    case 1:
        return (!rightButton_.prev && rightButton_.curr);
    case 2:
        return (!middleButton_.prev && middleButton_.curr);
    default:
        return false;
    }
}

// 離した瞬間（今フレームで離れた） i: 0=左ボタン、1=右ボタン、2=中ボタン
bool MouseController::IsJustReleased(int i)
{
    switch (i)
    {
    case 0:
        return (leftButton_.prev && !leftButton_.curr);
    case 1:
        return (rightButton_.prev && !rightButton_.curr);
    case 2:
        return (middleButton_.prev && !middleButton_.curr);
    default:
        return false;
    }
}

// 押されてからの経過フレーム数 i: 0=左ボタン、1=右ボタン、2=中ボタン
uint32_t MouseController::HoldFrames(int i)
{
    switch (i)
    {
    case 0:
        return leftButton_.holdFrames;
    case 1:
        return rightButton_.holdFrames;
    case 2:
        return middleButton_.holdFrames;
    default:
        return 0;
    }
}

// マウスカーソルの表示・非表示切り替え
void MouseController::ToggleMouseCursorVisible()
{
    isVisible_ = !isVisible_;
    if (isVisible_)
    {
        // カーソルを表示
        ShowCursor(TRUE);
    }
    else
    {
        // カーソルを非表示
        ShowCursor(FALSE);
	}
}

// マウスカーソルの表示・非表示設定
void MouseController::ShowCursor(bool visible)
{
    if (visible)
    {
        // カーソルを表示
        ::ShowCursor(TRUE);
    }
    else
    {
        // カーソルを非表示
        ::ShowCursor(FALSE);
	}
}

// マウスポジション取得
void MouseController::SetMousePosition()
{
    // hwnd: ゲームウィンドウのハンドル（WindowManagerなどから取得）
    POINT mousePosScreen;
    GetCursorPos(&mousePosScreen); // 画面座標で取得

    // クライアント座標（ウィンドウ左上基準）に変換
    ScreenToClient(hwnd_, &mousePosScreen);

    // mousePosScreen.x, mousePosScreen.y がウィンドウ内のマウス座標
    position_ = Vector2{ float(mousePosScreen.x),float(mousePosScreen.y) };
}

// マウスレイ取得
void MouseController::SetMouseRay()
{
    // 左下が０、右上が１とした時のマウスポジション
    float ndcX = (position_.x / WindowManager::winWidth_) * 2.0f - 1.0f;
    float ndcY = 1.0f - (position_.y / WindowManager::winHeight_) * 2.0f; // Yは上下反転

    // クリップ空間でZ=0(near)とZ=1(far)の2点を作る
    Vector4 nearPoint = { ndcX, ndcY, 0.0f, 1.0f };
    Vector4 farPoint = { ndcX, ndcY, 1.0f, 1.0f };

    // 逆射影行列
    Matrix4x4 inverseViewProj = cameraManager_->GetCurrentViewProjectionMatrix().Inverse();

    // ワールド空間に変換
    Vector4 nearWorld = Transform(nearPoint, inverseViewProj);
    Vector4 farWorld = Transform(farPoint, inverseViewProj);

    // マウスレイの始点・方向
    ray_.origin = { nearWorld.x / nearWorld.w, nearWorld.y / nearWorld.w, nearWorld.z / nearWorld.w };
    ray_.diff = Vector3{
    (farWorld.x / farWorld.w) - ray_.origin.x,
    (farWorld.y / farWorld.w) - ray_.origin.y,
    (farWorld.z / farWorld.w) - ray_.origin.z
    }.Normalized();
}

// マウスボタン状態取得
void MouseController::SetMouseButtenState()
{
    leftButton_.prev = leftButton_.curr;
	rightButton_.prev = rightButton_.curr;
	middleButton_.prev = middleButton_.curr;

    leftButton_.curr = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
    rightButton_.curr = (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0;
    middleButton_.curr = (GetAsyncKeyState(VK_MBUTTON) & 0x8000) != 0;

    if (leftButton_.curr)leftButton_.holdFrames++;
    else leftButton_.holdFrames = 0;
    if (rightButton_.curr) rightButton_.holdFrames++;
    else rightButton_.holdFrames = 0;
    if (middleButton_.curr) middleButton_.holdFrames++;
    else middleButton_.holdFrames = 0;
}