#include "UIManager/UIManager.h"
#include "UIManager/ScreenMode/PlayingScreen/PlayingScreen.h"
#include "UIManager/ScreenMode/InventoryScreen/InventoryScreen.h"

UIManager::UIManager(Player* player)
{}

UIManager::~UIManager()
{
	delete currentScreen_;
	currentScreen_ = nullptr;
}

void UIManager::Initialize()
{
	currentScreen_ = new PlayingScreen();
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
			CreateScreen(nextMode);
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

void UIManager::CreateScreen(UIMode mode)
{
	// 画面破棄
	delete currentScreen_;
	currentScreen_ = nullptr;
	// 新画面生成

	switch (mode)
	{
	case UIMode::Hidden:
		break;
	case UIMode::Playing:
		currentScreen_ = new PlayingScreen();
		break;
	case UIMode::Inventory:
		currentScreen_ = new InventoryScreen();
		break;
	case UIMode::Pause:
		break;
	default:
		break;
	}

	if (currentScreen_)
	{
		currentScreen_->Initialize();
	}
}
