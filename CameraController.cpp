#include "CameraController.h"
#include "Player.h"

CameraController::CameraController(Player* player)
{
	// プレイヤー情報を保存
	player_ = player;

	// カメラ操作可能に設定
	enableControl = true;

	// 初期カメラモードは三人称後方視点
	cameraMode_ = CameraMode_FirstPerson_ThirdPerson::FirstPerson;
}

void CameraController::Update()
{
	mousedelta = Game::Input::Mouse::GetPositionDelta();

	if (Game::Input::Key::IsJustPressed(DIK_T))
	{
		enableControl = !enableControl;
	}

	// カメラモード切り替え
	if (Game::Input::Key::IsJustPressed(DIK_F5))
	{
		switch (cameraMode_)
		{
		case CameraMode_FirstPerson_ThirdPerson::FirstPerson:
			cameraMode_ = CameraMode_FirstPerson_ThirdPerson::ThirdPerson_Back;
			break;
		case CameraMode_FirstPerson_ThirdPerson::ThirdPerson_Back:
			cameraMode_ = CameraMode_FirstPerson_ThirdPerson::ThirdPerson_Front;
			break;
		case CameraMode_FirstPerson_ThirdPerson::ThirdPerson_Front:
			cameraMode_ = CameraMode_FirstPerson_ThirdPerson::FirstPerson;
			break;
		default:
			break;
		}
	}

	if (enableControl)
	{
		// マウス移動量に応じてカメラ回転
		cameraRot.x += mousedelta.y * mouseSensitivity_;
		cameraRot.y += mousedelta.x * mouseSensitivity_;
		cameraRot.x = std::clamp<float>(cameraRot.x, -1.5f, 1.5f);

		switch (cameraMode_)
		{
		case CameraMode_FirstPerson_ThirdPerson::FirstPerson:
		{
			// カメラ位置はプレイヤーの目の位置
			cameraPos = player_->viewRay_.origin;
			break;
		}
		case CameraMode_FirstPerson_ThirdPerson::ThirdPerson_Back:
		{
			// カメラ位置はプレイヤーの後方
			// プレイヤーの向きから後方ベクトルを計算
			Vector3 backOffset;
			backOffset.x = -std::sin(cameraRot.y) * 4.0f;
			backOffset.y = 2.0f + std::sin(cameraRot.x) * 2.0f;
			backOffset.z = -std::cos(cameraRot.y) * 4.0f;
			cameraPos = player_->viewRay_.origin + backOffset;
			break;
		}
		case CameraMode_FirstPerson_ThirdPerson::ThirdPerson_Front:
		{
			// カメラ位置はプレイヤーの前方
			// プレイヤーの向きから前方ベクトルを計算
			Vector3 frontOffset;
			frontOffset.x = std::sin(cameraRot.y) * 4.0f;
			frontOffset.y = 2.0f + std::sin(cameraRot.x) * 2.0f;
			frontOffset.z = std::cos(cameraRot.y) * 4.0f;
			cameraPos = player_->viewRay_.origin + frontOffset;
			break;
		}
		default:
			break;
		}
	}

	/// カメラがマップにめり込まないようにする処理
	//{
	//	// カメラの位置がブロック内にあるかチェック
	//	Vector3int cameraBlockPos = {
	//		static_cast<int>(std::floor(cameraPos.x)),
	//		static_cast<int>(std::floor(cameraPos.y)),
	//		static_cast<int>(std::floor(cameraPos.z))
	//	};
	//	if (player_->mapManager_)
	//	{
	//		if (player_->mapManager_->IsBlockAt(cameraBlockPos))
	//		{
	//			// ブロック内にある場合、カメラをプレイヤーの目の位置に戻す
	//			cameraPos = player_->viewRay_.origin;
	//		}
	//	}
	//}

	Game::Camera::MoveCameraCenter(cameraPos, 0, EaseType::LINEAR);
	Game::Camera::MoveCameraRotate(cameraRot, 3, EaseType::LINEAR);
}