#pragma once
#include "input/GetHitKey.h"
#include "input/GetPadState.h"
#include "input/MouseController.h"
#include <memory>

class CameraManager;

class Input
{
public:
    Input(HWND hwnd, CameraManager* cameraManager);
    void Update();
    void EndFrame();

    MouseController* GetMouseController() const { return mouseController_.get(); }
    GetHitKey* GetGetHitKey() const { return getHitKey_.get(); }
    GetPadState* GetGetPadState() const { return getPadState_.get(); }

private:

    std::unique_ptr<GetHitKey> getHitKey_;
    std::unique_ptr<GetPadState> getPadState_;
    std::unique_ptr<MouseController> mouseController_;
};