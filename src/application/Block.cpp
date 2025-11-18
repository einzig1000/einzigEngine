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
	isJustDestroyed_ = false;
	if (!isDestroy_)
	{
		DecreaseDurability();
	}
}

void Block::Draw()
{
	if (!isDestroy_ && isExposed_)
	model_.Draw();
}

void Block::DecreaseDurability()
{
	// １番目に衝突しているフラグ
	bool isFirstCollision = model_.isCollisionMouseRay == 0;
	// マウス左ボタンが押されているフラグ
	bool isMouseLeftHeld = Game::Input::Mouse::IsHeld(0);

	// １番目に衝突している
	if (isFirstCollision)
	{
		model_.color = Vector4{ 0xFF, 0xFF, 0xFF, 0xFF };
	}
	else
	{
		model_.color = Vector4{ 0x77, 0x77, 0x77, 0xFF };
	}

	// マウス左ボタンが押されている
	if (isMouseLeftHeld)
	{
		// １番目に衝突している
		if (isFirstCollision)
		{
			// 表面に露出している
			if (isExposed_)
			{
				isBeingDestroyed_ = true;
				// 耐久値があれば減らす
				if (nowDurability_ > 0)
				{
					nowDurability_--;
				}
				else
				{
					model_.scale.value = Vector3(0.001f, 0.001f, 0.001f);
					isJustDestroyed_ = true;
					isBeingDestroyed_ = false;
					isDestroy_ = true;
					isExposed_ = false;
				}
			}
		}
	}
	else
	{
		isBeingDestroyed_ = false;
		nowDurability_ = maxDurability_;
	}
}
