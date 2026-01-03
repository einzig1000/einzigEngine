#include "Item/HaveItem/HaveItem.h"

HaveItem::HaveItem()
{
	float yOffset[4];
	yOffset[0] = 0.0f;
	yOffset[1] = yOffset[0] - 89.0f;
	yOffset[2] = yOffset[1] - 72.0f;
	yOffset[3] = yOffset[2] - 72.0f;

	for (int y = 0; y < 4; ++y)
	{
		for (int x = 0; x < 9; ++x)
		{
			baseInventoryPositions_[y][x] = Vector3{
				static_cast<float>(352.0f + x * 72),
				static_cast<float>(593.0f + yOffset[y]),
				0.0f };

			inventory_[y][x].count = 0;
			inventory_[y][x].renderDataSprite = std::make_unique<RenderData_Sprite>();
			inventory_[y][x].renderDataSprite->anchor = Anchor::Center;
			inventory_[y][x].renderDataSprite->transforms.translate = baseInventoryPositions_[y][x];
		}
	}

	for (int x = 0; x < 9; ++x)
	{
		hotbar_[x].count = 0;
		hotbar_[x].renderDataSprite = std::make_unique<RenderData_Sprite>();
		hotbar_[x].renderDataSprite->transforms.translate = Vector3{
			320.0f + x * 80.0f,
			670.0f,
			0.0f };
		hotbar_[x].renderDataSprite->anchor = Anchor::Center;
	}
}

HaveItem::~HaveItem()
{}

// アイテム獲得
void HaveItem::AddItem(ItemID id)
{
	for (auto& row : inventory_)
	{
		for (auto& slot : row)
		{
			if (slot.item != nullptr)
			{
				// if (既にinventory_に登録されているアイテム && 
				// そのスロットのcountがitem->GetAbleStackCount()未満)　そのスロットに追加
				if (slot.item->GetID() == id && slot.count < slot.item->GetAbleStackCount())
				{
					// 既に所持しているアイテムのスロットに追加
					slot.count++;
					slot.renderDataSprite->texture = ResourceID::Get2DTextureID(id);
					return;
				}
			}
		}
	}

	// if (まだもっていない || item->GetAbleStackCount()以下のスロットがない)　新しいスロットに追加
	for (auto& row : inventory_)
	{
		for (auto& slot : row)
		{
			// 空スロットを探す
			if (slot.item == nullptr || slot.count == 0)
			{
				// 新しいスロットに追加
				slot.item = nullptr;
				slot.item = std::make_unique<Item>(id);
				slot.count = 1;
				slot.renderDataSprite->texture = ResourceID::Get2DTextureID(id);
				return;
			}
		}
	}
}

void HaveItem::UpdateInventry()
{
	Vector2 mousePos = Game::Input::Mouse::GetPosition();

	// カーソルが乗ってるインデックスを調べる
	hoverIndex_ = { -1, -1 };
	for (int y = 0; y < 4; ++y)
	{
		for (int x = 0; x < 9; ++x)
		{
			Vector3 slotPos = baseInventoryPositions_[y][x];
			// スロットの範囲内にカーソルがあるか
			if (mousePos.x >= slotPos.x - 32.0f &&
				mousePos.x <= slotPos.x + 32.0f &&
				mousePos.y >= slotPos.y - 32.0f &&
				mousePos.y <= slotPos.y + 32.0f)
			{
				hoverIndex_ = { x, y };
			}
		}
	}

	// 左クリック押されたとき
	if (Game::Input::Mouse::IsJustPressed(0))
	{
		// なにも掴んでないとき
		if (!grabbed_)
		{
			// カーソルが乗ってるインデックスを掴む
			if (hoverIndex_ != Vector2int{ -1, -1 })
			{
				grabbedIndex_ = hoverIndex_;
				grabbed_ = true;
			}
		}
		else
		{
			// 掴んでるインデックスを離す
			grabbed_ = false;

			// 離した先が有効なインデックスなら
			if (hoverIndex_ != Vector2int{ -1, -1 })
			{
				//// 掴んでるインデックスと離した先のインデックスを入れ替える
				//// 掴んでるインデックスのスロットを元の位置に戻す
				//inventory_[grabbedIndex_.y][grabbedIndex_.x].renderDataSprite->transforms.translate =
				//	baseInventoryPositions_[grabbedIndex_.y][grabbedIndex_.x];
				//// 離した先にアイテムがあったなら代わりにそのアイテムを掴む
				//if (inventory_[grabbedIndex_.y][grabbedIndex_.x].count > 0)
				//{
				//	grabbedIndex_ = hoverIndex_;
				//	grabbed_ = true;
				//}

				//inventory_[grabbedIndex_.y][grabbedIndex_.x].renderDataSprite->transforms.translate =
				//	baseInventoryPositions_[grabbedIndex_.y][grabbedIndex_.x];

				//std::swap(
				//	inventory_[grabbedIndex_.y][grabbedIndex_.x],
				//	inventory_[hoverIndex_.y][hoverIndex_.x]);

				// 入れ替える
				inventory_[grabbedIndex_.y][grabbedIndex_.x].renderDataSprite->transforms.translate =
					baseInventoryPositions_[hoverIndex_.y][hoverIndex_.x];
				inventory_[hoverIndex_.y][hoverIndex_.x].renderDataSprite->transforms.translate =
					baseInventoryPositions_[grabbedIndex_.y][grabbedIndex_.x];

				//// 離した先にアイテムがあったなら代わりにそのアイテムを掴む
				//if (inventory_[grabbedIndex_.y][grabbedIndex_.x].count > 0)
				//{
				//	grabbedIndex_ = hoverIndex_;
				//	grabbed_ = true;
				//}
			}
			else
			{
				// 離した先が無効なインデックスなら、掴んでるインデックスのスロットを元の位置に戻す
				inventory_[grabbedIndex_.y][grabbedIndex_.x].renderDataSprite->transforms.translate =
					baseInventoryPositions_[grabbedIndex_.y][grabbedIndex_.x];
			}
		}
	}

	// 掴んでるインデックスのスロットをマウスに追従させる
	if (grabbed_)
	{
		inventory_[grabbedIndex_.y][grabbedIndex_.x].renderDataSprite->transforms.translate = Vector3{
			mousePos.x,
			mousePos.y,
			0.0f };
	}
}

void HaveItem::UpdateHotbar()
{

}

// インベントリ描画
void HaveItem::DrawInventory()
{
	// 9x4全て描画
	for (int y = 0; y < 4; ++y)
	{
		for (int x = 0; x < 9; ++x)
		{
			//if (grabbedIndex_ != Vector2int{ x, y })
				inventory_[y][x].renderDataSprite->Draw();
		}
	}

	//if (grabbed_)
	//{
	//	// 掴んでるインデックスのスロットを最後に描画
	//	inventory_[grabbedIndex_.y][grabbedIndex_.x].renderDataSprite->Draw();

	//}

	//ImGui::Begin("Inventry");
	//for (int y = 0; y < 4; ++y)
	//{
	//	for (int x = 0; x < 9; ++x)
	//	{
	//	}
	//}
	//ImGui::End();
}

// ホットバー描画
void HaveItem::DrawHotbar()
{
	for (int x = 0; x < 9; ++x)
	{
		hotbar_[x].renderDataSprite->Draw();
	}
}
