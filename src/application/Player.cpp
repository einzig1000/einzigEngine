#include "Player.h"
#include "Window/WindowManager.h"
#include "Itemslot.h"

Player::Player()
{
	// プレイヤーデータ初期化
	data_.model = ResourceID::GetModelID(ModelID::Cube);
	data_.texture = ResourceID::GetTextureID(TextureID::UVChecker);
	data_.color.w = 0;
	data_.name = "Player";

	// レティクル初期化
	reticle_.texture = ResourceID::GetTextureID(TextureID::UVChecker);
	reticle_.transforms.scale = Vector3(0.02f, 0.02f, 0.02f);
	reticle_.transforms.translate = Vector3(
		WindowManager::winWidth_ / 2.0f,
		WindowManager::winHeight_ / 2.0f,
		0.0f
	);

	Itemslot_ = new Itemslot();
}

Player::~Player()
{
}

void Player::Initialize()
{
	data_.translate.value = Vector3(0.0f, 20.0f, 0.0f);
	data_.translate.velocity = Vector3(0.0f, -0.0f, 0.0f);
	data_.translate.acceleration = Vector3(0.0f, GRAVITY, 0.0f);
	data_.scale.value = Vector3(0.6f, 2.0f, 0.6f);
	data_.rotate.value = Vector3(0.0f, 0.0f, 0.0f);
}

void Player::Update()
{
	// 移動更新
	UpdateDush();
	UpdateMove();
	UpdateJump();

	// 移動後のめりこみ修正
	ResolveMapCollision();

	// 移動後の視線レイ更新
	UpdateViewLine();


	Itemslot_->Update();
}

void Player::Draw()
{
	data_.Draw();
	reticle_.Draw();
	Itemslot_->Draw();
}

void Player::DrawImGui()
{}

void Player::UpdateViewLine()
{
	Vector3 cameraRot = Game::Camera::Getter::GetCurrentRotate();
	Vector3 direction = Game::Math::DirectionFromYawPitch(cameraRot.y, cameraRot.x);

	viewLine_.origin = data_.aabbs[0].center();
	viewLine_.origin.y += (data_.aabbs[0].max.y - data_.aabbs[0].min.y) * 0.5f;
	viewLine_.end = viewLine_.origin + direction * 10.0f;
}

void Player::UpdateMove()
{
	Vector3 cameraRot = Game::Camera::Getter::GetCurrentRotate();
	Vector3 forward = Game::Math::DirectionFromYawPitch(cameraRot.y, 0.0f);
	forward.Normalize();

	// 移動処理
	Vector2 input(0.0f, 0.0f);

	if (Game::Input::Key::IsHeld(DIK_W)) input.y += 1.0f;
	if (Game::Input::Key::IsHeld(DIK_S)) input.y -= 1.0f;
	if (Game::Input::Key::IsHeld(DIK_A)) input.x += 1.0f;
	if (Game::Input::Key::IsHeld(DIK_D)) input.x -= 1.0f;

	if (input.x != 0.0f || input.y != 0.0f)
	{
		// 正規化
		input.Normalize();

		// 入力ベクトルの角度（ラジアン）
		float angle = std::atan2(input.x, input.y); // XZ平面での回転

		// forward を angle だけ回転
		float cosA = std::cos(angle);
		float sinA = std::sin(angle);

		Vector3 speed(
			forward.x * cosA - forward.z * sinA,
			forward.y,
			forward.x * sinA + forward.z * cosA
		);

		data_.translate.velocity.x = speed.x * speed_;
		data_.translate.velocity.z = speed.z * speed_;
	}
	else
	{
		data_.translate.velocity.x = 0.0f;
		data_.translate.velocity.z = 0.0f;
	}
}

void Player::UpdateDush()
{
	if (Game::Input::Key::IsJustReleased(DIK_W))
	{
		speed_ = normalSpeed_;
		if (wHeldFrames_ < 20)dashBufferTimer_ = 20;
	}
	if (dashBufferTimer_ > 0)
	{
		dashBufferTimer_--;
		if (Game::Input::Key::IsJustPressed(DIK_W))
		{
			speed_ = dashSpeed_;
		}
	}

	wHeldFrames_ = Game::Input::Key::HoldFrames(DIK_W);
}

void Player::UpdateJump()
{
	// ジャンプ処置
	if (Game::Input::Key::IsJustPressed(DIK_SPACE))
	{
		Jump();
	}
}

void Player::AddItemToItemslot(int itemID)
{
	Itemslot_->AddItemToItemslot(itemID);
}
