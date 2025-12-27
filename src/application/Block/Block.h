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
	// チャンク内の更新
	void Update();

	// インスタンシング描画の配列のインデックス
	uint32_t instanceIndex_ = 0;

	// ブロックの種類設定
	void SetBlockType(Blockinfo info);
	BlockID blockID = BlockID::Air;

	// ブロックの位置設定
	void SetBlockPosition(const Vector3& position);

	BlockID GetBlockID() const { return blockID; };


	// プレイヤーに見られている時、輝度に応じて色を更新
	void UpdateColor();


	Vector4 color_ = { 1.0f,1.0f,1.0f,1.0f };
	Vector3 position_;
	AABB aabb_;

	// 輝度
	uint32_t lightEmission_ = 14;

	// 耐久値
	BlockDurability* durability_;

	// アクティブか
	bool isActive_ = false;
	// 表面に露出しているか
	bool isExposed_ = false;
};