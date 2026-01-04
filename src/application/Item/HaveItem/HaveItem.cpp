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

			inventory_[y][x].item.Initialize(ItemID::None);

			inventory_[y][x].count = 0;
			inventory_[y][x].icon = std::make_unique<RenderData_Sprite>();
			inventory_[y][x].icon->anchor = Anchor::Center;
			inventory_[y][x].icon->transforms.translate = baseInventoryPositions_[y][x];

			inventory_[y][x].counter[0] = std::make_unique<RenderData_Sprite>();
			inventory_[y][x].counter[0]->anchor = Anchor::Center;
			inventory_[y][x].counter[0]->transforms.translate = baseInventoryPositions_[y][x];
			inventory_[y][x].counter[0]->transforms.translate += Vector3{ 16.0f, 16.0f, 0.0f };

			inventory_[y][x].counter[1] = std::make_unique<RenderData_Sprite>();
			inventory_[y][x].counter[1]->anchor = Anchor::Center;
			inventory_[y][x].counter[1]->transforms.translate = baseInventoryPositions_[y][x];
			inventory_[y][x].counter[1]->transforms.translate += Vector3{ 32.0f, 16.0f, 0.0f };
		}
	}

	for (int x = 0; x < 9; ++x)
	{
		hotbar_[x].item.Initialize(ItemID::None);

		hotbar_[x].count = 0;
		hotbar_[x].icon = std::make_unique<RenderData_Sprite>();
		hotbar_[x].icon->transforms.translate = Vector3{ 320.0f + x * 80.0f, 670.0f,0.0f };
		hotbar_[x].icon->anchor = Anchor::Center;

		hotbar_[x].counter[0] = std::make_unique<RenderData_Sprite>();
		hotbar_[x].counter[0]->anchor = Anchor::Center;
		hotbar_[x].counter[0]->transforms.translate = Vector3{ 320.0f + x * 80.0f + 16.0f, 670.0f + 16.0f,0.0f };

		hotbar_[x].counter[1] = std::make_unique<RenderData_Sprite>();
		hotbar_[x].counter[1]->anchor = Anchor::Center;
		hotbar_[x].counter[1]->transforms.translate = Vector3{ 320.0f + x * 80.0f + 32.0f, 670.0f + 16.0f,0.0f };
	}

	hand_.item.Initialize(ItemID::None);
	hand_.count = 0;
	hand_.icon = std::make_unique<RenderData_Sprite>();
	hand_.icon->anchor = Anchor::Center;
}

HaveItem::~HaveItem()
{}

