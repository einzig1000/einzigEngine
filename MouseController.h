#pragma once
#include "definition.h"

class MouseController
{
public:
	void SetMousePosition(Vector2 position) { position_ = position; };
	void SetMouseRay(uint32_t width, uint32_t height, Matrix4x4 viewProjectionMatrix);

	Vector2 GetMousePosition() { return position_; }
	Ray GetMouseRay() { return ray_; }

private:
	Vector2 position_;
	Ray ray_;



};

