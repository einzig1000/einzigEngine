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

	hand_.counter[0] = std::make_unique<RenderData_Sprite>();
	hand_.counter[0]->anchor = Anchor::Center;
	hand_.counter[1] = std::make_unique<RenderData_Sprite>();
	hand_.counter[1]->anchor = Anchor::Center;

}

HaveItem::~HaveItem()
{}

// アイテム獲得
void HaveItem::AddItem(ItemID id)
{
	// if (既にinventory_に登録されているアイテム && 
	// そのスロットのcountがitem->GetAbleStackCount()未満)　そのスロットに追加
	for (int y = 0; y < 4; ++y)
	{
		for (int x = 0; x < 9; ++x)
		{
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
	UpdateHoverIndex();
	UpdateLeftClick();  // 左クリック（通常・Shift・ダブルクリック）
	UpdateRightClick(); // 右クリック（半分持つ & 1個置く）
	UpdateDrag();       // 左ドラッグ分配

	// 手のアイコンをマウスに追従
	Vector2 mousePos = Game::Input::Mouse::GetPosition();
	if (!hand_.IsEmpty())
	{
		hand_.icon->transforms.translate = Vector3(mousePos.x, mousePos.y, 0);

		if (hand_.count >= 10)
		{
			int tens = hand_.count / 10;
			int units = hand_.count % 10;

			hand_.counter[0]->texture = ResourceID::GetNumberTextureID(tens);
			hand_.counter[1]->texture = ResourceID::GetNumberTextureID(units);

			hand_.counter[0]->transforms.translate = Vector3(mousePos.x + 16, mousePos.y + 16, 0);
			hand_.counter[1]->transforms.translate = Vector3(mousePos.x + 32, mousePos.y + 16, 0);
		}
		else
		{
			hand_.counter[0]->texture = ResourceID::GetNumberTextureID(hand_.count);
			hand_.counter[1]->texture = -1;

			hand_.counter[0]->transforms.translate = Vector3(mousePos.x + 24, mousePos.y + 16, 0);
		}
	}
	else
	{
		// 手が空ならカウンター非表示
		hand_.counter[0]->texture = -1;
		hand_.counter[1]->texture = -1;
	}

	UpdateCounters();
	SyncHotbar();
}

void HaveItem::UpdateHotbar()
{
	SyncHotbar();
}


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
	
	if (!hand_.IsEmpty())
	{
		hand_.icon->Draw();
		hand_.counter[0]->Draw();
		hand_.counter[1]->Draw();
	}
}

void HaveItem::DrawHotbar()
{
	for (int x = 0; x < 9; ++x)
	{
		auto& slot = hotbar_[x];
		slot.icon->Draw();
		slot.counter[0]->Draw();
		slot.counter[1]->Draw();
	}
}


void HaveItem::UpdateHoverIndex()
{
	Vector2 mousePos = Game::Input::Mouse::GetPosition();
	hoverIndex_ = { -1, -1 };

	for (int y = 0; y < 4; ++y)
	{
		for (int x = 0; x < 9; ++x)
		{
			Vector3 pos = baseInventoryPositions_[y][x];

			if (mousePos.x >= pos.x - 32 && mousePos.x <= pos.x + 32 &&
				mousePos.y >= pos.y - 32 && mousePos.y <= pos.y + 32)
			{
				hoverIndex_ = { x, y };
			}
		}
	}

}

void HaveItem::UpdateLeftClick()
{
	if (!Game::Input::Mouse::IsJustReleased(0))
		return;

	// ダブルクリック判定
	int currentFrame = Game::Time::GetElapsedTime();
	bool isDoubleClick = false;
	if (currentFrame - lastClickFrame_ <= 10) // 10フレーム以内ならダブルクリック扱い
		isDoubleClick = true;
	lastClickFrame_ = currentFrame;

	bool shift = Game::Input::Key::IsHeld(DIK_LSHIFT) || Game::Input::Key::IsHeld(DIK_RSHIFT);

	if (hoverIndex_ == Vector2int{ -1, -1 })
		return;

	auto& slot = inventory_[hoverIndex_.y][hoverIndex_.x];

	// ダブルクリック：手に持っているアイテムと同じものを全て集める
	if (isDoubleClick && !hand_.IsEmpty())
	{
		ItemID target = hand_.item.GetID();
		int able = hand_.item.GetAbleStackCount();

		for (int y = 0; y < 4; ++y)
		{
			for (int x = 0; x < 9; ++x)
			{
				auto& s = inventory_[y][x];

				if (s.item.GetID() == target)
				{
					int canTake = able - hand_.count;
					if (canTake <= 0) return;

					int take = std::min<int>(s.count, canTake);
					hand_.count += take;
					s.count -= take;

					if (s.count == 0)
					{
						s.item.Initialize(ItemID::None);
						s.icon->texture = -1;
					}
				}
			}
		}

		return;
	}

	// Shift クリック：自動移動（上3行⇔下段）
	if (shift)
	{
		if (slot.item.GetID() == ItemID::None || slot.count == 0)
			return;

		int targetRow = (hoverIndex_.y == 3) ? 0 : 3;

		// 既存スタックに詰める
		for (int x = 0; x < 9; ++x)
		{
			auto& dst = inventory_[targetRow][x];
			if (dst.item.GetID() == slot.item.GetID() &&
				dst.count < dst.item.GetAbleStackCount())
			{
				int able = dst.item.GetAbleStackCount();
				int total = dst.count + slot.count;
				if (total <= able)
				{
					dst.count = total;
					slot.item.Initialize(ItemID::None);
					slot.count = 0;
					slot.icon->texture = -1;
					return;
				}
				else
				{
					dst.count = able;
					slot.count = total - able;
				}
			}
		}

		// 空スロットに移動
		for (int x = 0; x < 9; ++x)
		{
			auto& dst = inventory_[targetRow][x];
			if (dst.item.GetID() == ItemID::None)
			{
				dst.item = slot.item;
				dst.count = slot.count;
				dst.icon->texture = slot.icon->texture;

				slot.item.Initialize(ItemID::None);
				slot.count = 0;
				slot.icon->texture = -1;
				return;
			}
		}

		return;
	}

	// 通常左クリック（マイクラ式）
	if (hand_.IsEmpty())
	{
		if (slot.item.GetID() != ItemID::None && slot.count > 0)
		{
			hand_.item = slot.item;
			hand_.count = slot.count;
			hand_.icon->texture = slot.icon->texture;

			slot.item.Initialize(ItemID::None);
			slot.count = 0;
			slot.icon->texture = -1;
		}
	}
	else
	{
		if (slot.item.GetID() == ItemID::None)
		{
			slot.item = hand_.item;
			slot.count = hand_.count;
			slot.icon->texture = hand_.icon->texture;
			hand_.Clear();
		}
		else if (slot.item.GetID() == hand_.item.GetID())
		{
			int able = slot.item.GetAbleStackCount();
			int total = slot.count + hand_.count;

			if (total <= able)
			{
				slot.count = total;
				hand_.Clear();
			}
			else
			{
				slot.count = able;
				hand_.count = total - able;
			}
		}
		else
		{
			std::swap(slot.item, hand_.item);
			std::swap(slot.count, hand_.count);
			std::swap(slot.icon->texture, hand_.icon->texture);
		}
	}
}

void HaveItem::UpdateRightClick()
{
	if (!Game::Input::Mouse::IsJustPressed(1))
		return;

	if (hoverIndex_ == Vector2int{ -1, -1 })
		return;

	auto& slot = inventory_[hoverIndex_.y][hoverIndex_.x];

	// 手が空 → スロットの半分を手に持つ
	if (hand_.IsEmpty())
	{
		if (slot.item.GetID() == ItemID::None || slot.count == 0)
			return;

		int half = slot.count / 2;
		int remain = slot.count - half;

		if (half == 0)
		{
			half = 1;
			remain = slot.count - 1;
		}

		hand_.item = slot.item;
		hand_.count = half;
		hand_.icon->texture = slot.icon->texture;

		slot.count = remain;
		if (slot.count == 0)
		{
			slot.item.Initialize(ItemID::None);
			slot.icon->texture = -1;
		}
	}
	else
	{
		// 手に何か持っている → 1個だけ置く
		if (slot.item.GetID() == ItemID::None)
		{
			slot.item = hand_.item;
			slot.count = 1;
			slot.icon->texture = hand_.icon->texture;

			hand_.count--;
			if (hand_.count == 0)
				hand_.Clear();
		}
		else if (slot.item.GetID() == hand_.item.GetID())
		{
			if (slot.count < slot.item.GetAbleStackCount())
			{
				slot.count++;
				hand_.count--;
				if (hand_.count == 0)
					hand_.Clear();
			}
		}
		else
		{
			// 右クリックで違うアイテムのスロットは何もしない
		}
	}
}

void HaveItem::UpdateDrag()
{
	bool leftDown = Game::Input::Mouse::IsHeld(0);
	bool leftJustPressed = Game::Input::Mouse::IsJustPressed(0);
	bool leftJustReleased = Game::Input::Mouse::IsJustReleased(0);

	// 左ドラッグ開始
	if (leftJustPressed && !hand_.IsEmpty())
	{
		isDragging_ = true;
		dragSlots_.clear();

		if (hoverIndex_ != Vector2int{ -1, -1 })
			dragSlots_.push_back(hoverIndex_);
	}

	// ドラッグ中に通過したスロットを記録
	if (isDragging_ && leftDown)
	{
		if (hoverIndex_ != Vector2int{ -1, -1 })
		{
			if (std::find(dragSlots_.begin(), dragSlots_.end(), hoverIndex_) == dragSlots_.end())
			{
				dragSlots_.push_back(hoverIndex_);
			}
		}
	}

	// 左ボタンを離したとき → 分配実行
	if (isDragging_ && leftJustReleased)
	{
		if (!hand_.IsEmpty() && !dragSlots_.empty())
		{
			int totalSlots = static_cast<int>(dragSlots_.size());
			int perSlot = hand_.count / totalSlots;
			if (perSlot == 0) perSlot = 1;

			for (auto& idx : dragSlots_)
			{
				if (hand_.count == 0) break;

				auto& slot = inventory_[idx.y][idx.x];

				if (slot.item.GetID() == ItemID::None)
				{
					slot.item = hand_.item;
					slot.icon->texture = hand_.icon->texture;
					int put = std::min<int>(perSlot, hand_.count);
					slot.count = put;
					hand_.count -= put;
				}
				else if (slot.item.GetID() == hand_.item.GetID())
				{
					int able = slot.item.GetAbleStackCount();
					int canPut = able - slot.count;
					if (canPut > 0)
					{
						int put = std::min<int>(perSlot, std::min<int>(canPut, hand_.count));
						slot.count += put;
						hand_.count -= put;
					}
				}
			}

			if (hand_.count == 0)
				hand_.Clear();
		}

		isDragging_ = false;
		dragSlots_.clear();
	}
}

void HaveItem::UpdateCounters()
{
	for (int y = 0; y < 4; ++y)
	{
		for (int x = 0; x < 9; ++x)
		{
			auto& slot = inventory_[y][x];
			int count = slot.count;

			if (count >= 10)
			{
				int tens = count / 10;
				int units = count % 10;
				slot.counter[0]->texture = ResourceID::GetNumberTextureID(tens);
				slot.counter[1]->texture = ResourceID::GetNumberTextureID(units);

				slot.counter[0]->transforms.translate =
					baseInventoryPositions_[y][x] + Vector3{ 16.0f, 16.0f, 0.0f };
				slot.counter[1]->transforms.translate =
					baseInventoryPositions_[y][x] + Vector3{ 32.0f, 16.0f, 0.0f };
			}
			else if (count > 0)
			{
				slot.counter[0]->texture = ResourceID::GetNumberTextureID(count);
				slot.counter[1]->texture = -1;

				slot.counter[0]->transforms.translate =
					baseInventoryPositions_[y][x] + Vector3{ 24.0f, 16.0f, 0.0f };
			}
			else
			{
				slot.counter[0]->texture = -1;
				slot.counter[1]->texture = -1;
			}
		}
	}
}


void HaveItem::SyncHotbar()
{
	for (int x = 0; x < 9; ++x)
	{
		auto& src = inventory_[0][x];
		auto& dst = hotbar_[x];

		dst.item = src.item;
		dst.count = src.count;
		dst.icon->texture = src.icon->texture;

		dst.counter[0]->texture = src.counter[0]->texture;
		dst.counter[1]->texture = src.counter[1]->texture;
	}
}
