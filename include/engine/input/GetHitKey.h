#pragma once
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <wrl.h>
#include <vector>
#include "definition/definition.h"

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

class GetHitKey
{
public:
	GetHitKey(HWND hwnd);
	void Update();
    
	// 今押しているか
	static bool IsPressedNow(BYTE key);
	// 押した瞬間（今フレームで押された）
	static bool IsPressedDown(BYTE key);
	// 離した瞬間（今フレームで離れた）
	static bool IsReleased(BYTE key);

	// 押されてからの経過フレーム数
	static uint32_t HoldFrames(BYTE key);

	// 0: なし  1:単押し  2:長押し(n = 長押し判定)
	static int TestTapLong(int n, BYTE key);

private:

	static KeyState keys_[256];


	Microsoft::WRL::ComPtr<IDirectInputDevice8> keyboard_;
	BYTE diKeys_[256]; // DirectInput から読んだ生データ

};