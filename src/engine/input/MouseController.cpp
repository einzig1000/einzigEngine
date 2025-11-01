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

bool MouseController::GetMousePress(int i) const
{
    // 左クリック
    if (i == 0)
    {
        return Buttens.leftButton;
    }
    // 右クリック
    else if (i == 1)
    {
        return Buttens.rightButton;
    }
    // ミドルボタン（マウスホイールクリック）
    else if (i == 2)
    {
        return Buttens.middleButton;
    }

    return false;
}

bool MouseController::GetMousePrePress(int i) const
{
    // 左クリック
    if (i == 0)
    {
        return preButtens.leftButton;
    }
    // 右クリック
    else if (i == 1)
    {
        return preButtens.rightButton;
    }
    // ミドルボタン（マウスホイールクリック）
    else if (i == 2)
    {
        return preButtens.middleButton;
    }

    return false;
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
    Matrix4x4 inverseViewProj = cameraManager_->GetViewProjectionMatrix().Inverse();

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
	preButtens = Buttens;

    Buttens.leftButton = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
    Buttens.rightButton = (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0;
    Buttens.middleButton = (GetAsyncKeyState(VK_MBUTTON) & 0x8000) != 0;
}