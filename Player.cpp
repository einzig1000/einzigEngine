#include "Player.h"

Player::Player(int Tex, int model)
{
	data.texture = Tex;
	data.model = model;
	frame = 0;

	bullet_ = new PlayerBullet();

	state = actionState::move;
}

Player::~Player()
{
	delete bullet_;
	bullet_ = nullptr;
}

void Player::Update()
{
	int pattern = GetHitKey::TestTapLong(20, DIK_SPACE);

	switch (state)
	{
	case actionState::none:
		break;
	case actionState::move:
	{
		// 移動
		data.transforms.translate.z += 0.1f;

		// ジャンプ
		if (pattern == 1)
		{
			data.velocity.y = 0.3f;
			data.gravity.y = 0.01f;
		}

		//if ()

		if (pattern == 2)
		{
			落下高度 = data.transforms.translate.y;
			state = actionState::attack;
		}
		break;
	}
	case actionState::attack:
	{
		data.velocity.y = -1.0f;
		if (data.transforms.translate.y <= 0.0f)
		{
			data.velocity.y = 攻撃終後の速度;
			攻撃終後の速度 = 0.0f;
			攻撃中 = true;
			state = actionState::move;
		}

		break;
	}
	default:
		break;
	} 

	if (bullet_->Update(data.GetWorldPosition(), 攻撃中, 落下高度))
	{
		攻撃中 = false;
	}

	//ImGui::Begin("Player");
	//ImGui::Text("pattern: %d", pattern);
	//ImGui::Text("IsPressedNow: %d", GetHitKey::IsPressedNow(DIK_SPACE));
	//ImGui::Text("IsPressedDown: %d", GetHitKey::IsPressedDown(DIK_SPACE));
	//ImGui::Text("IsReleased: %d", GetHitKey::IsReleased(DIK_SPACE));
	//ImGui::Text("HoldFrames: %d", GetHitKey::HoldFrames(DIK_SPACE));
	//ImGui::End();
}

void Player::Draw()
{
	data.Draw();
	bullet_->Draw();
}