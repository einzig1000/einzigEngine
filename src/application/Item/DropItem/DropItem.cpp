#include "Item/DropItem/DropItem.h"
#include "Item/Item.h"
#include "Charactor/Player/Player.h"
#include "MapManager/MapManager.h"

DropItem::DropItem()
{
	frame_ = 0;

	renderData_ = std::make_unique<RenderData_Model>();

	renderData_->scale.value = Vector3(0.2f, 0.2f, 0.2f);
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
	velocityY_ = 0.01f;
	accelerationY_ = GRAVITY / 2.0f;
}


void DropItem::Update()
{
    if (isDestroy_) return;

	// --- 揺れ計算 ---
    offsetYForSin_ = sinf(frame_ * 0.04f) * 0.1f;

    // --- 衝突判定 ---
    Vector3 checkPos(InitPos_.x, targetY_, InitPos_.z);
    bool isSolid = mapManager_->isSolidAt(checkPos);

    if (isSolid)
    {
        // ブロックの上に位置合わせ
        float blockTop = mapManager_->GetAABB(checkPos).max.y;
        targetY_ = blockTop - 0.1f;

        // 落下停止
        velocityY_ = 0.0f;
        accelerationY_ = 0.0f;
    }
    else
    {
        targetY_ += velocityY_;
        velocityY_ += accelerationY_;

        // 足元が空気なら落下再開
        if (accelerationY_ == 0.0f)
        {
            accelerationY_ = GRAVITY / 2.0f;
            velocityY_ = -0.01f;
        }
    }

	renderData_->translate.value.y = targetY_ + offsetYForSin_ + offsetYForGround_;
    renderData_->rotate.value.y += 0.03f;

    renderData_->UpdateLocalMatrix();
    renderData_->UpdateWorldMatrix();
    renderData_->UpdateAABB();
    renderData_->UpdateInPicture();
    renderData_->SavePreTransforms();

    frame_++;
}

void DropItem::Draw()
{
	if (!isDestroy_)
		renderData_->Draw();

	ImGui::Begin("DropItem Info");
	// 実際の描画座標
	ImGui::Text("Position: (%.2f, %.2f, %.2f)", renderData_->translate.value.x, renderData_->translate.value.y, renderData_->translate.value.z);
	// 揺れを除いた基準座標
	ImGui::Text("Base Position: (%.2f, %.2f, %.2f)", InitPos_.x, targetY_, InitPos_.z);
	// 揺れオフセット
	ImGui::Text("Offset Position: (0.0f %.2f 0.0f)", offsetYForSin_);
	// 速度・加速度
	ImGui::Text("Velocity Y: %.4f", velocityY_);
	ImGui::Text("Acceleration Y: %.4f", accelerationY_);
	ImGui::End();
}
