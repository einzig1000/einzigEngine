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
}

void Block::SetBlockPosition(const Vector3& position)
{
	position_ = position;
	aabb_.min = position - Vector3(BLOCK_SIZE / 2.0f, BLOCK_SIZE / 2.0f, BLOCK_SIZE / 2.0f);
	aabb_.max = position + Vector3(BLOCK_SIZE / 2.0f, BLOCK_SIZE / 2.0f, BLOCK_SIZE / 2.0f);
	durability_->SetPosition(position);
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
	if (isCollisionRay = 0)
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
	//color = Vector4(float(0x11 * lightEmission_), float(0x11 * lightEmission_), float(0x11 * lightEmission_), float(0xFF));

	// １番目に衝突している時
	if (isCollisionRay == 0)
	{
		//color += Vector4(0x22, 0x22, 0x22, 0x00);
	}
}
