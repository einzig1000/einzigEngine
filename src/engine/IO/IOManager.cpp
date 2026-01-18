#include "IO/IOManager.h"

IOManager::IOManager(HWND hwnd, CameraManager* cameraManager)
{
    getHitKey_ = std::make_unique<GetHitKey>(hwnd);
    getPadState_ = std::make_unique<GetPadState>();
    mouseController_ = std::make_unique<MouseController>(hwnd, cameraManager);
}

void IOManager::Update()
{
    getHitKey_->Update();
    getPadState_->Update();
    mouseController_->Update();
}

void IOManager::EndFrame()
{
    mouseController_->EndFrame();
}