#include "Block/BlockDurability.h"
#include "Game.h"

void BlockDurability::Update(int power)
{
	// 破壊された瞬間フラグを毎フレームリセット
	isJustDestroyed_ = false;

	if (!isDestroy_)
	{
		if (power <= 0)
		{
			// 非破壊中
			isBeingDestroyed_ = false;
			// 破壊フレーム数リセット
			destroyFrame_ = 0;
			// 耐久値回復
			nowDurability_ = maxDurability_;
			// 破壊済みフラグリセット
			isDestroy_ = false;
		}
		else
		{
			// 破壊中
			isBeingDestroyed_ = true;
			// 破壊フレーム数加算
			destroyFrame_++;

			if (nowDurability_ > 0)
			{
				// 耐久値減少
				nowDurability_ -= power;
				// 耐久値が０以下になったら破壊済みフラグを立てる
				if (nowDurability_ <= 0)
				{
					// 耐久値を０に固定
					nowDurability_ = 0;
					// 破壊された瞬間フラグを立てる
					isJustDestroyed_ = true;
					// 破壊済みフラグを立てる
					isDestroy_ = true;
					// 破壊中フラグを下ろす
					isBeingDestroyed_ = false;
				}
			}
		}
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

void BlockDurability::DrawBreakEffect()
{
	if (isBeingDestroyed_)
	{
		// 耐久値を五段階に分けて破壊テクスチャを変更
		uint32_t breakStage = (maxDurability_ - nowDurability_) * 5 / maxDurability_;
		//breakEffect.texture = ResourceID::GetTextureID(TextureID(uint32_t(TextureID::BreakBlock_0) + breakStage));
		//breakEffect.Draw();
	}
}
