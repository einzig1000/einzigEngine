#pragma once
#include "definition/definition.h"
#include "DrawSystem/RenderData/RenderData.h"

class Block
{
public:
	Block();
	~Block();
	void Initialize(const Vector3& position);
	void Update();
	void Draw();

	// ブロック
	RenderData_Model model_;

	// 耐久値
	void DecreaseDurability();
	int nowDurability_;
	int maxDurability_;
	int destroyFrame_;
	// 破壊中か
	bool isBeingDestroyed_ = false;
	// 破壊済みか
	bool isDestroy_ = false;
	// 破壊された瞬間か
	bool isJustDestroyed_ = false;

	// 表面に露出しているか
	bool isExposed_ = false;

	// レイと衝突しているか
	int isCollisionRay = -1;
	DirectionXYZ direction = DirectionXYZ::None;
};

