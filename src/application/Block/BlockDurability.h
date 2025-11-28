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
	int nowDurability_ = 0;
	int maxDurability_ = 60;
	int destroyFrame_ = 0; 

	// 破壊中か
	bool isBeingDestroyed_ = false;
	// 破壊済みか
	bool isDestroy_ = false;
	// 破壊された瞬間か
	bool isJustDestroyed_ = false;

	// 破壊テクスチャ
	RenderData_Model breakEffect;

};

