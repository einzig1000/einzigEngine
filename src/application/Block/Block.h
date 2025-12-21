#pragma once
#include "definition/definition.h"
#include "DrawSystem/RenderData/RenderData.h"

class BlockDurability;

class Block
{
public:
	Block();
	~Block();
	void Initialize();
	void Update();
	void Draw();

	// インスタンシング描画の配列のインデックス
	uint32_t instanceIndex_ = 0;


	// ブロックの種類設定
	void SetBlockType(Blockinfo info);

	// ブロックの位置設定
	void SetBlockPosition(const Vector3& position);

	BlockID GetBlockID() const { return blockID; };

	// 破壊更新
	void UpdateBreak(int power);

	// プレイヤーに見られている時、輝度に応じて色を更新
	void UpdateColor();

	BlockID blockID = BlockID::Air;

	Vector4 color_ = { 1.0f,1.0f,1.0f,1.0f };
	Vector3 position_;
	AABB aabb_;

	// 輝度
	uint32_t lightEmission_ = 14;

	// 耐久値
	BlockDurability* durability_;

	// アクティブか(空気じゃないか)
	bool isActive_ = false;
	// 表面に露出しているか
	bool isExposed_ = false;
	// レイと衝突しているか
	bool isCollisionRay = false;
	// 衝突した方向
	DirectionXYZ direction = DirectionXYZ::None;
};