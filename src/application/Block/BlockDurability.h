#pragma once
#include "DrawSystem/RenderData/RenderData.h"

class BlockDurability
{
public:
	void Update();
	void DecreaseDurability(int power);
	void SetIsCollisionRay(bool* isCollisionRay) { isCollisionRay_ = isCollisionRay; }

	void SetMaxDurability(int maxDurability);
	bool GetIsDestroy() const { return isDestroy_; }
	uint32_t GetBreakStage() const;

private:
	int32_t nowDurability_ = 0;
	int32_t maxDurability_ = 60;
	int32_t destroyFrame_ = 0; 

	
	bool isBeingDestroyed_ = false;	// 破壊中か
	bool isDestroy_ = false;		// 破壊済みか
	bool isJustDestroyed_ = false;	// 破壊された瞬間か

	
	bool* isCollisionRay_;			// 視線レイと衝突しているかポインタ
};

