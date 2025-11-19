#include "TitlePhase.h"
#include "ResourceID.h"


void TitlePhase::Initialize()
{
	nextPhase = PHASE::Phase_None;
	currentState = TitlePhaseState::None;
	nextState = TitlePhaseState::Title;

	worldSelect.texture = ResourceID::TextureIDs_[int(TextureID::start_title)];
	worldSelect.anchor = Anchor::Center;
	worldSelect.name = "WorldSelectButton";
	worldSelect.transforms.translate = { 640.0f,300.0f,0.0f };
	worldSelect.color = 0x777777FF;

	options.texture = ResourceID::TextureIDs_[int(TextureID::Options_title)];
	options.anchor = Anchor::Center;
	options.name = "OptionsButton";
	options.transforms.translate = { 640.0f,500.0f,0.0f };
	options.color = 0x777777FF;

	//Game::Input::Mouse::ToggleMouseCursorVisible();
}


void TitlePhase::Update()
{
	if (nextState != TitlePhaseState::None)
	{
		currentState = nextState;
		switch (currentState)
		{
		case TitlePhaseState::Title:
			break;
		case TitlePhaseState::WorldSelect:
			break;
		case TitlePhaseState::Options:
			break;
		default:
			break;
		}
		nextState = TitlePhaseState::None;
	}

	switch (currentState)
	{
	case TitlePhaseState::None:
	{
		break;
	}
	case TitlePhaseState::Title:
	{
		if (worldSelect.isCollisionMouseRay)
		{
			worldSelect.color = 0xFFFFFFFF;
			if (Game::Input::Mouse::IsHeld(0))
			{
				nextState = TitlePhaseState::WorldSelect;
				nextPhase = PHASE::Phase_GameScene;
			}
		}
		else
		{
			worldSelect.color = 0x777777FF;
		}
		if (options.isCollisionMouseRay)
		{
			options.color = 0xFFFFFFFF;
			if (Game::Input::Mouse::IsHeld(0))
			{
				nextState = TitlePhaseState::Options;
			}
		}
		else
		{
			options.color = 0x777777FF;
		}

		worldSelect.transforms.scale = { 1.0f + 0.05f * std::sin(frameCount * 0.1f), 1.0f + 0.05f * std::sin(frameCount * 0.1f), 1.0f };
		options.transforms.scale = { 1.0f + 0.05f * std::sin(frameCount * 0.1f), 1.0f + 0.05f * std::sin(frameCount * 0.1f), 1.0f };

		break;
	}
	case TitlePhaseState::WorldSelect:
	{
		break;
	}
	case TitlePhaseState::Options:
	{
		break;
	}
	default:
	{
		break;
	}
	}

	frameCount++;
}


void TitlePhase::Draw()
{
	switch (currentState)
	{
	case TitlePhaseState::None:
		break;
	case TitlePhaseState::Title:
	{
		worldSelect.Draw();
		worldSelect.DrawImGui();
		options.Draw();
		options.DrawImGui();
		break;
	}
	case TitlePhaseState::WorldSelect:
	{
		break;
	}
	case TitlePhaseState::Options:
	{
		break;
	}
	default:
		break;
	}

}