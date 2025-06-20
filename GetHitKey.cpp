#include "GetHitKey.h"
#include <cassert>

BYTE GetHitKey::keys[256];
BYTE GetHitKey::preKeys[256];

GetHitKey::GetHitKey(HWND hwnd)
{
	Microsoft::WRL::ComPtr<IDirectInput8> directInput;
	HINSTANCE hInstance = GetModuleHandle(nullptr);
	HRESULT result = DirectInput8Create(
		hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8,
		reinterpret_cast<void**>(directInput.GetAddressOf()),
		nullptr
	);
	assert(SUCCEEDED(result));

	result = directInput->CreateDevice(GUID_SysKeyboard, keyboard.GetAddressOf(), NULL);
	assert(SUCCEEDED(result));

	result = keyboard->SetDataFormat(&c_dfDIKeyboard);
	assert(SUCCEEDED(result));

	result = keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));
}

void GetHitKey::Update()
{
	memcpy(preKeys, keys, 256);
	keyboard->Acquire();
	keyboard->GetDeviceState(sizeof(keys), keys);
}