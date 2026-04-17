#pragma once
#include <Windows.h>
#include "definition/definition.h"
#include <Xinput.h>
#pragma comment(lib, "xinput.lib") // XInputライブラリをリンク

#define PAD_A               0x00
#define PAD_B               0x01
#define PAD_X               0x02
#define PAD_Y               0x03
#define PAD_LB              0x04
#define PAD_RB              0x05
#define PAD_LS              0x06
#define PAD_RS              0x07
#define PAD_BACK            0x08
#define PAD_START           0x09
#define PAD_UP              0x0A
#define PAD_DOWN            0x0B
#define PAD_LEFT            0x0C
#define PAD_RIGHT           0x0D
#define PAD_BUTTON_MAX      0x0E

/// <summary>
/// パッド管理クラス
/// </summary>
class GetPadState
{
public:
    GetPadState();
	~GetPadState();

    void Update();

	bool IsHeld(int padIndex, BYTE button);
	bool IsJustPressed(int padIndex, BYTE button);
	bool IsJustReleased(int padIndex, BYTE button);

    uint32_t HoldFrames(int padIndex, BYTE button);	// 押されてからの経過フレーム数
	Vector2 GetLeftStick(int padIndex);	    // 左スティックの値取得 (-1.0f ～ 1.0f)
	Vector2 GetRightStick(int padIndex);	// 右スティックの値取得 (-1.0f ～ 1.0f)

    // トリガーの値取得（0.0f ～ 1.0f）
    float GetLeftTrigger(int padIndex);
    float GetRightTrigger(int padIndex);
    // ゲームパッド振動
    void SetVibration(int padIndex, float leftMotor, float rightMotor);

    int32_t GetConnectedPadNum() const;

private:
    PadButtonState padStates[4][PAD_BUTTON_MAX]{};

    bool isConnect[4];

    BYTE leftTrigger[4];     // 左トリガー（0〜255）
    BYTE rightTrigger[4];    // 右トリガー（0〜255）

    SHORT leftStickX[4];        // 左スティックX軸（-32768〜32767）
    SHORT leftStickY[4];        // 左スティックY軸（-32768〜32767）
    SHORT rightStickX[4];        // 右スティックX軸（-32768〜32767）
    SHORT rightStickY[4];        // 右スティックY軸（-32768〜32767）
    Vector2 leftStickDir[4];        // 左スティック方向({-1〜1},{-1〜1})
    Vector2 rightStickDir[4];        // 右スティック方向({-1〜1},{-1〜1})



    XINPUT_STATE state[4];
    XINPUT_STATE preState[4];
    DWORD dwUserIndex[4];
};