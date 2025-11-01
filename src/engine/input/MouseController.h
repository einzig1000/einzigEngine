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
	bool GetMousePress(int i) const;
	bool GetMousePrePress(int i) const;
	Ray GetMouseRay() const { return ray_; }

	mouseButtenState Buttens;
	mouseButtenState preButtens;
	int wheelDelta = 0;

private:
	CameraManager* cameraManager_;

	Vector2 position_;
	Ray ray_;

	HWND hwnd_;
	//bool* debugCameraMode_;
	//Matrix4x4* viewProjectionMatrix_;
	//Matrix4x4* debugViewProjectionMatrix_;
};