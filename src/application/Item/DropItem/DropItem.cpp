#include "Item/DropItem/DropItem.h"
#include "Item/Item.h"
#include "Charactor/Player/Player.h"
#include "MapManager/MapManager.h"

DropItem::DropItem(Player* player)
{
	player_ = player;
	frame_ = 0;

	renderData_ = std::make_unique<RenderData_Model>();
}

void DropItem::SetItem(ItemID id, Vector3 pos)
{
	item_ = std::make_unique<Item>(id);
	InitPos_ = pos;
	offsetYForSin_ = 0.0f;
	offsetYForGround_ = 0.3f;
	isDestroy_ = false;

	renderData_->SetModel(item_->GetModelHandle());
	renderData_->SetTexture(item_->GetTextureHandleForModel());
	
	renderData_->translate.value = pos;

	// 上向き速度・下向き加速度
	targetY_ = pos.y;
	velocityY_ = 0.13f;
	accelerationY_ = GRAVITY;
}


void DropItem::Update()
{
	if (isDestroy_) return;


	// Y軸sin揺れ計算
	offsetYForSin_ = sinf(frame_ * 0.05f) * 0.2f;

	// 重力落下計算
	velocityY_ += accelerationY_;
	targetY_ += velocityY_;

	// 衝突判定
	Vector3 checkPos(InitPos_.x, targetY_, InitPos_.z);
	bool onGround = mapManager_->isSolidAt(checkPos);

	if (onGround)
	{
		// ブロックの上に位置合わせ
		float blockTop = mapManager_->GetAABB(checkPos).max.y;
		targetY_ = blockTop + offsetYForGround_;

		// 落下停止
		velocityY_ = 0.0f;
		accelerationY_ = 0.0f;
	}
	else
	{
		// 足元が空気なら落下再開
		if (accelerationY_ == 0.0f)
		{
			accelerationY_ = GRAVITY;
			velocityY_ = -0.01f;
		}
	}

	// 描画位置に反映    
	renderData_->translate.value.y = targetY_ + offsetYForSin_;

	frame_++;
}

void DropItem::Draw()
{
	if (!isDestroy_)
		renderData_->Draw();
}
