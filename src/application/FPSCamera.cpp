#include "FPSCamera.h"
#include "Player.h"

FPSCamera::FPSCamera(Player* player)
{
	player_ = player;
}

void FPSCamera::Update()
{
	preMousePos = mousePos;
	mousePos = Game::Input::Mouse::GetMousePosition();
	mouseGap = mousePos - preMousePos;

	if (!Game::Input::Key::IsHeld(DIK_SPACE))
	{
		cameraRot.x += mouseGap.y * mouseSensitivity_;
		cameraRot.y += mouseGap.x * mouseSensitivity_;
		cameraRot.x = std::clamp<float>(cameraRot.x, -1.5f, 1.5f);
	}

	cameraPos = player_->viewRay_.origin;//->data_.aabbs[0].center();

	Game::Camera::MoveCameraCenter(cameraPos, 0, EaseType::LINEAR);
	Game::Camera::MoveCameraRotate(cameraRot, 3, EaseType::LINEAR);
}