// アイテム獲得
void HaveItem::AddItem(ItemID id)
{
	for (int y = 0; y < 4; ++y)
	{
		for (int x = 0; x < 9; ++x)
		{
			// if (既にinventory_に登録されているアイテム && 
			// そのスロットのcountがitem->GetAbleStackCount()未満)　そのスロットに追加
			if (inventory_[y][x].item.GetID() == id && inventory_[y][x].count < inventory_[y][x].item.GetAbleStackCount())
			{
				// 既に所持しているアイテムのスロットに追加
				inventory_[y][x].count++;

				return;
			}
		}
	}


	// if (まだもっていない || item->GetAbleStackCount()以下のスロットがない)　新しいスロットに追加
	for (int y = 0; y < 4; ++y)
	{
		for (int x = 0; x < 9; ++x)
		{
			// 空スロットを探す
			if (inventory_[y][x].item.GetID() == ItemID::None && inventory_[y][x].count == 0)
			{
				// 新しいスロットに追加
				inventory_[y][x].item.Initialize(id);
				inventory_[y][x].count = 1;
				inventory_[y][x].icon->texture = ResourceID::Get2DTextureID(id);

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
		if (hoverIndex_ == Vector2int{ -1, -1 }) return;

		// 手が空 → スロットの中身を手に移す
		if (hand_.IsEmpty())
		{
			if (inventory_[hoverIndex_.y][hoverIndex_.x].item.GetID() != ItemID::None)
			{
				hand_.item = inventory_[hoverIndex_.y][hoverIndex_.x].item;
				hand_.count = inventory_[hoverIndex_.y][hoverIndex_.x].count;
				hand_.icon->texture = inventory_[hoverIndex_.y][hoverIndex_.x].icon->texture;

				inventory_[hoverIndex_.y][hoverIndex_.x].item.Initialize(ItemID::None);
				inventory_[hoverIndex_.y][hoverIndex_.x].count = 0;
				inventory_[hoverIndex_.y][hoverIndex_.x].icon->texture = -1;
			}
		}
		else
		{
			// 手に何か持っている → スロットと入れ替える or 結合
			if (inventory_[hoverIndex_.y][hoverIndex_.x].item.GetID() == hand_.item.GetID())
			{
				int able = inventory_[hoverIndex_.y][hoverIndex_.x].item.GetAbleStackCount();
				int total = inventory_[hoverIndex_.y][hoverIndex_.x].count + hand_.count;

				if (total <= able)
				{
					inventory_[hoverIndex_.y][hoverIndex_.x].count = total;
					hand_.Clear();
				}
				else
				{
					inventory_[hoverIndex_.y][hoverIndex_.x].count = able;
					hand_.count = total - able;
				}
			}
			else
			{
				std::swap(inventory_[hoverIndex_.y][hoverIndex_.x].item, hand_.item);
				std::swap(inventory_[hoverIndex_.y][hoverIndex_.x].count, hand_.count);
				std::swap(inventory_[hoverIndex_.y][hoverIndex_.x].icon->texture, hand_.icon->texture);
			}
		}
	}

	//// 左クリック押されたとき
	//if (Game::Input::Mouse::IsJustPressed(0))
	//{
	//	// なにも掴んでないとき
	//	if (!grabbed_)
	//	{
	//		// カーソルが乗ってるインデックスを掴む
	//		if (hoverIndex_ != Vector2int{ -1, -1 })
	//		{
	//			grabbedIndex_ = hoverIndex_;
	//			grabbed_ = true;
	//		}
	//	}
	//	else
	//	{
	//		// 掴んでるインデックスを離す
	//		grabbed_ = false;

	//		// 離した先が有効なインデックスなら
	//		if (hoverIndex_ != Vector2int{ -1, -1 })
	//		{
	//			// 掴んでるインデックスと離した先のインデックスを入れ替える
	//			std::swap(inventory_[grabbedIndex_.y][grabbedIndex_.x], inventory_[hoverIndex_.y][hoverIndex_.x]);
	//			// 入れ替えた後の両方のスロットの位置を元に戻す
	//			inventory_[grabbedIndex_.y][grabbedIndex_.x].icon->transforms.translate =
	//				baseInventoryPositions_[grabbedIndex_.y][grabbedIndex_.x];
	//			inventory_[hoverIndex_.y][hoverIndex_.x].icon->transforms.translate =
	//				baseInventoryPositions_[hoverIndex_.y][hoverIndex_.x];

	//		}
	//		else
	//		{
	//			// 離した先が無効なインデックスなら、掴んでるインデックスのスロットを元の位置に戻す
	//			inventory_[grabbedIndex_.y][grabbedIndex_.x].icon->transforms.translate =
	//				baseInventoryPositions_[grabbedIndex_.y][grabbedIndex_.x];
	//		}
	//	}
	//}

	// 掴んでるインデックスのスロットをマウスに追従させる
	if (!hand_.IsEmpty())
	{
		hand_.icon->transforms.translate = Vector3(mousePos.x, mousePos.y, 0);
	}


	/// countからcounter[]のテクスチャを更新
	for (int y = 0; y < 4; ++y)
	{
		for (int x = 0; x < 9; ++x)
		{
			int count = inventory_[y][x].count;
			if (count >= 10)
			{
				int tens = count / 10;
				int units = count % 10;
				inventory_[y][x].counter[0]->texture = ResourceID::GetNumberTextureID(tens);
				inventory_[y][x].counter[1]->texture = ResourceID::GetNumberTextureID(units);
				inventory_[y][x].counter[0]->transforms.translate = Vector3{
					baseInventoryPositions_[y][x].x + 16.0f,
					baseInventoryPositions_[y][x].y + 16.0f,
					0.0f };
				inventory_[y][x].counter[1]->transforms.translate = Vector3{
					baseInventoryPositions_[y][x].x + 32.0f,
					baseInventoryPositions_[y][x].y + 16.0f,
					0.0f };
			}
			else if (count > 0)
			{
				inventory_[y][x].counter[0]->texture = ResourceID::GetNumberTextureID(count);
				inventory_[y][x].counter[0]->transforms.translate = Vector3{
					baseInventoryPositions_[y][x].x + 24.0f,
					baseInventoryPositions_[y][x].y + 16.0f,
					0.0f };
				inventory_[y][x].counter[1]->texture = -1;
			}
			else
			{
				inventory_[y][x].counter[0]->texture = -1;
				inventory_[y][x].counter[1]->texture = -1;
			}
		}
	}

	// インベントリの下列とホットバー同期
	for (int x = 0; x < 9; ++x)
	{
		hotbar_[x].item = inventory_[3][x].item;
		hotbar_[x].count = inventory_[3][x].count;
		hotbar_[x].icon->texture = inventory_[3][x].icon->texture;
	}
}

void HaveItem::UpdateHotbar()
{

}

// インベントリ描画
void HaveItem::DrawInventory()
{
	for (int y = 0; y < 4; ++y)
	{
		for (int x = 0; x < 9; ++x)
		{
			auto& slot = inventory_[y][x];

			slot.icon->transforms.translate = baseInventoryPositions_[y][x];
			slot.icon->Draw();

			slot.counter[0]->Draw();
			slot.counter[1]->Draw();
		}
	}

	// 手のアイテム
	if (!hand_.IsEmpty())
	{
		hand_.icon->Draw();

	}
}

// ホットバー描画
void HaveItem::DrawHotbar()
{
	for (int x = 0; x < 9; ++x)
	{
		hotbar_[x].icon->Draw();
	}

	// カウンター描画
	for (int x = 0; x < 9; ++x)
	{
		hotbar_[x].counter[0]->Draw();
		hotbar_[x].counter[1]->Draw();
	}
}

void HaveItem::UpdateHoverIndex()
{}

void HaveItem::UpdateLeftClick()
{}

void HaveItem::UpdateRightClick()
{}

void HaveItem::UpdateDrag()
{}

void HaveItem::UpdateCounters()
{}

void HaveItem::SyncHotbar()
{}
