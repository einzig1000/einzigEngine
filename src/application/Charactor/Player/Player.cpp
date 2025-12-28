#include "Charactor/Player/Player.h"
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

	breakPower_ = 10;

	Itemslot_ = new Itemslot();
}

Player::~Player()
{}

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
	if (Game::Input::Key::IsJustPressed(DIK_X))
	{
		data_.translate.value.y = 100.0f;
		data_.translate.velocity.y = 0.0f;
		data_.translate.acceleration.y = 0.0f;
	}


	// 移動更新
	UpdateDush();
	UpdateMove();
	UpdateJump();

	// 移動後のめりこみ修正
	ResolveMapCollision();

	// 移動後の視線レイ更新
	UpdateViewRay();

	// ターゲットブロック取得
	SetTargetBlock();

	// ブロック破壊
	if (Game::Input::Mouse::IsHeld(0))
	{
		BreakTargetBlock();
	}

	// ブロック設置
	if (Game::Input::Mouse::IsJustPressed(1))
	{
		SetNewBlock(BlockID::Dirt);
	}

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

void Player::UpdateViewRay()
{
	Vector3 cameraRot = Game::Camera::Getter::GetCurrentRotate();
	Vector3 direction = Game::Math::DirectionFromYawPitch(cameraRot.y, cameraRot.x);

	viewRay_.origin = data_.aabbs[0].center();
	viewRay_.origin.y += data_.scale.value.y * 0.4f; // プレイヤーの目線の高さに調整
	viewRay_.diff = direction.Normalized();

	SetViewRay(viewRay_);
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

	// 移動方向ベクトル
	Vector3 moveDir;

	if (input.x != 0.0f || input.y != 0.0f)
	{
		// 正規化
		input.Normalize();

		// 入力ベクトルの角度（ラジアン）
		float angle = std::atan2(input.x, input.y); // XZ平面での回転

		// forward を angle だけ回転
		float cosA = std::cos(angle);
		float sinA = std::sin(angle);

		// 進む方向ベクトル
		moveDir = Vector3(
			forward.x * cosA - forward.z * sinA,
			forward.y,
			forward.x * sinA + forward.z * cosA
		);

		moveDir.Normalize();

		Move(moveDir, speed_);
	}
	else
	{
		moveDir = Vector3(0.0f, 0.0f, 0.0f);

		Move(moveDir, speed_);
	}
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
