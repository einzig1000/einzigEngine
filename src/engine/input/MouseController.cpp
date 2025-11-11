#include "input/MouseController.h"
#include "Utilities/functions.h"
#include "Window/WindowManager.h"
#include "Camera/CameraManager.h"
#include "Game.h"


MouseController::MouseController(HWND hwnd, CameraManager* cameraManager)
	:cameraManager_(cameraManager)
{
    hwnd_ = hwnd;
    wheelDelta = 0;
	isVisible = true;
}

void MouseController::Update()
{
    // マウスポジション取得
    SetMousePosition();

    // マウスレイ取得
    SetMouseRay();

    // マウスボタン状態取得
    SetMouseButtenState();
}

void MouseController::EndFrame()
{
    wheelDelta = 0;
}

bool MouseController::IsHeld(int i)
{
    switch (i)
    {
    case 0:
        return leftButton.curr;
    case 1:
        return rightButton.curr;
    case 2:
        return middleButton.curr;
    default:
        return false;
    }
}

bool MouseController::IsJustPressed(int i)
{
    switch (i)
    {
    case 0:
        return (!leftButton.prev && leftButton.curr);
    case 1:
        return (!rightButton.prev && rightButton.curr);
    case 2:
        return (!middleButton.prev && middleButton.curr);
    default:
        return false;
    }
}

bool MouseController::IsJustReleased(int i)
{
    switch (i)
    {
    case 0:
        return (leftButton.prev && !leftButton.curr);
    case 1:
        return (rightButton.prev && !rightButton.curr);
    case 2:
        return (middleButton.prev && !middleButton.curr);
    default:
        return false;
    }
}

uint32_t MouseController::HoldFrames(int i)
{
    switch (i)
    {
    case 0:
        return leftButton.holdFrames;
    case 1:
        return rightButton.holdFrames;
    case 2:
        return middleButton.holdFrames;
    default:
        return 0;
    }
}

void MouseController::ToggleMouseCursorVisible()
{
    isVisible = !isVisible;
    if (isVisible)
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

void MouseController::SetMouseButtenState()
{
    leftButton.prev = leftButton.curr;
	rightButton.prev = rightButton.curr;
	middleButton.prev = middleButton.curr;

    leftButton.curr = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
    rightButton.curr = (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0;
    middleButton.curr = (GetAsyncKeyState(VK_MBUTTON) & 0x8000) != 0;

    if (leftButton.curr)leftButton.holdFrames++;
    else leftButton.holdFrames = 0;
    if (rightButton.curr) rightButton.holdFrames++;
    else rightButton.holdFrames = 0;
    if (middleButton.curr) middleButton.holdFrames++;
    else middleButton.holdFrames = 0;
}