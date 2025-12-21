#include "Block/Block.h"
#include "Block/BlockDurability.h"
#include "Game.h"

Block::Block()
{
	durability_ = new BlockDurability();
}

Block::~Block()
{
	delete durability_;
	durability_ = nullptr;
}

void Block::Initialize()
{
}

void Block::SetBlockType(Blockinfo info)
{
	blockID = info.type;
	durability_->SetMaxDurability(info.durability);
	isActive_ = true;
}

void Block::SetBlockPosition(const Vector3& position)
{
	position_ = position;
	aabb_.min = position - Vector3(BLOCK_SIZE / 2.0f, BLOCK_SIZE / 2.0f, BLOCK_SIZE / 2.0f);
	aabb_.max = position + Vector3(BLOCK_SIZE / 2.0f, BLOCK_SIZE / 2.0f, BLOCK_SIZE / 2.0f);
}

void Block::Update()
{
	// 破壊されていないかつ表面に露出している
	if (!durability_->GetIsDestroy() && isExposed_)
	{
		// 色更新
		UpdateColor();

		// プレイヤーに見られている時の更新処理
		UpdateBreak(1);
	}
	if (durability_->GetIsDestroy())
	{
		isActive_ = false;
	}
}

void Block::Draw()
{
	// 破壊されていないかつ表面に露出している
	if (!durability_->GetIsDestroy() && isExposed_)
	{
		// 破壊エフェクト描画
		durability_->DrawBreakEffect();
	}
}


void Block::UpdateBreak(int power)
{
	// １番目に衝突している時
	if (isCollisionRay == true)
	{
		// マウス左ボタンが押されている時
		if (Game::Input::Mouse::IsHeld(0))
		{
			// 耐久値更新
			durability_->Update(power);
		}
	}
}

void Block::UpdateColor()
{
	// 輝度に応じて色を変更
	lightEmission_ = std::clamp(lightEmission_, 0u, 14u);
	
	color_ = Vector4(float(0x11 * lightEmission_), float(0x11 * lightEmission_), float(0x11 * lightEmission_), float(0xFF));
	color_ = Vector4(
		Game::Math::RandInt(color_.x - 8, color_.x + 8),
		Game::Math::RandInt(color_.y - 8, color_.y + 8),
		Game::Math::RandInt(color_.z - 8, color_.z + 8),
		color_.w);
	color_.x = std::clamp(color_.x, 0.0f, 255.0f);
	color_.y = std::clamp(color_.y, 0.0f, 255.0f);
	color_.z = std::clamp(color_.z, 0.0f, 255.0f);


	// １番目に衝突している時
	if (isCollisionRay == true)
	{
		color_ = Vector4(0x22, 0x22, 0x22, 0x00);
	}
}
