#pragma once
#include "DrawSystem/RenderData/RenderData.h"

class BlockDurability
{
public:
	void Update(int power);
	void SetMaxDurability(int maxDurability);
	bool GetIsDestroy() const { return isDestroy_; }
	void SetPosition(const Vector3& position)
	{
		breakEffect.translate.value = position;
	}

	void DrawBreakEffect();

private:
	int32_t nowDurability_ = 0;
	int32_t maxDurability_ = 60;
	int32_t destroyFrame_ = 0; 

	// 破壊中か
	bool isBeingDestroyed_ = false;
	// 破壊済みか
	bool isDestroy_ = false;
	// 破壊された瞬間か
	bool isJustDestroyed_ = false;

	// 破壊テクスチャ
	RenderData_Model breakEffect;

};

