#pragma once
#include <Windows.h>
#include "definition/definition.h"
#include <Xinput.h>
#pragma comment(lib, "xinput.lib") // XInputライブラリをリンク

#define PAD_A               0x01
#define PAD_B               0x02
#define PAD_X               0x03
#define PAD_Y               0x04
#define PAD_LB              0x05
#define PAD_RB              0x06
#define PAD_LS              0x07
#define PAD_RS              0x08
#define PAD_BACK            0x09
#define PAD_START           0x0A
#define PAD_UP              0x0B
#define PAD_DOWN            0x0C
#define PAD_LEFT            0x0D
#define PAD_RIGHT           0x0E
#define PAD_BUTTON_MAX      0x0F

class GetPadState
{
public:
    static void Update();

    // 使用箇所で GetPadState::buttons[PAD_A] のようにアクセス可能
    static bool buttons[4][PAD_BUTTON_MAX];
    static bool preButtons[4][PAD_BUTTON_MAX];
    static bool isConnect[4];

    static BYTE leftTrigger[4];     // 左トリガー（0〜255）
    static BYTE rightTrigger[4];    // 右トリガー（0〜255）

    static SHORT leftStickX[4];        // 左スティックX軸（-32768〜32767）
    static SHORT leftStickY[4];        // 左スティックY軸（-32768〜32767）
    static SHORT rightStickX[4];        // 右スティックX軸（-32768〜32767）
    static SHORT rightStickY[4];        // 右スティックY軸（-32768〜32767）
    static Vector2 leftStickDir[4];        // 左スティック方向({-1〜1},{-1〜1})
    static Vector2 rightStickDir[4];        // 左スティック方向({-1〜1},{-1〜1})

private:
    static XINPUT_STATE state[4];
    static XINPUT_STATE preState[4];
    static DWORD dwUserIndex[4];
};