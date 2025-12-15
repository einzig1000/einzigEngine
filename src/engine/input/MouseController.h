#pragma once
#include "definition/definition.h"

class CameraManager;

class MouseController
{
public:
	MouseController(HWND hwnd, CameraManager* cameraManager);
	void Update();
	void EndFrame();

	void SetMousePosition();
	void SetMouseRay();
	void SetMouseButtenState();

	Vector2 GetMousePosition() const { return position_; }
	Vector3 GetMouseWorldPosition() const { return ray_.origin; }
	Ray GetMouseRay() const { return ray_; }

	bool IsHeld(int i);// 今押しているか
	bool IsJustPressed(int i);// 押した瞬間（今フレームで押された）
	bool IsJustReleased(int i);// 離した瞬間（今フレームで離れた）
	uint32_t HoldFrames(int i);// 押されてからの経過フレーム数

	void ToggleMouseCursorVisible();
	void ShowCursor(bool visible);

	int wheelDelta_ = 0;

private:

	bool isVisible_;

	mouseButtenState leftButton_;
	mouseButtenState rightButton_;
	mouseButtenState middleButton_;

	Vector2 position_;
	Ray ray_;

	HWND hwnd_;


	CameraManager* cameraManager_;
};