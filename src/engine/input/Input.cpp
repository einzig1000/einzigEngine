#include "Input.h"

Input::Input(HWND hwnd, CameraManager* cameraManager)
{
    getHitKey_ = std::make_unique<GetHitKey>(hwnd);
    getPadState_ = std::make_unique<GetPadState>();
    mouseController_ = std::make_unique<MouseController>(hwnd, cameraManager);
}

void Input::Update()
{
    getHitKey_->Update();
    getPadState_->Update();
    mouseController_->Update();
}

void Input::EndFrame()
{
    mouseController_->EndFrame();
}