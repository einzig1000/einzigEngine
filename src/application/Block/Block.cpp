#include "Block/Block.h"
#include "Block/BlockDurability.h"
#include "Game.h"

Block::Block()
{
	durability_ = new BlockDurability();
	data_.mass = 100.0f;
}

Block::~Block()
{
	delete durability_;
	durability_ = nullptr;
}

void Block::Initialize()
{
}

void Block::SetBlockType(Blockinfo type)
{
	data_.model = ResourceID::GetModelID(ModelID::Cube);
	data_.texture = ResourceID::GetTextureID(type.type);
	durability_->SetMaxDurability(type.durability);
}

void Block::SetBlockPosition(const Vector3& position)
{
	data_.translate.value = position;
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
		// ブロック本体描画
		data_.Draw();

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
	data_.color = Vector4(float(0x11 * lightEmission_), float(0x11 * lightEmission_), float(0x11 * lightEmission_), float(0xFF));

	// １番目に衝突している時
	if (isCollisionRay == 0)
	{
		data_.color += Vector4(0x22, 0x22, 0x22, 0x00);
	}
}
