#pragma once
#include "definition/definition.h"

class MouseController
{
public:
	MouseController(HWND hwnd, Matrix4x4* viewProjectionMatrix, Matrix4x4* debugViewProjectionMatrix, bool* debugCameraMode);
	void Update();
	void EndFrame();

	void SetMousePosition();
	void SetMouseRay();
	void SetMouseButtenState();

	Vector2 GetMousePosition() const { return position_; }
	bool GetMousePress(int i);
	bool GetMousePrePress(int i);
	Ray GetMouseRay() const { return ray_; }

	mouseButtenState Buttens;
	mouseButtenState preButtens;
	int wheelDelta = 0;

private:
	Vector2 position_;
	Ray ray_;

	HWND hwnd_;
	bool* debugCameraMode_;
	Matrix4x4* viewProjectionMatrix_;
	Matrix4x4* debugViewProjectionMatrix_;
};