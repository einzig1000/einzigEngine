#include "input/GetPadState.h"
#include <cstring>

XINPUT_STATE GetPadState::state{};
XINPUT_STATE GetPadState::preState{};
bool GetPadState::isConnect;
bool GetPadState::buttons[PAD_BUTTON_MAX]{};
bool GetPadState::preButtons[PAD_BUTTON_MAX]{};
DWORD GetPadState::dwUserIndex = 0;
BYTE GetPadState::leftTrigger = 0;
BYTE GetPadState::rightTrigger = 0;
SHORT GetPadState::leftStickX = 0;
SHORT GetPadState::leftStickY = 0;
SHORT GetPadState::rightStickX = 0;
SHORT GetPadState::rightStickY = 0;
Vector2 GetPadState::leftStickDir = { 0.0f,0.0f };
Vector2 GetPadState::rightStickDir = { 0.0f,0.0f };

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
        buttons[PAD_LS] = b & XINPUT_GAMEPAD_LEFT_THUMB;
        buttons[PAD_RS] = b & XINPUT_GAMEPAD_RIGHT_THUMB;
        buttons[PAD_BACK] = b & XINPUT_GAMEPAD_BACK;
        buttons[PAD_START] = b & XINPUT_GAMEPAD_START;
        buttons[PAD_UP] = b & XINPUT_GAMEPAD_DPAD_UP;
        buttons[PAD_DOWN] = b & XINPUT_GAMEPAD_DPAD_DOWN;
        buttons[PAD_LEFT] = b & XINPUT_GAMEPAD_DPAD_LEFT;
        buttons[PAD_RIGHT] = b & XINPUT_GAMEPAD_DPAD_RIGHT;


        // トリガーとスティックの値を更新
        leftTrigger = state.Gamepad.bLeftTrigger;
        rightTrigger = state.Gamepad.bRightTrigger;

        leftStickX = state.Gamepad.sThumbLX;
        leftStickY = state.Gamepad.sThumbLY;
        rightStickX = state.Gamepad.sThumbRX;
        rightStickY = state.Gamepad.sThumbRY;

        Vector2 leftStick = { float(leftStickX) , float(leftStickY) };
        leftStickDir = leftStick.Normalized();
        Vector2 rightStick = { float(rightStickX) , float(rightStickY) };
        rightStickDir = rightStick.Normalized();

        isConnect = true;
    }
    else
    {
        isConnect = false;
    }
}