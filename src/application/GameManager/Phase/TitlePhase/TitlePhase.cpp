#include "TitlePhase.h"


TitlePhase::TitlePhase()
{
	worldSelect = std::make_unique<RenderData_Sprite>();
	options = std::make_unique<RenderData_Sprite>();
}

TitlePhase::~TitlePhase()
{}

void TitlePhase::Initialize()
{
	nextPhase_ = PHASE::Phase_None;
	currentState = TitlePhaseState::None;
	nextState = TitlePhaseState::Title;

	worldSelect->texture = ResourceID::GetUITextureID(UITextureID::TITLE_start);
	worldSelect->anchor = Anchor::Center;
	worldSelect->name = "WorldSelectButton";
	worldSelect->transforms.translate = { 640.0f,300.0f,0.0f };
	worldSelect->color = 0x777777FF;

	options->texture = ResourceID::GetUITextureID(UITextureID::TITLE_option);
	options->anchor = Anchor::Center;
	options->name = "OptionsButton";
	options->transforms.translate = { 640.0f,500.0f,0.0f };
	options->color = 0x777777FF;

	Game::Input::Mouse::ShowCursor(true);
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
		if (worldSelect->isCollisionMouseRay)
		{
			worldSelect->color = 0xFFFFFFFF;
			if (Game::Input::Mouse::IsHeld(0))
			{
				nextState = TitlePhaseState::WorldSelect;
				nextPhase_ = PHASE::Phase_GameScene;
			}
		}
		else
		{
			worldSelect->color = 0x777777FF;
		}
		if (options->isCollisionMouseRay)
		{
			options->color = 0xFFFFFFFF;
			//if (Game::Input::Mouse::IsHeld(0))
			//{
			//	nextState = TitlePhaseState::Options;
			//}
		}
		else
		{
			options->color = 0x777777FF;
		}

		worldSelect->transforms.scale = { 1.0f + 0.05f * std::sin(frameCount * 0.1f), 1.0f + 0.05f * std::sin(frameCount * 0.1f), 1.0f };
		options->transforms.scale = { 1.0f + 0.05f * std::sin(frameCount * 0.1f), 1.0f + 0.05f * std::sin(frameCount * 0.1f), 1.0f };

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
	{
		break;
	}
	case TitlePhaseState::Title:
	{
		worldSelect->Draw();
		options->Draw();
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


void TitlePhase::DrawImGui()
{}
