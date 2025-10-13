#pragma once
#include "input/GetHitKey.h"
#include "input/GetPadState.h"
#include "input/MouseController.h"
#include <memory>

class Input
{
public:
    Input(HWND hwnd, uint32_t width, uint32_t height, Matrix4x4* viewProjectionMatrix, Matrix4x4* debugViewProjectionMatrix, bool* debugCameraMode);
    void Update();


    MouseController* GetMouseController() const { return mouseController_.get(); }

    static bool left;
    static bool right;
    static bool jump;
    static bool hide;
    static bool Pause;

    static bool preHide;


private:
    GetPadState* GetGetPadState() const { return getPadState_.get(); }
    GetHitKey* GetGetHitKey() const { return getHitKey_.get(); }


    std::unique_ptr<GetHitKey> getHitKey_;
    std::unique_ptr<GetPadState> getPadState_;
    std::unique_ptr<MouseController> mouseController_;
};