#include "UIManager/ScreenMode/PlayingScreen/PlayingScreen.h"
#include "UIManager/ScreenMode/UIElement/UIElement.h"

PlayingScreen::PlayingScreen()
{
}

PlayingScreen::~PlayingScreen()
{
}

void PlayingScreen::Initialize()
{
	nextUIMode_ = UIMode::None;

	for (const auto& element : uiElements_)
	{
		element->Initialize();
	}
}

void PlayingScreen::Update()
{
	for (const auto& element : uiElements_)
	{
		element->Update();
	}

	if (Game::Input::Key::IsJustPressed(DIK_E))
	{
		nextUIMode_ = UIMode::Inventory;
	}
}

void PlayingScreen::Draw()
{
	for (const auto& element : uiElements_)
	{
		element->Draw();
	}
}