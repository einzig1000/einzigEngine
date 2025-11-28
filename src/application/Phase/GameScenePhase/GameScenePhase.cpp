#include "GameScenePhase.h"
#include "MapManager.h"
#include "Player.h"
#include <fstream>

GameScenePhase::GameScenePhase()
{
	player_ = new Player();

	map_ = new MapManager(player_);
	map_->LoadMap("resources/Map/map1x1.csv");
	//map_->LoadMap("resources/Map/map.csv");
	//map_->LoadMap("resources/Map/mapFlat.csv");
}

GameScenePhase::~GameScenePhase()
{
	delete map_;
	map_ = nullptr;

	delete player_;
	player_ = nullptr;
}

void GameScenePhase::Initialize()
{
	nextPhase = PHASE::Phase_None;

	map_->Initialize();
	player_->Initialize();

	Game::Camera::SetCurrentOrbitMode(false);
	//Game::Input::Mouse::ToggleMouseCursorVisible();
}


void GameScenePhase::Update()
{
	map_->Update();

	player_->Update();
}


void GameScenePhase::Draw()
{
	map_->Draw();

	player_->Draw();
}




