#pragma once
#include "definition.h"

class MouseController
{
public:
	void SetMousePosition(Vector2 position) { position_ = position; };
	void SetMouseRay(const uint32_t width, const  uint32_t height, const Matrix4x4 viewProjectionMatrix);

	Vector2 GetMousePosition() const { return position_; }
	Ray GetMouseRay() const { return ray_; }

private:
	Vector2 position_;
	Ray ray_;
};