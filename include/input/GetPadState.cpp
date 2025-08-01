#include "input/GetPadState.h"
#include <cstring>

XINPUT_STATE GetPadState::state{};
XINPUT_STATE GetPadState::preState{};
bool GetPadState::buttons[PAD_BUTTON_COUNT]{};
bool GetPadState::preButtons[PAD_BUTTON_COUNT]{};
DWORD GetPadState::dwUserIndex = 0;

void GetPadState::Update()
{
    preState = state;
    std::memcpy(preButtons, buttons, sizeof(buttons));

    ZeroMemory(&state, sizeof(XINPUT_STATE));
    if (XInputGetState(dwUserIndex, &state) == ERROR_SUCCESS)
    {
        WORD b = state.Gamepad.wButtons;
        buttons[PAD_A] = b & XINPUT_GAMEPAD_A;
        buttons[PAD_B] = b & XINPUT_GAMEPAD_B;
        buttons[PAD_X] = b & XINPUT_GAMEPAD_X;
        buttons[PAD_Y] = b & XINPUT_GAMEPAD_Y;
        buttons[PAD_LB] = b & XINPUT_GAMEPAD_LEFT_SHOULDER;
        buttons[PAD_RB] = b & XINPUT_GAMEPAD_RIGHT_SHOULDER;
        buttons[PAD_BACK] = b & XINPUT_GAMEPAD_BACK;
        buttons[PAD_START] = b & XINPUT_GAMEPAD_START;
        buttons[PAD_UP] = b & XINPUT_GAMEPAD_DPAD_UP;
        buttons[PAD_DOWN] = b & XINPUT_GAMEPAD_DPAD_DOWN;
        buttons[PAD_LEFT] = b & XINPUT_GAMEPAD_DPAD_LEFT;
        buttons[PAD_RIGHT] = b & XINPUT_GAMEPAD_DPAD_RIGHT;
    }
}