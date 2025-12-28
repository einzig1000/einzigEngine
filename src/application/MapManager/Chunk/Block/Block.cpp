#include "MapManager/Chunk/Block/Block.h"
#include "MapManager/Chunk/Block/BlockDurability.h"
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
	isTransparent_ = info.isTransparent;
	durability_->SetMaxDurability(info.durability);
}

void Block::SetBlockPosition(const Vector3& position)
{
	position_ = position;
	aabb_.min = position - Vector3(BLOCK_SIZE / 2.0f, BLOCK_SIZE / 2.0f, BLOCK_SIZE / 2.0f);
	aabb_.max = position + Vector3(BLOCK_SIZE / 2.0f, BLOCK_SIZE / 2.0f, BLOCK_SIZE / 2.0f);
}

void Block::Update()
{
	// 表面に露出していなかったらreturn
	if (!isExposed_) return;

	// 色更新
	//UpdateColor();

	// 耐久値更新
	durability_->Update();
}

void Block::UpdateColor()
{
	lightEmission_ = std::clamp(lightEmission_, 0u, 9u);

	float emission = 1.0f * (float(lightEmission_) / 9.0f);

	// 輝度に応じて色を変更
	color_ = Vector4(emission, emission, emission, 1.0f);
}
