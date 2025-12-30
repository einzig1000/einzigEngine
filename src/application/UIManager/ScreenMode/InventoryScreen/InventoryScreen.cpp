#include "UIManager/ScreenMode/InventoryScreen/InventoryScreen.h"
#include "UIManager/ScreenMode/UIElement/Inventory/Inventory.h"

InventoryScreen::InventoryScreen()
{
	// uiElements_[0] : Inventry
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
	for (const auto& element : uiElements_)
	{
		element->Draw();
	}
}
