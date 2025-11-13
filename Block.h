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
	bool isDestroy_ = false;

	// 表面に露出しているか
	bool isExposed_ = false;
};

