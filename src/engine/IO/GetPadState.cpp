#include "IO/GetPadState.h"
#include <cstring>

XINPUT_STATE GetPadState::state[4]{};
XINPUT_STATE GetPadState::preState[4]{};
bool GetPadState::isConnect[4];
bool GetPadState::buttons[4][PAD_BUTTON_MAX]{};
bool GetPadState::preButtons[4][PAD_BUTTON_MAX]{};
DWORD GetPadState::dwUserIndex[4] = { 0,0,0,0 };
BYTE GetPadState::leftTrigger[4] = { 0,0,0,0 };
BYTE GetPadState::rightTrigger[4] = { 0,0,0,0 };
SHORT GetPadState::leftStickX[4] = { 0,0,0,0 };
SHORT GetPadState::leftStickY[4] = { 0,0,0,0 };
SHORT GetPadState::rightStickX[4] = { 0,0,0,0 };
SHORT GetPadState::rightStickY[4] = { 0,0,0,0 };
Vector2 GetPadState::leftStickDir[4] = { { 0.0f,0.0f } ,{ 0.0f,0.0f } ,{ 0.0f,0.0f } ,{ 0.0f,0.0f } };
Vector2 GetPadState::rightStickDir[4] = { { 0.0f,0.0f } ,{ 0.0f,0.0f } ,{ 0.0f,0.0f } ,{ 0.0f,0.0f } };

void GetPadState::Update()
{
    for (DWORD i = 0; i < 4; ++i)
    {
        preState[i] = state[i];
        std::memcpy(preButtons[i], buttons[i], sizeof(buttons[i]));

        ZeroMemory(&state[i], sizeof(XINPUT_STATE));
        if (XInputGetState(i, &state[i]) == ERROR_SUCCESS)
        {
            WORD b = state[i].Gamepad.wButtons;
            buttons[i][PAD_A] = b & XINPUT_GAMEPAD_A;
            buttons[i][PAD_A] = b & XINPUT_GAMEPAD_A;
            buttons[i][PAD_B] = b & XINPUT_GAMEPAD_B;
            buttons[i][PAD_X] = b & XINPUT_GAMEPAD_X;
            buttons[i][PAD_Y] = b & XINPUT_GAMEPAD_Y;
            buttons[i][PAD_LB] = b & XINPUT_GAMEPAD_LEFT_SHOULDER;
            buttons[i][PAD_RB] = b & XINPUT_GAMEPAD_RIGHT_SHOULDER;
            buttons[i][PAD_LS] = b & XINPUT_GAMEPAD_LEFT_THUMB;
            buttons[i][PAD_RS] = b & XINPUT_GAMEPAD_RIGHT_THUMB;
            buttons[i][PAD_BACK] = b & XINPUT_GAMEPAD_BACK;
            buttons[i][PAD_START] = b & XINPUT_GAMEPAD_START;
            buttons[i][PAD_UP] = b & XINPUT_GAMEPAD_DPAD_UP;
            buttons[i][PAD_DOWN] = b & XINPUT_GAMEPAD_DPAD_DOWN;
            buttons[i][PAD_LEFT] = b & XINPUT_GAMEPAD_DPAD_LEFT;
            buttons[i][PAD_RIGHT] = b & XINPUT_GAMEPAD_DPAD_RIGHT;

            leftTrigger[i] = state[i].Gamepad.bLeftTrigger;
            rightTrigger[i] = state[i].Gamepad.bRightTrigger;
            
            leftStickX[i] = state[i].Gamepad.sThumbLX;
            leftStickY[i] = state[i].Gamepad.sThumbLY;
            rightStickX[i] = state[i].Gamepad.sThumbRX;
            rightStickY[i] = state[i].Gamepad.sThumbRY;
            
            Vector2 leftStick = { float(leftStickX[i]), float(leftStickY[i]) };
            leftStickDir[i] = leftStick.Normalized();
            Vector2 rightStick = { float(rightStickX[i]), float(rightStickY[i]) };
            rightStickDir[i] = rightStick.Normalized();
            isConnect[i] = true;
        }
        else
        {
            isConnect[i] = false;
        }
    }
}