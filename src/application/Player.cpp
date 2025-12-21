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
	data_.translate.value = Vector3(0.0f, 5.0f, 0.0f);
	data_.translate.velocity = Vector3(0.0f, -0.0f, 0.0f);
	data_.translate.acceleration = Vector3(0.0f, GRAVITY, 0.0f);
	data_.scale.value = Vector3(0.6f, 2.0f, 0.6f);
	data_.rotate.value = Vector3(0.0f, 0.0f, 0.0f);
}

void Player::Update()
{
	Vector3 cameraRot = Game::Camera::Getter::GetCurrentRotate();
	Vector3 forward = Game::Math::DirectionFromYawPitch(cameraRot.y, 0.0f);
	Vector3 direction = Game::Math::DirectionFromYawPitch(cameraRot.y, cameraRot.x);
	forward.Normalize();

	viewLine_.origin = data_.aabbs[0].center();
	viewLine_.origin.y += (data_.aabbs[0].max.y - data_.aabbs[0].min.y) * 0.5f;
	viewLine_.end = viewLine_.origin + direction * 10.0f;

	// 移動処理
	if (Game::Input::Key::IsHeld(DIK_W) || Game::Input::Key::IsHeld(DIK_S) || 
		Game::Input::Key::IsHeld(DIK_A) || Game::Input::Key::IsHeld(DIK_D))
	{
		DirectionXZ8Way dir = DirectionXZ8Way::None;

		if (Game::Input::Key::IsHeld(DIK_W))
		{
			dir = DirectionXZ8Way::Front;
		}
		if (Game::Input::Key::IsHeld(DIK_S))
		{
			if (dir == DirectionXZ8Way::Front)
			{
				dir = DirectionXZ8Way::None;
			}
			else
			{
				dir = DirectionXZ8Way::Back;
			}
		}
		if (Game::Input::Key::IsHeld(DIK_D))
		{
			if (dir == DirectionXZ8Way::Front)
			{
				dir = DirectionXZ8Way::FrontRight;
			}
			else if (dir == DirectionXZ8Way::Back)
			{
				dir = DirectionXZ8Way::BackRight;
			}
			else
			{
				dir = DirectionXZ8Way::Right;
			}
		}
		if (Game::Input::Key::IsHeld(DIK_A))
		{
			if (dir == DirectionXZ8Way::Front)
			{
				dir = DirectionXZ8Way::FrontLeft;
			}
			else if (dir == DirectionXZ8Way::Back)
			{
				dir = DirectionXZ8Way::BackLeft;
			}
			else if (dir == DirectionXZ8Way::Right)
			{
				dir = DirectionXZ8Way::None;
			}
			else if (dir == DirectionXZ8Way::FrontRight)
			{
				dir = DirectionXZ8Way::Front;
			}
			else if (dir == DirectionXZ8Way::BackRight)
			{
				dir = DirectionXZ8Way::Back;
			}
			else
			{
				dir = DirectionXZ8Way::Left;
			}
		}

		if (dir != DirectionXZ8Way::None)
		{
			float angle = int(dir) * 45.0f;
			Vector3 temp;

			float radians = Game::Math::DegreeToRadian(angle);
			float cosA = std::cos(radians);
			float sinA = std::sin(radians);

			temp = Vector3(
				forward.x * cosA - forward.z * sinA,
				forward.y,
				forward.x * sinA + forward.z * cosA
			);

			data_.translate.velocity.x = temp.x * speed_;
			data_.translate.velocity.z = temp.z * speed_;
		}
	}
	else
	{
		data_.translate.velocity.x = 0.0f;
		data_.translate.velocity.z = 0.0f;
	}

	// ダッシュ処理
	if (Game::Input::Key::IsHeld(DIK_W))
	{
		wHeldFrames_++;
	}
	else
	{
		wHeldFrames_ = 0;
	}
	if (Game::Input::Key::IsJustReleased(DIK_W) && wHeldFrames_ < 20)
	{
		preDash_ = 20;
	}
	if (preDash_ > 0)
	{
		if (Game::Input::Key::IsJustPressed(DIK_W))
		{
			speed_ = dashSpeed_;
		}
		preDash_--;
	}
	if (Game::Input::Key::IsJustReleased(DIK_W) && speed_ == dashSpeed_)
	{
		speed_ = normalSpeed_;
	}

	// ジャンプ処置
	if (Game::Input::Key::IsJustPressed(DIK_SPACE))
	{
		data_.translate.velocity.y = 0.1f;
		data_.translate.value.y += data_.translate.velocity.y;
	}

	Itemslot_->Update();
}

void Player::Draw()
{
	data_.Draw();
	reticle_.Draw();
	Itemslot_->Draw();
}

void Player::AddItemToItemslot(int itemID)
{
	Itemslot_->AddItemToItemslot(itemID);
}
