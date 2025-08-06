#include "UnitOverview.h"
#include "CharacterManager.h"
#include "Map.h"

UnitOverview::UnitOverview(CharacterManager* characterManager)
	: characterManager_(characterManager)
{
	nextPhase = PHASE::Phase_None;

	map_ = new Map();
}

UnitOverview::~UnitOverview()
{
	delete map_;
	map_ = nullptr;
}


void UnitOverview::Initialize()
{
	nextPhase = PHASE::Phase_None;

	map_->LoadMap(0);
	targetY = 0;

	int x = 0;
	int y = 0;
	for (uint32_t i = 0; i < characterManager_->GetAllCharactor().size(); ++i)
	{
		characterManager_->GetAllCharactor()[i]->data.transforms.translate = PositionByIndex({y,x});
		x++;
		if (x >= MAP_HEIGHT)
		{
			x = 0;
			y ++;
			if (y >= MAP_WIDTH)
			{

			}
		}
	}

	Game::MoveCenterTarget({ 0.0f, -0.0f, -5.390f }, 200);
	Game::MoveRotateTarget({ 0.3f, -std::numbers::pi / 2.0f, 0.0f }, 200);
	Game::MoveDistanceTarget(15.60f, 200);
}

void UnitOverview::Update()
{
	map_->Update();

	int pre = Game::GetMouseWheel();

	if (Game::GetMouseWheel() > 0 && pre == 0)
	{
		for (int y = 0; y < MAP_HEIGHT; ++y)
		{
			for (int x = 0; x < MAP_WIDTH; ++x)
			{
				map_->data[y][x].transforms.translate.z -= 10;
				if (x <= targetY)
				{
					map_->data[y][x].transforms.translate.z -= 30;
				}
			}
		}
	}
	ImGui::Begin("view");
	ImGui::Text("%d", Game::GetMouseWheel());
	ImGui::End();
}

void UnitOverview::Draw()
{
	map_->Draw();

	// キャラの描画
	for (uint32_t i = 0; i < characterManager_->GetAllCharactor().size(); ++i)
	{
		characterManager_->GetAllCharactor()[i]->data.Draw();
	}
}