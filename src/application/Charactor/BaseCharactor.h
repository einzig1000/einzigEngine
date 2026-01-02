#pragma once
#include "Game.h"
#include "definition/definition.h"
#include "Item/HaveItem/HaveItem.h"

class MapManager;
class Block;

class BaseCharactor
{
public:
	virtual ~BaseCharactor() = default;

	virtual void Initialize() = 0;
	virtual void SetMapManager(MapManager* mapManager);
	virtual void SetHaveItem() { haveItem_ = std::make_unique<HaveItem>(); }
	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual void DrawImGui() = 0;

	// 移動
	virtual void Move(const Vector3& direction, float speed);
	// ジャンプ
	virtual void Jump();
	// ターゲットブロック破壊
	virtual void BreakTargetBlock();
	// ブロック設置
	virtual void SetNewBlock(BlockID id);


	// 見ているブロックをtargetBlock_にセットする
	virtual void SetTargetBlock();
	// 取得した視線レイをセットする
	virtual void SetViewRay(Ray ray) { viewRay_ = ray; }


	// アイテム獲得
	virtual void AddItem(ItemID id);



	RenderData_Model data_;		// データ
	Ray viewRay_;				// 視線レイ
	float maxDistance = 5.0f;	// 視線範囲

protected:

	std::unique_ptr<HaveItem> haveItem_; // 所持アイテム管理

	MapManager* mapManager_ = nullptr;
	std::optional<lookAtBlock> targetBlock_;	// ターゲットにしているブロック
	std::optional<lookAtBlock> preTargetBlock_;// 前フレームでターゲットにしていたブロック

	

	float jumpPower_ = 0.1491f;	// ジャンプ力
	float breakPower_ = 1;	// ブロック破壊力


	bool isGrounded_ = false;
};

