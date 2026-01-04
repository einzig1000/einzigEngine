#include "BaseCharactor.h"
#include "MapManager/MapManager.h"
#include "MapManager/Chunk/Chunk.h"
#include "MapManager/Chunk/Block/Block.h"
#include "MapManager/Chunk/Block/BlockDurability.h"
#include "UIManager/UIManager.h"

// 見ているブロックをtargetBlock_にセットする
void BaseCharactor::SetTargetBlock()
{
	// 前フレームの見ているブロック保存
	preTargetBlock_ = targetBlock_;
	// 見ているブロック取得
	targetBlock_ = mapManager_->GetBlockByCrossedRay(viewRay_, maxDistance);
}


void BaseCharactor::AddItem(ItemID id)
{
	haveItem_->AddItem(id);
}


void BaseCharactor::SetMapManager(MapManager* mapManager)
{
	mapManager_ = mapManager;
}

void BaseCharactor::SetUIManager(UIManager* uiManager)
{
	uiManager_ = uiManager;
}

void BaseCharactor::Jump()
{
	// 接地していないならジャンプ不可
	//if (!isGrounded_) return;

	data_.translate.velocity.y = jumpPower_;
	data_.translate.acceleration.y = GRAVITY; // ジャンプ後は重力を戻す
	isGrounded_ = false;
}

void BaseCharactor::Move(const Vector3& direction, float speed)
{
	data_.translate.velocity.x = direction.x * speed;
	data_.translate.velocity.z = direction.z * speed;
}


void BaseCharactor::BreakTargetBlock()
{
	if (!targetBlock_.has_value()) return;

	lookAtBlock lab = targetBlock_.value();
	Block* block = lab.block;
	if (!block) return;

	block->durability_->DecreaseDurability(breakPower_);

	// 破壊されていたら非アクティブ化
	if (block->durability_->GetIsDestroy())
	{
		mapManager_->DestroyBlockAt(lab.chunkIndex, lab.localIndex);
	}
}

void BaseCharactor::SetNewBlock(BlockID id)
{
	if (!targetBlock_.has_value()) return;

	lookAtBlock lab = targetBlock_.value();

	if (lab.block->blockInfo_.isExtraAction)
	{
		uiManager_->ChangeScreen(UIMode::Crafting);
		// 特殊ブロックの上に置けない
		return;
	}

	// 当たった面情報が無いなら置けない
	if (lab.face == AABBFace::NONE) return;


	// MapManagerから設置処理を呼び出す
	mapManager_->SetBlockAt(lab, id);

	// アイテムを1つ消費
	haveItem_->RemoveCurrentSelectedItem(1);
}
