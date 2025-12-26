#include "Block/Block.h"
#include "Block/BlockDurability.h"
#include "Game.h"

Block::Block()
{
	durability_ = new BlockDurability();
	isActive_ = false;
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
	if (blockID != BlockID::Air)isActive_ = true;
}

void Block::SetBlockPosition(const Vector3& position)
{
	position_ = position;
	aabb_.min = position - Vector3(BLOCK_SIZE / 2.0f, BLOCK_SIZE / 2.0f, BLOCK_SIZE / 2.0f);
	aabb_.max = position + Vector3(BLOCK_SIZE / 2.0f, BLOCK_SIZE / 2.0f, BLOCK_SIZE / 2.0f);
}

void Block::Update()
{
	if (!isExposed_) return;

	// 色更新
	UpdateColor();

	// プレイヤーに見られている時の更新処理
	UpdateBreak(1);

	// 破壊されていたら非アクティブ化
	if (durability_->GetIsDestroy())
	{
		isActive_ = false;
	}
}

void Block::UpdateBreak(int power)
{
	// １番目に衝突している時 || 更新リクエストがある時
	if (isCollisionRay == true || durability_->GetUpdateRequest())
	{
		durability_->SetIsCollisionRay(isCollisionRay);
		// 耐久値更新
		durability_->Update();
	}
}

void Block::UpdateColor()
{
	lightEmission_ = std::clamp(lightEmission_, 0u, 9u);

	float emission = 1.0f * (float(lightEmission_) / 9.0f);

	// 輝度に応じて色を変更
	color_ = Vector4(emission, emission, emission, 1.0f);

	//// １番目に衝突している時
	//if (isCollisionRay == true)
	//{
	//	color_ = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
	//}
}
