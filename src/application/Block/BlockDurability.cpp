#include "Block/BlockDurability.h"
#include "Game.h"

void BlockDurability::Update()
{
	if (isDestroy_ == true) return;

	// 視線レイと衝突している
	if (isCollisionRay_)
	{
		updateRequest_ = true;

		// マウス左クリックが押されている間
		if (Game::Input::Mouse::IsHeld(0))
		{
			DecreaseDurability(1);
		}
		else
		{
			// 非破壊中
			isBeingDestroyed_ = false;
			// 破壊フレーム数リセット
			destroyFrame_ = 0;
			// 耐久値回復
			nowDurability_ = maxDurability_;
		}
	}
	// 視線レイと衝突しなくなった瞬間
	else
	{
		updateRequest_ = false;

		// 破壊中フラグを下ろす
		isBeingDestroyed_ = false;
		// 破壊フレーム数リセット
		destroyFrame_ = 0;
		// 耐久値回復
		nowDurability_ = maxDurability_;
	}
}

void BlockDurability::DecreaseDurability(int power)
{
	nowDurability_ -= power;
	destroyFrame_++;
	if (nowDurability_ <= 0)
	{
		nowDurability_ = 0;
		isDestroy_ = true;
	}
}

void BlockDurability::SetMaxDurability(int maxDurability)
{
	maxDurability_ = maxDurability;
	nowDurability_ = maxDurability_;

	if (nowDurability_ <= 0)
	{
		isDestroy_ = true;
	}
	else
	{
		isDestroy_ = false;
	}
}

uint32_t BlockDurability::GetBreakStage() const
{
	// 0~5段階で返す
	if (isDestroy_) return 5;
	if (!isBeingDestroyed_) return 0;
	float ratio = static_cast<float>(nowDurability_) / static_cast<float>(maxDurability_);
	return static_cast<uint32_t>(5 - std::ceil(ratio * 5.0f));
}
