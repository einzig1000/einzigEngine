#pragma once
#include "input/GetHitKey.h"
#include "input/GetPadState.h"
#include "input/MouseController.h"

class InputManager
{
public:
	InputManager(HWND hwnd);

	void Update();

private:

	GetPadState* getPadState_;
	GetHitKey* getHitKey_;
	MouseController* mouseController_;



};

