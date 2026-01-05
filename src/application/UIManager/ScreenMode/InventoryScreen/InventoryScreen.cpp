#include "UIManager/ScreenMode/InventoryScreen/InventoryScreen.h"
#include "UIManager/ScreenMode/UIElement/Inventory/Inventory.h"
#include "UIManager/ScreenMode/UIElement/Hotbar/Hotbar.h"
#include "Charactor/Player/Player.h"

InventoryScreen::InventoryScreen()
{
	// uiElements_[0] : Hotbar
	uiElements_.emplace_back(std::make_unique<Hotbar>());
	// uiElements_[1] : Inventry
	uiElements_.emplace_back(std::make_unique<Inventory>());
}

InventoryScreen::~InventoryScreen()
{}

void InventoryScreen::Initialize()
{
	nextUIMode_ = UIMode::None;

	for (const auto& element : uiElements_)
	{
		element->Initialize();
	}

	// カーソル操作有効化
	Game::Input::Mouse::ShowCursor(true);
}

void InventoryScreen::Update()
{
	for (const auto& element : uiElements_)
	{
		element->Update();
	}

	if (Game::Input::Key::IsJustPressed(DIK_ESCAPE) ||
		Game::Input::Key::IsJustPressed(DIK_E))
	{
		nextUIMode_ = UIMode::Playing;
	}
}

void InventoryScreen::Draw()
{
	uiElements_[0]->Draw();		// Hotbar

	player_->DrawHotbar();		// Hotbarアイコン描画

	uiElements_[1]->Draw();		// Inventory

	player_->DrawInventory();	// Inventoryアイコン描画
}
