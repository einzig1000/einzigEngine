#pragma once
#include "DrawSystem/RenderData/RenderData.h"

class BlockDurability
{
public:
	void Update(int power);
	void SetMaxDurability(int maxDurability);
	bool GetIsDestroy() const { return isDestroy_; }

	void DrawBreakEffect();

private:
	int32_t nowDurability_ = 0;
	int32_t maxDurability_ = 60;
	int32_t destroyFrame_ = 0; 

	Vector4 color_ = { 255,255,255,255 };

	// 破壊中か
	bool isBeingDestroyed_ = false;
	// 破壊済みか
	bool isDestroy_ = false;
	// 破壊された瞬間か
	bool isJustDestroyed_ = false;

};

