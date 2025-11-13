#include "Block.h"
#include "ResourceID.h"
#include "Game.h"

Block::Block()
{
	nowDurability_ = 0;
	maxDurability_ = 60;
	destroyFrame_ = 0;
}

Block::~Block()
{}




void Block::Initialize(const Vector3 & position)
{
	model_.translate.value = position;
	model_.model = ResourceID::blockModelIDs_[int(ModelID::Cube)];
}

void Block::Update()
{
	DecreaseDurability();
}

void Block::Draw()
{
	if (!isDestroy_ && isExposed_)
	model_.Draw();
}

void Block::DecreaseDurability()
{
	if (destroyFrame_ > 0)
	{
		nowDurability_--;
		if (nowDurability_ < 0)
		{
			isDestroy_ = true;
		}
	}

	if (model_.isCollisionMouseRay != 0)
	{
		destroyFrame_ = 0;
		nowDurability_ = maxDurability_;
	}
}
