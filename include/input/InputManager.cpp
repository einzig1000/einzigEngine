#include "InputManager.h"

InputManager::InputManager(HWND hwnd)
{
	getPadState_ = new GetPadState();
	getHitKey_ = new GetHitKey(hwnd);
	mouseController_ = new MouseController();
}


void InputManager::Update()
{
	getPadState_->Update();
	getHitKey_->Update();
	mouseController_->Update();
}

