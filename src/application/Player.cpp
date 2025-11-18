#include "Player.h"
#include "ResourceID.h"
#include "Window/WindowManager.h"
#include "FPSCamera.h"

Ray Player::viewRay_;

Player::Player()
{
	// プレイヤーデータ初期化
	data_.model = ResourceID::blockModelIDs_[int(ModelID::Cube)];
	data_.texture = ResourceID::blockTextureIDs_[int(TextureID::UVChecker)];
	data_.name = "Player";

	// レティクル初期化
	reticle_.texture = ResourceID::TextureIDs_[int(TextureID::UVChecker)];
	reticle_.transforms.scale = Vector3(0.02f, 0.02f, 0.02f);
	reticle_.transforms.translate = Vector3(
		WindowManager::winWidth_ / 2.0f,
		WindowManager::winHeight_ / 2.0f,
		0.0f
	);

	fpsCamera_ = new FPSCamera(this);
}

Player::~Player()
{
	delete fpsCamera_;
	fpsCamera_ = nullptr;
}

void Player::Initialize()
{
	data_.translate.value = Vector3(0.0f, 5.0f, 0.0f);
	data_.translate.velocity = Vector3(0.0f, -0.0f, 0.0f);
	data_.translate.acceleration = Vector3(0.0f, GRAVITY, 0.0f);
	data_.scale.value = Vector3(1.0f, 2.0f, 1.0f);
	data_.rotate.value = Vector3(0.0f, 0.0f, 0.0f);
}

void Player::Update()
{
	Vector3 cameraRot = Game::Camera::Getter::GetCurrentRotate();
	Vector3 forward = Game::Math::DirectionFromYawPitch(cameraRot.y, 0.0f);
	Vector3 right = Game::Math::DirectionFromYawPitch(cameraRot.y + 1.5708f, 0.0f);
	Vector3 direction = Game::Math::DirectionFromYawPitch(cameraRot.y, cameraRot.x);

	viewRay_.origin = data_.aabbs[0].center();
	viewRay_.origin.y += (data_.aabbs[0].max.y - data_.aabbs[0].min.y) * 0.5f;
	viewRay_.diff = direction * 100.0f;

	// 移動
	if (Game::Input::Key::IsHeld(DIK_W) || Game::Input::Key::IsHeld(DIK_S) || 
		Game::Input::Key::IsHeld(DIK_A) || Game::Input::Key::IsHeld(DIK_D))
	{
		if (Game::Input::Key::IsHeld(DIK_W))
		{
			data_.translate.velocity = forward * PLAYER_SPEED;
		}
		if (Game::Input::Key::IsHeld(DIK_S))
		{
			data_.translate.velocity = -forward * PLAYER_SPEED;
		}
		if (Game::Input::Key::IsHeld(DIK_D))
		{
			data_.translate.velocity = right * PLAYER_SPEED;
		}
		if (Game::Input::Key::IsHeld(DIK_A))
		{
			data_.translate.velocity = -right * PLAYER_SPEED;
		}
	}
	else
	{
		data_.translate.velocity.x = 0.0f;
		data_.translate.velocity.z = 0.0f;
	}

	if (Game::Input::Key::IsJustPressed(DIK_SPACE))
	{
		data_.translate.velocity.y += 0.1f;
	}

	fpsCamera_->Update();
}

void Player::Draw()
{
	reticle_.Draw();
	//data_.Draw();
	data_.DrawImGui();
}