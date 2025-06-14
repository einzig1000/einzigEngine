#include "GetHitKey.h"
#include <cassert>

BYTE GetHitKey::keys[256];
BYTE GetHitKey::preKeys[256];

GetHitKey::GetHitKey(HWND hwnd)
{
	IDirectInput8* directInput{};
	HINSTANCE hInstance = GetModuleHandle(nullptr); // Define hInstance to fix the undefined "w" issue  
	HRESULT result = DirectInput8Create(
		hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8,
		(void**)&directInput, nullptr
	);

	//HRESULT result = DirectInput8Create(
	//	w.hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8,
	//	(void**)&directInput, nullptr
	//);
	assert(SUCCEEDED(result));

	result = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
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