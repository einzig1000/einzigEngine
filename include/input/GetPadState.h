#include <Windows.h>
#include <Xinput.h>
#pragma comment(lib, "xinput.lib") // XInputライブラリをリンク

enum PadButton
{
    PAD_A, PAD_B, PAD_X, PAD_Y,
    PAD_LB, PAD_RB,
    PAD_BACK, PAD_START,
    PAD_UP, PAD_DOWN, PAD_LEFT, PAD_RIGHT,
    PAD_BUTTON_COUNT
};

class GetPadState
{
public:
    static void Update();

    // 使用箇所で GetPadState::buttons[PAD_A] のようにアクセス可能
    static bool buttons[PAD_BUTTON_COUNT];
    static bool preButtons[PAD_BUTTON_COUNT];

private:
    static XINPUT_STATE state;
    static XINPUT_STATE preState;
    static DWORD dwUserIndex;
};