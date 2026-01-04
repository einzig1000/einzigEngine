#include "UIManager/UIManager.h"
#include "UIManager/ScreenMode/PlayingScreen/PlayingScreen.h"
#include "UIManager/ScreenMode/InventoryScreen/InventoryScreen.h"
#include "UIManager/ScreenMode/CraftScreen/CraftScreen.h"


UIManager::UIManager(Player* player)
	: player_(player)
{
	playingScreen_ = new PlayingScreen();
	playingScreen_->SetPlayer(player_);

	inventoryScreen_ = new InventoryScreen();
	inventoryScreen_->SetPlayer(player_);

	craftingScreen_ = new CraftScreen();
	craftingScreen_->SetPlayer(player_);
}

UIManager::~UIManager()
{
	delete playingScreen_;
	playingScreen_ = nullptr;

	delete inventoryScreen_;
	inventoryScreen_ = nullptr;

	delete craftingScreen_;
	craftingScreen_ = nullptr;
}

void UIManager::Initialize()
{
	ChangeScreen(UIMode::Playing);
}

void UIManager::Update()
{
	if (currentScreen_)
	{
		currentScreen_->Update();
		// 画面遷移確認
		UIMode nextMode = currentScreen_->GetNextUIMode();
		if (nextMode != UIMode::None)
		{
			// 新画面生成
			ChangeScreen(nextMode);
		}
	}
}

void UIManager::Draw()
{
	if (currentScreen_)
	{
		currentScreen_->Draw();
	}
}

void UIManager::DrawImGui()
{}

void UIManager::ChangeScreen(UIMode mode)
{
	// 現在の画面破棄
	if (currentScreen_)
	{
		currentScreen_ = nullptr;
	}

	// 新しい画面生成
	switch (mode)
	{
	case UIMode::Playing:
		currentScreen_ = playingScreen_;
		break;
	case UIMode::Inventory:
		currentScreen_ = inventoryScreen_;
		break;
	case UIMode::Crafting:
		currentScreen_ = craftingScreen_;
		break;
	default:
		currentScreen_ = nullptr;
		break;
	}

	currentUIMode_ = mode;

	if (currentScreen_)
	{
		currentScreen_->Initialize();
	}
}
