#include "Player.h"
#include "ResourceID.h"
#include "Window/WindowManager.h"
#include "FPSCamera.h"

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
	data_.translate.velocity = Vector3(0.0f, 0.0f, 0.0f);
	data_.translate.acceleration = Vector3(0.0f, -0.001f, 0.0f);
	data_.scale.value = Vector3(1.0f, 2.0f, 1.0f);
	data_.rotate.value = Vector3(0.0f, 0.0f, 0.0f);
}

void Player::Update()
{
	Vector3 cameraRot = Game::Camera::Getter::GetCurrentRotate();
	ImGui::Text("Camera Rot Y: %.2f", Game::Math::RadianToDegree(cameraRot.y));

	Vector3 forward = Game::Math::DirectionFromYawPitch(cameraRot.y, 0.0f);
	Vector3 right = Game::Math::DirectionFromYawPitch(cameraRot.y + 1.5708f, 0.0f);

	// 移動（フレーム独立）
	if (Game::Input::Key::IsHeld(DIK_W)) data_.translate.value += forward * PLAYER_SPEED;
	if (Game::Input::Key::IsHeld(DIK_S)) data_.translate.value -= forward * PLAYER_SPEED;
	if (Game::Input::Key::IsHeld(DIK_A)) data_.translate.value -= right * PLAYER_SPEED;
	if (Game::Input::Key::IsHeld(DIK_D)) data_.translate.value += right * PLAYER_SPEED;

	if (Game::Input::Key::IsJustPressed(DIK_SPACE))
	{
		data_.translate.velocity.y += 0.1f;
	}

	fpsCamera_->Update();
}

void Player::Draw()
{
	reticle_.Draw();
}