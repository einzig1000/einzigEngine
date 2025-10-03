#pragma once
#include "definition/definition.h"

class MouseController
{
public:
	MouseController(HWND hwnd, uint32_t width, uint32_t height, Matrix4x4* viewProjectionMatrix, Matrix4x4* debugViewProjectionMatrix, bool* debugCameraMode);
	void Update();

	void SetMousePosition();
	void SetMouseRay();
	void SetMouseButtenState();

	Vector2 GetMousePosition() const { return position_; }
	Ray GetMouseRay() const { return ray_; }

	mouseButtenState Buttens;
	mouseButtenState preButtens;

private:
	Vector2 position_;
	Ray ray_;

	HWND hwnd_;
	bool* debugCameraMode_;
	uint32_t width_;
	uint32_t height_;
	Matrix4x4* viewProjectionMatrix_;
	Matrix4x4* debugViewProjectionMatrix_;
};