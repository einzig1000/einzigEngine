#include "Itemslot.h"
#include "ResourceID.h"

Itemslot::Itemslot()
{
	for (int i = 0; i < 10; i++)
	{
		slotSprite_[i].texture = ResourceID::TextureIDs_[int(TextureID::Item_slot)];
		slotSprite_[i].transforms.scale = Vector3(0.5f, 0.5f, 1.0f);
		slotSprite_[i].transforms.translate = Vector3(640.0f + (i - 5) * 60.0f, 680.0f, 0.0f);
		slotSprite_[i].color = 0x999999FF;

		itemSprite_[i].texture = ResourceID::TextureIDs_[int(TextureID::UVChecker)];
		itemSprite_[i].transforms.scale = Vector3(0.1f, 0.1f, 1.0f);
		itemSprite_[i].transforms.translate = Vector3(640.0f + (i - 5) * 60.0f, 680.0f, 0.0f);
	}

}

void Itemslot::Update()
{
	for (int i = 0; i < 10; i++)
	{
		if (i == selectedSlot_)
		{
			slotSprite_[i].color = 0xFFFFFFFF;
		}
		else
		{
			slotSprite_[i].color = 0x999999FF;
		}
	}

	int mouseWheel = Game::Input::Mouse::GetMouseWheel();
	if (mouseWheel > 0)
	{
		selectedSlot_--;
		if (selectedSlot_ < 0)
		{
			selectedSlot_ = 9;
		}
	}
	else if (mouseWheel < 0)
	{
		selectedSlot_++;
		if (selectedSlot_ > 9)
		{
			selectedSlot_ = 0;
		}
	}

	if (Game::Input::Key::IsJustPressed(DIK_1))
	{
		selectedSlot_ = 0;
	}
	else if (Game::Input::Key::IsJustPressed(DIK_2))
	{
		selectedSlot_ = 1;
	}
	else if (Game::Input::Key::IsJustPressed(DIK_3))
	{
		selectedSlot_ = 2;
	}
	else if (Game::Input::Key::IsJustPressed(DIK_4))
	{
		selectedSlot_ = 3;
	}
	else if (Game::Input::Key::IsJustPressed(DIK_5))
	{
		selectedSlot_ = 4;
	}
	else if (Game::Input::Key::IsJustPressed(DIK_6))
	{
		selectedSlot_ = 5;
	}
	else if (Game::Input::Key::IsJustPressed(DIK_7))
	{
		selectedSlot_ = 6;
	}
	else if (Game::Input::Key::IsJustPressed(DIK_8))
	{
		selectedSlot_ = 7;
	}
	else if (Game::Input::Key::IsJustPressed(DIK_9))
	{
		selectedSlot_ = 8;
	}
	else if (Game::Input::Key::IsJustPressed(DIK_0))
	{
		selectedSlot_ = 9;
	}
}

void Itemslot::Draw()
{
	for (int i = 0; i < 10; i++)
	{
		slotSprite_[i].Draw();
		itemSprite_[i].Draw();
	}
}

void Itemslot::AddItemToItemslot(int itemID)
{

}