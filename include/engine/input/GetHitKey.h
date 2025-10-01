#pragma once
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <wrl.h>

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

class GetHitKey
{
public:
	GetHitKey(HWND hwnd);
	void Update();

	static BYTE keys[256];
	static BYTE preKeys[256];

private:

	Microsoft::WRL::ComPtr<IDirectInputDevice8> keyboard;
};