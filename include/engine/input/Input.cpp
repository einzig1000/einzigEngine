#include "Input.h"
bool Input::left;
bool Input::right;
bool Input::jump;
bool Input::hide;
bool Input::Pause;
bool Input::preHide;

Input::Input(HWND hwnd, uint32_t width, uint32_t height, Matrix4x4* viewProjectionMatrix, Matrix4x4* debugViewProjectionMatrix, bool* debugCameraMode)
{
    getHitKey_ = std::make_unique<GetHitKey>(hwnd);
    getPadState_ = std::make_unique<GetPadState>();
    mouseController_ = std::make_unique<MouseController>(hwnd, width, height, viewProjectionMatrix, debugViewProjectionMatrix, debugCameraMode);
}

void Input::Update()
{
    getHitKey_->Update();
    getPadState_->Update();
    mouseController_->Update();

    //if (GetHitKey::keys[DIK_A] ||
    //    GetPadState::leftStickX < -10000 ||
    //    GetPadState::buttons[PAD_LEFT])
    //{
    //    left = true;
    //}
    //else
    //{
    //    left = false;
    //}
    //
    //if (GetHitKey::keys[DIK_D] ||
    //    GetPadState::leftStickX > 10000 ||
    //    GetPadState::buttons[PAD_RIGHT])
    //{
    //    right = true;
    //}
    //else
    //{
    //    right = false;
    //}
    //
    //if (GetHitKey::keys[DIK_W] ||
    //    GetPadState::buttons[PAD_A])
    //{
    //    jump = true;
    //}
    //else
    //{
    //    jump = false;
    //}
    //
    //if (GetHitKey::keys[DIK_SPACE] ||
    //    GetPadState::buttons[PAD_B] ||
    //    GetPadState::buttons[PAD_X])
    //{
    //    hide = true;
    //}
    //else
    //{
    //    hide = false;
    //}
    //
    //if (GetHitKey::keys[DIK_ESCAPE] ||
    //    GetPadState::buttons[PAD_START])
    //{
    //    Pause = true;
    //}
    //else
    //{
    //    Pause = false;
    //}
    //
    //if (GetHitKey::preKeys[DIK_SPACE] ||
    //    GetPadState::preButtons[PAD_B] ||
    //    GetPadState::preButtons[PAD_X]
    //    )
    //{
    //    preHide = true;
    //}
    //else
    //{
    //    preHide = false;
    //}
}

void Input::EndFrame()
{
    mouseController_->EndFrame();
}