#include "UIManager/ScreenMode/UIElement/Hotbar/Hotbar.h"

Hotbar::Hotbar()
{
	// sprites_[0] : ホットバー
	sprites_.emplace_back(std::make_unique<RenderData_Sprite>());
	sprites_[0]->texture = ResourceID::GetTextureID(UITextureID::Hotbar);
	sprites_[0]->anchor = Anchor::Center;
	sprites_[0]->transforms.scale = Vector3(1.0f, 1.0f, 1.0f);
	sprites_[0]->transforms.translate = Vector3(640.0f, 670.0f, 0.0f);

	// sprites_[1] : ホットバー選択枠
	sprites_.emplace_back(std::make_unique<RenderData_Sprite>());
	sprites_[1]->texture = ResourceID::GetTextureID(UITextureID::Hotbar_Selected);
	sprites_[1]->anchor = Anchor::Center;
	sprites_[1]->transforms.scale = Vector3(1.0f, 1.0f, 1.0f);
	sprites_[1]->transforms.translate = Vector3(640.0f, 670.0f, 0.0f);
}

Hotbar::~Hotbar()
{}

void Hotbar::Initialize()
{}

void Hotbar::Update()
{
	int mouseWheel = Game::Input::Mouse::GetWheel();
	if (mouseWheel > 0)
	{
		selectedIndex_--;
		if (selectedIndex_ < 0)
		{
			selectedIndex_ = 8;
		}
	}
	else if (mouseWheel < 0)
	{
		selectedIndex_++;
		if (selectedIndex_ > 8)
		{
			selectedIndex_ = 0;
		}
	}

	if (Game::Input::Key::IsJustPressed(DIK_1))
	{
		selectedIndex_ = 0;
	}
	else if (Game::Input::Key::IsJustPressed(DIK_2))
	{
		selectedIndex_ = 1;
	}
	else if (Game::Input::Key::IsJustPressed(DIK_3))
	{
		selectedIndex_ = 2;
	}
	else if (Game::Input::Key::IsJustPressed(DIK_4))
	{
		selectedIndex_ = 3;
	}
	else if (Game::Input::Key::IsJustPressed(DIK_5))
	{
		selectedIndex_ = 4;
	}
	else if (Game::Input::Key::IsJustPressed(DIK_6))
	{
		selectedIndex_ = 5;
	}
	else if (Game::Input::Key::IsJustPressed(DIK_7))
	{
		selectedIndex_ = 6;
	}
	else if (Game::Input::Key::IsJustPressed(DIK_8))
	{
		selectedIndex_ = 7;
	}
	else if (Game::Input::Key::IsJustPressed(DIK_9))
	{
		selectedIndex_ = 8;
	}

	// 選択枠位置更新
	sprites_[1]->transforms.translate.x = 320.0f + selectedIndex_ * 80.0f;
}

void Hotbar::Draw()
{
	for (const auto& sprite : sprites_)
	{
		sprite->Draw();
	}
}
