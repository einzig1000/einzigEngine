#include <Windows.h>
#include <Xinput.h>
#pragma comment(lib, "xinput.lib") // XInputライブラリをリンク

// ゲームパッドの入力状態を管理するクラス
class GetPadState
{
public:
    // 取得対象のボタン種類を定義
    enum Button
    {
        BUTTON_A,
        BUTTON_B,
        BUTTON_X,
        BUTTON_Y,
        BUTTON_LB,
        BUTTON_RB,
        BUTTON_BACK,
        BUTTON_START,
        BUTTON_UP,
        BUTTON_DOWN,
        BUTTON_LEFT,
        BUTTON_RIGHT
    };

    // プレイヤー番号を指定（0〜3）。デフォルトは0番プレイヤー
    GetPadState(int playerIndex = 0)
        : playerIndex(playerIndex)
    {
        // ゲームパッドの状態構造体をゼロ初期化
        ZeroMemory(&currentState, sizeof(XINPUT_STATE));
        ZeroMemory(&previousState, sizeof(XINPUT_STATE));
    }

    // 毎フレーム呼び出して入力状態を更新
    void Update()
    {
        previousState = currentState;
        XInputGetState(playerIndex, &currentState);
    }

    // 指定ボタンが「押された瞬間か」を判定
    bool IsPressed(Button button) const
    {
        return (GetButton(currentState, button) &&
            !GetButton(previousState, button));
    }

    // 指定ボタンが「押し続けられているか」を判定
    bool IsHeld(Button button) const
    {
        return GetButton(currentState, button);
    }

    // 指定ボタンが「離された瞬間か」を判定
    bool IsReleased(Button button) const
    {
        return (!GetButton(currentState, button) &&
            GetButton(previousState, button));
    }

    // 左トリガーの圧力を取得（0.0〜1.0）
    float GetLeftTrigger() const
    {
        return currentState.Gamepad.bLeftTrigger / 255.0f;
    }

    // 右トリガーの圧力を取得（0.0〜1.0）
    float GetRightTrigger() const
    {
        return currentState.Gamepad.bRightTrigger / 255.0f;
    }

    // 左スティックX方向の入力値（±1.0）を取得
    float GetLeftStickX() const
    {
        return NormalizeStick(currentState.Gamepad.sThumbLX);
    }

    // 左スティックY方向の入力値（±1.0）を取得
    float GetLeftStickY() const
    {
        return NormalizeStick(currentState.Gamepad.sThumbLY);
    }

private:
    int playerIndex;                   // プレイヤー番号（XInputのコントローラーID）
    XINPUT_STATE currentState;         // 現在の状態
    XINPUT_STATE previousState;        // 前フレームの状態

    // 指定したボタンの状態を取得（押されていればtrue）
    bool GetButton(const XINPUT_STATE& state, Button button) const
    {
        const WORD b = state.Gamepad.wButtons;
        switch (button)
        {
        case BUTTON_A:      return b & XINPUT_GAMEPAD_A;
        case BUTTON_B:      return b & XINPUT_GAMEPAD_B;
        case BUTTON_X:      return b & XINPUT_GAMEPAD_X;
        case BUTTON_Y:      return b & XINPUT_GAMEPAD_Y;
        case BUTTON_LB:     return b & XINPUT_GAMEPAD_LEFT_SHOULDER;
        case BUTTON_RB:     return b & XINPUT_GAMEPAD_RIGHT_SHOULDER;
        case BUTTON_BACK:   return b & XINPUT_GAMEPAD_BACK;
        case BUTTON_START:  return b & XINPUT_GAMEPAD_START;
        case BUTTON_UP:     return b & XINPUT_GAMEPAD_DPAD_UP;
        case BUTTON_DOWN:   return b & XINPUT_GAMEPAD_DPAD_DOWN;
        case BUTTON_LEFT:   return b & XINPUT_GAMEPAD_DPAD_LEFT;
        case BUTTON_RIGHT:  return b & XINPUT_GAMEPAD_DPAD_RIGHT;
        default:            return false;
        }
    }

    // スティック入力の正規化（デッドゾーン補正を含む）
    float NormalizeStick(SHORT value) const
    {
        const SHORT deadZone = XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
        if (abs(value) < deadZone) return 0.0f;
        return value / 32767.0f;
    }
};