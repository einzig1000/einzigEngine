#include "FPSCamera.h"
#include "Player.h"

FPSCamera::FPSCamera(Player* player)
{
	player_ = player;
	enableControl = true;
}

void FPSCamera::Update()
{
	preMousePos = mousePos;
	mousePos = Game::Input::Mouse::GetMousePosition();
	mouseGap = mousePos - preMousePos;

	if (Game::Input::Key::IsJustPressed(DIK_T))
	{
		enableControl = !enableControl;
	}

	if (enableControl)
	{
		cameraRot.x += mouseGap.y * mouseSensitivity_;
		cameraRot.y += mouseGap.x * mouseSensitivity_;
		cameraRot.x = std::clamp<float>(cameraRot.x, -1.5f, 1.5f);
	}

	cameraPos = player_->viewRay_.origin;

	Game::Camera::MoveCameraCenter(cameraPos, 0, EaseType::LINEAR);
	Game::Camera::MoveCameraRotate(cameraRot, 3, EaseType::LINEAR);
}